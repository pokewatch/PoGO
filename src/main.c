/*
TODO:
  Sort based on distance realtime (since walking updates GPS)
  Have "expired" be removed after a few seconds
  Select to remove selected pokemon (caught!)
  Make sure same pokemon is selected, despite
    - expired removing it
    - reordered based on distance
    - 
Status Bar Layer should show #/MAX pokemon on upper-right corner

Diorite: No Compass
  Remove pointing arrow
  Or just have it point statically based on north = up
  Or have it say "N" "NW" "SSW"
  



*/

#include <pebble.h>
#include "pokedex.h"

// TODO: replace crude hack with https://github.com/smallstoneapps/data-processor ;)

// ------------------------------------------------------------------------ //
//  Global Variables, Structs and Constants
// ------------------------------------------------------------------------ //

#define KEY_POKEMON_ID 0
#define KEY_POKEMON_EXPIRATION_TIME 1
#define KEY_POKEMON_LATITUDE 2
#define KEY_POKEMON_LONGITUDE 3

#define KEY_USER_LATITUDE 4
#define KEY_USER_LONGITUDE 5

#define KEY_DISPLAY_MESSAGE 6

#define KEY_REQUEST_TYPE 7


// Measured in seconds:
#define TIME_TO_CLEAR_AFTER_CAUGHT 2
#define TIME_TO_CLEAR_AFTER_EXPIRED 10


// Predefined messages
#define MSG_LOADING 0
#define MSG_NO_POKEMON_FOUND 1
#define MSG_FINDING_POKEMON 2

// Compass window (not in this version)
//Window *compass_window;


// Main Window and Layers
Window *list_window;
MenuLayer *menu_layer;
StatusBarLayer *status_bar_layer;
Layer *overlay_layer;
char list_text_buffer[100] = "";
GFont custom_font;


// loading splash screen
Window *splash_window;
GBitmap *splash_bitmap;
BitmapLayer *bitmap_layer;
TextLayer *loading_layer;
bool loading = true;


//alert layer
Layer *alert_layer;
TextLayer *alert_text_layer;
BitmapLayer *alert_background_bitmap_layer;
GBitmap *alert_background_bitmap;
char alert_text_buffer[100] = "";


// Compass and Distance
bool compass_exists = true;   // If a compass exists (i.e. not Diorite)
int32_t compass_heading = 0;

typedef enum {
  BEARING_MODE_INVISIBLE      = 0, // No bearing arrow
  BEARING_MODE_STATIC_ARROW   = 1, // Arrow points toward direction, doesn't rotate with compass
  BEARING_MODE_ROTATING_ARROW = 2, // Normal: Arrows rotate with compass (if compass exists, else STATIC_ARROW)
  BEARING_MODE_CARDINAL       = 3  // Shows cardnal direction (N, S, SW, NNW)
} bearing_mode_enum;
bearing_mode_enum bearing_mode = BEARING_MODE_ROTATING_ARROW;


typedef enum {
  DISTANCE_MODE_INVISBLE   = 0,
  DISTANCE_MODE_METERS     = 1,
  DISTANCE_MODE_FEET       = 2,
  DISTANCE_MODE_FOOTPRINTS = 3, // Footprints
} distance_mode_enum;
distance_mode_enum distance_mode = DISTANCE_MODE_METERS;


// Team Coloration
#define TEAM_NONE 0
#define TEAM_VALOR 1
#define TEAM_MYSTIC 2
#define TEAM_INSTINCT 3
int32_t team = TEAM_MYSTIC;
GBitmap *ui_top_bitmap, *ui_bottom_bitmap;


// User location
typedef struct {
  int32_t lat;
  int32_t lon;
} user_struct;
user_struct user;




// ------------------------------------------------------------------------ //
//  Pokemon Structs and Functions
// ------------------------------------------------------------------------ //
// Maximum number of pokemon to allow on our list
// Currently max of 255 due to using uint8_t, but that should be plenty
#define MAX_POKEMON 100 

typedef struct {
  int32_t lat;
  int32_t lon;
  time_t  expires;   // time_t is int32_t
  int32_t distance;  // only saving to increase sort speed
  int32_t dex;       // only needs to be uint8_t
} pokemon_struct;

pokemon_struct pokemon[MAX_POKEMON];
int32_t NUM_POKEMON = 1;

// TODO: Implement pointer list instead of sorting big struct array
//uint8_t pokemon_list[MAX_POKEMON];

static void remove_pokemon(int32_t index) {
  if (index >= NUM_POKEMON) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR! Index to remove too large: %d", (int)index);
    return;
  }
  if (NUM_POKEMON == 0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR! No pokemon left to remove");
    return;
  }
  
  for(uint8_t i=index; i<NUM_POKEMON-1; i++) {
    pokemon[i] = pokemon[i+1];
  }
  NUM_POKEMON--;
  
  menu_layer_reload_data(menu_layer);
}


static void cleanup_pokemon_list() {
  // Clear Expired
  time_t now = time(NULL);
  for(uint8_t i = 0; i < NUM_POKEMON - 1;) {
    if (pokemon[i].expires + TIME_TO_CLEAR_AFTER_EXPIRED < now)
      remove_pokemon(i);  // function removes item from list, so don't need to increment i
    else
      i++;
  }
  
  // Remove any duplicates (shouldn't happen, but just in case)
  for(uint8_t i = 0; i < NUM_POKEMON - 1; i++) {
    for(uint8_t j = i + 1; j < NUM_POKEMON; ) {
      if(pokemon[i].dex == pokemon[j].dex &&
         pokemon[i].lat == pokemon[j].lat &&
         pokemon[i].lon == pokemon[j].lon
        ) {
        remove_pokemon(j); // function removes item from list, so don't need to increment j
      } else {
        j++;
      }
    }
  }  
}


static void sort_pokemon_by_distance() {
  // Insertion Sort: Sort pokemon in order of distance
  // Chosen by comparing sorting algorithms here: http://www.sorting-algorithms.com
  // Adapted from pseudocode found here: https://en.wikipedia.org/wiki/Insertion_sort
  for(uint8_t i=1; i<NUM_POKEMON; i++) {
    pokemon_struct temp = pokemon[i];
    uint8_t j = i;
    while(j>0 && pokemon[j-1].distance < temp.distance) {
      pokemon[j] = pokemon[j-1];
      --j;
    }
    pokemon[j] = temp;
  }
}


void flag_pokemon_as_caught(int index) {
  if(pokemon[index].lat == 0 && pokemon[index].lon == 0)  // if already flagged
    return;
  pokemon[index].lat = 0;
  pokemon[index].lon = 0;
  time_t now = time(NULL);
  pokemon[index].expires = now - TIME_TO_CLEAR_AFTER_EXPIRED + TIME_TO_CLEAR_AFTER_CAUGHT;
}


// ------------------------------------------------------------------------ //
//  GPS Functions
// ------------------------------------------------------------------------ //
// GPS Constants
// I convert all floating point GPS coordinates to 32bit integers

#define EARTH_RADIUS_IN_METERS 6378137
#define EARTH_CIRCUMFERENCE_IN_METERS 40075016
#define MAX_GPS_ANGLE 8388608    // Equivalent to 360 degrees

// 64bit GPS constants
#define EARTH_RADIUS_IN_METERS_64BIT 6378137LL
#define EARTH_CIRCUMFERENCE_IN_METERS_64BIT 40075016LL
#define MAX_GPS_ANGLE_64BIT 8388608LL
#define TRIG_MAX_RATIO_64BIT 65536LL  // I know it's 65535, but this is close enough


static int32_t get_bearing(int32_t y, int32_t x) {  // Keeping (y, x) notation cause atan2 uses it.  /shrug
  return TRIG_MAX_ANGLE - (atan2_lookup(y, x) - (TRIG_MAX_ANGLE / 4));
}

#define root_depth 10          // How many iterations square root function performs
int32_t  sqrt32(int32_t a) {int32_t b=a; for(int8_t i=0; i<root_depth; i++) b=(b+(a/b))/2; return b;} // Square Root

static int32_t gps_distance(int32_t lat_distance_in_meters, int32_t lon_distance_in_meters) {
  return sqrt32((lat_distance_in_meters*lat_distance_in_meters)+(lon_distance_in_meters*lon_distance_in_meters));
  
}



// Returns vertical distance (in meters) between two latitudes
static int32_t gps_lat_distance(int32_t lat1, int32_t lat2) {
  //int32_t lat_distance = EARTH_CIRCUMFERENCE_IN_METERS * (lat2 - lat1) / MAX_GPS_ANGLE;
  int32_t lat_distance = (
    (
      EARTH_CIRCUMFERENCE_IN_METERS_64BIT *
      (int64_t) (lat2 - lat1)
    ) / (
      MAX_GPS_ANGLE_64BIT
    )
  );
  return lat_distance;
}



// Returns horizontal distance (in meters) between two longitudinal points at a specific latitude
static int32_t gps_lon_distance(int32_t lat, int32_t lon1, int32_t lon2) {
  //return cos_lookup((map_lat/128)) * EARTH_CIRCUMFERENCE_IN_METERS * (pos_lon - map_lon) / (MAX_GPS_ANGLE * TRIG_MAX_RATIO);
  int32_t lon_distance = (
    (
      (int64_t) cos_lookup(lat/128) *
      EARTH_CIRCUMFERENCE_IN_METERS_64BIT *
      (int64_t) (lon2 - lon1)
    ) / (
      MAX_GPS_ANGLE_64BIT *
      TRIG_MAX_RATIO_64BIT
    )
  );
  return lon_distance;
}


  
// ------------------------------------------------------------------------ //
//  Layer Drawing Functions
// ------------------------------------------------------------------------ //
void draw_overlay_layer(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w,16), 0, GCornerNone);

  #ifdef PBL_RECT
  graphics_context_set_stroke_color(ctx, GColorWhite);
  for (int32_t i = 0; i < bounds.size.w; i++) {
    if (i%2 == 0)
      graphics_draw_pixel(ctx, GPoint(i, 15));
  }
  #endif


  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, ui_top_bitmap, GRect(PBL_IF_RECT_ELSE(0, 18), 16, 144, 11));
  graphics_draw_bitmap_in_rect(ctx, ui_bottom_bitmap, GRect(PBL_IF_RECT_ELSE(0, 18), bounds.size.h - 11, 144, 11));

  // Draw Clock
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, "     ", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, PBL_IF_RECT_ELSE(-2,2), bounds.size.w, 16), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

void remove_splash_screen() {
  if (loading) {
    window_stack_push(list_window, true);
    layer_add_child(overlay_layer, alert_layer);  // Move alert layer to main window
    vibes_short_pulse();
    loading = false;
  }
}

// ------------------------------------------------------------------------ //
//  Menu Functions
// ------------------------------------------------------------------------ //
static const GPathInfo MINI_COMPASS_INFO = {
  .num_points = 4,
  .points = (GPoint []) { {0,-9}, {-6,9}, {0,3}, {6,9} }
};

int16_t menu_get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 20;  // Height of both header at top, and footer at bottom (footer = 2nd header)
}


uint16_t menu_get_num_sections(struct MenuLayer *menu_layer, void *callback_context) {
  return 2;
}

static uint16_t get_num_pokemon(MenuLayer *menu_layer, uint16_t section_index, void *data){
  if (section_index == 0)
    return NUM_POKEMON;
  // Second section is just a header with 0 rows under it (so return 0), therefore: it's just a footer
  return 0;
}

static int16_t pokemon_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
  return 60;
}


void menu_draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
}
  
void draw_pokemon(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data){
  
  if(menu_cell_layer_is_highlighted(cell_layer)){
    graphics_context_set_fill_color(ctx, GColorLightGray);
    graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
    #ifndef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, GPoint(2, 4), GPoint(2, 56));
    #endif
  }
  

  GBitmap *sprite = NULL;
  int pokemon_index = index->row;
  int32_t bearing = 0;
  bool bearing_valid = false;
  
  // ---------------------
  // Prepare Icon, Compass and Text
  // ---------------------
  graphics_context_set_text_color(ctx, GColorBlack);
  // NUM_POKEMON=1 and DEX=0 means "0 Pokemon, Display Message instead"
  if(NUM_POKEMON == 1 && pokemon[0].dex == 0) {
    // When in "Display Message" mode, EXPIRES denotes which message
    if(pokemon[0].expires == MSG_LOADING)
      strncpy(list_text_buffer, "Loading...", sizeof(list_text_buffer));
    else if(pokemon[0].expires == MSG_NO_POKEMON_FOUND)
      strncpy(list_text_buffer, "No\nPokemon\nFound", sizeof(list_text_buffer));
    else if(pokemon[0].expires == MSG_FINDING_POKEMON)
      strncpy(list_text_buffer, "Finding\nPokemon...", sizeof(list_text_buffer));
    else
      strncpy(list_text_buffer, "Error", sizeof(list_text_buffer));
      
    sprite = gbitmap_create_with_resource(poke_images[0]);  // load sprite 0 (pokeball image)
    
  } else if(pokemon[pokemon_index].dex < 151 && pokemon[pokemon_index].dex>=0) {    //if dex is between 0 and 151:
    sprite = gbitmap_create_with_resource(poke_images[pokemon[pokemon_index].dex]);

    time_t now = time(NULL);
    
    int32_t expiration_delta = pokemon[pokemon_index].expires - now;
    if(pokemon[pokemon_index].lat == 0 && pokemon[pokemon_index].lon == 0) {
      strncpy(list_text_buffer, "Caught!", sizeof(list_text_buffer));
    } else if (expiration_delta < 0) {
      strncpy(list_text_buffer, "Expired!", sizeof(list_text_buffer));
    } else {
      int32_t y = gps_lat_distance(user.lat, pokemon[pokemon_index].lat);
      int32_t x = gps_lon_distance(user.lat, user.lon, pokemon[pokemon_index].lon);
      pokemon[pokemon_index].distance = gps_distance(y, x);
      //bearing = get_bearing(y, x);
      bearing = TRIG_MAX_ANGLE - (atan2_lookup(y, x) - (TRIG_MAX_ANGLE / 4));
      bearing_valid = true;
      
      snprintf(list_text_buffer, sizeof(list_text_buffer),
               "%s\n%d:%02d\n%d m",
               poke_names[pokemon[pokemon_index].dex],
               (int)expiration_delta / 60,
               (int)expiration_delta % 60,
               (int)pokemon[pokemon_index].distance
              );
      
    }
  } else { //else dex is not between 0 and 151:
    sprite = gbitmap_create_with_resource(poke_images[0]);  // load sprite 0 (pokeball image)
    strncpy(list_text_buffer, "Unknown", sizeof(list_text_buffer));
  }
  
  
  // --------------------- //
  // Draw Bearing GPath
  // --------------------- //
  if(bearing_valid)
    switch (bearing_mode) {
      case BEARING_MODE_ROTATING_ARROW:
        bearing = bearing  + compass_heading;  // Move arrow with compass
        // no break;, falls thru to display arrow

      case BEARING_MODE_STATIC_ARROW: {  // I dunno why I gotta have braces here, cloudpebble complains otherwise
        GPath *compass_gpath = gpath_create(&MINI_COMPASS_INFO);
        gpath_move_to(compass_gpath, GPoint(124, 40));
        gpath_rotate_to(compass_gpath, bearing);
        graphics_context_set_fill_color(ctx, GColorBlack);
        gpath_draw_filled(ctx, compass_gpath);
        gpath_destroy(compass_gpath);
        break;
      }

      case BEARING_MODE_CARDINAL:
        // TODO: North, south, south south west, etc
      break;

      default:
        // invisible or other
      break;
    }

  // --------------------- //
  // Draw Pokemon Sprite
  // --------------------- //
  if (sprite) {
    GRect bounds = gbitmap_get_bounds(sprite);
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, sprite, GRect(2+30-(bounds.size.w/2), 30-(bounds.size.h/2), bounds.size.w, bounds.size.h));
    gbitmap_destroy(sprite);
  }

  // --------------------- //
  // Draw Text Next to Sprite
  // --------------------- //
  graphics_draw_text(ctx, list_text_buffer, custom_font, GRect(64, 14, 180, 30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

// ------------------------------------------------------------------------ //
//  Button Functions
// ------------------------------------------------------------------------ //
// SELECT button was clicked
void sl_click_handler(ClickRecognizerRef ref, void *context) {
  MenuIndex current = menu_layer_get_selected_index(menu_layer);
  //remove_pokemon(current.row);
  if(NUM_POKEMON != 1 || pokemon[0].dex != 0)  // if not in "message" mode
    flag_pokemon_as_caught(current.row);
}

//  DOWN  button was clicked
void dn_click_handler(ClickRecognizerRef ref, void *context) {
  MenuIndex current = menu_layer_get_selected_index(menu_layer);
  if(current.row + 1 >= NUM_POKEMON)
    return;
  else
    menu_layer_set_selected_next(menu_layer, false, MenuRowAlignCenter, true);
}

//   UP  button was clicked
void up_click_handler(ClickRecognizerRef ref, void *context) {
  MenuIndex current = menu_layer_get_selected_index(menu_layer);
  if(current.row == 0) return;
  else
    menu_layer_set_selected_next(menu_layer, true, MenuRowAlignCenter, true);
}

//  BACK  button was clicked
void bk_click_handler  (ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(true);  // Quit
}

void config(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, dn_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, sl_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, bk_click_handler);
}



// ------------------------------------------------------------------------ //
//  Toast Functions
// ------------------------------------------------------------------------ //
void stopped(Animation *anim, bool finished, void *context){
  property_animation_destroy((PropertyAnimation*) anim);
}


void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
  PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

  animation_set_duration((Animation*) anim, duration);
  animation_set_delay((Animation*) anim, delay);

  AnimationHandlers handlers = {
    .stopped = (AnimationStoppedHandler) stopped
  };
  animation_set_handlers((Animation*) anim, handlers, NULL);

  animation_schedule((Animation*) anim);
}


void displayToast(char *string) {
  static GRect from_frame, to_frame;
  from_frame =  GRect(PBL_IF_ROUND_ELSE(40, 22), 180, 100, 100);
  to_frame = GRect(PBL_IF_ROUND_ELSE(40, 22), 34, 100, 100);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Toast: \"%s\"", string);
  strncpy(alert_text_buffer, string, sizeof(alert_text_buffer));
  text_layer_set_text(alert_text_layer, alert_text_buffer);

  vibes_double_pulse();

  animate_layer(alert_layer, &from_frame, &to_frame, 1000, 0);
  animate_layer(alert_layer, &to_frame, &from_frame, 1000, 3000);
}



// ------------------------------------------------------------------------ //
//  AppMessage Communication Functions
// ------------------------------------------------------------------------ //
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // TODO: use https://github.com/smallstoneapps/data-processor instead of this quick crude hack!
  //  APP_LOG(APP_LOG_LEVEL_INFO, "App Message received!");

  Tuple *display_message_tuple,
  *pokemon_id_tuple,
  *pokemon_expiration_tuple,
  *pokemon_latitude_tuple,
  *pokemon_longitude_tuple,
  *user_latitude_tuple,
  *user_longitude_tuple;


  // If app message is: a DISPLAY MESSAGE
  if ((display_message_tuple = dict_find(iterator, KEY_DISPLAY_MESSAGE))) {
    if(display_message_tuple->value->cstring) {
      displayToast(display_message_tuple->value->cstring);
    }
  }

  
  // If app message is: USER GPS LOCATION
  if ((user_latitude_tuple = dict_find(iterator, KEY_USER_LATITUDE)) &&
      (user_longitude_tuple = dict_find(iterator, KEY_USER_LONGITUDE))) {
    user.lat = user_latitude_tuple->value->int32;
    user.lon = user_longitude_tuple->value->int32;
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "New User GPS: (%d, %d)", (int)user.lat, (int)user.lon);
    if(NUM_POKEMON == 1 && pokemon[0].dex == 0) {  // If currently in "No pokemon" / "message" mode
      //if (pokemon[0].expires == MSG_LOADING) pokemon[0].expires = MSG_FINDING_POKEMON;  // set message (using expires) to 
    } else {
      sort_pokemon_by_distance();  // new user location means distances changed!
    }
  }
  

  // If app message is: A POKEMON
  if ((pokemon_id_tuple = dict_find(iterator, KEY_POKEMON_ID))) {
    int32_t pokemon_id = pokemon_id_tuple->value->int32;
    time_t now = time(NULL);
    // Initialize variables with default values in case tuples don't exist
    int32_t pokemon_longitude = 0;
    int32_t pokemon_latitude = 0;
    int32_t pokemon_distance = -1;
    int32_t pokemon_expiration = now;

    if ((pokemon_expiration_tuple = dict_find(iterator, KEY_POKEMON_EXPIRATION_TIME)))
      pokemon_expiration = pokemon_expiration_tuple->value->int32;

    if ((pokemon_latitude_tuple = dict_find(iterator, KEY_POKEMON_LATITUDE)))
      pokemon_latitude = pokemon_latitude_tuple->value->int32;

    if ((pokemon_longitude_tuple = dict_find(iterator, KEY_POKEMON_LONGITUDE)))
      pokemon_longitude = pokemon_longitude_tuple->value->int32;
    
    int32_t y = gps_lat_distance(user.lat, pokemon_latitude);
    int32_t x = gps_lon_distance(user.lat, user.lon, pokemon_longitude);
    pokemon_distance = gps_distance(y, x);
  
    APP_LOG(APP_LOG_LEVEL_DEBUG,
            "PEB Pokemon: ID %d (%d, %d), Dist:%d (%d, %d)",
            (int)pokemon_id,
            (int)pokemon_latitude,
            (int)pokemon_longitude,
            (int)pokemon_distance,
            (int)x,
            (int)y
           );

    if(NUM_POKEMON == 1 && pokemon[0].dex == 0) {  // If currently in "No pokemon" / "message" mode
      if (pokemon_id == 0) {         // Received a "no pokemon found" message
        pokemon[0].expires = MSG_NO_POKEMON_FOUND;  // using expires to denote a message when dex=0
        if(loading) remove_splash_screen();  // No pokemon found!  Remove splash screen to display message
        return;
      } else {
        NUM_POKEMON = 0;  // ID doesn't = 0, so get rid of "Loading..." or "No pokemon" message. We found pokemon!
      }
    }
    
    if (pokemon_id == 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "No additional pokemon found.");
      return;
    }
    
    if (pokemon_id <= 0 || pokemon_id > 151) {  // = 0 shouldn't happen, already caught above
      APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR! Invalid Pokemon ID = %d", (int)pokemon_id);
      return;
    }
    
    if (pokemon_latitude==0 && pokemon_longitude==0) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR! Bad Pokemon Location");
      return;
    }
    
    if (pokemon_expiration < now) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR! Pokemon already expired!");
      return;
    }
    
    if (NUM_POKEMON == MAX_POKEMON)  // If we've maxed out our list, then don't add it
      return;
    
    if (pokemon_distance < 0)        // I'm not even sure how this is possible...
      return;
    
    
    // Passed all the error traps above!  Add the new pokemon to the list!
    pokemon[NUM_POKEMON].dex      = pokemon_id;
    pokemon[NUM_POKEMON].lat      = pokemon_latitude;
    pokemon[NUM_POKEMON].lon      = pokemon_longitude;
    pokemon[NUM_POKEMON].expires  = pokemon_expiration;
    pokemon[NUM_POKEMON].distance = pokemon_distance;
    NUM_POKEMON++;
    
    if(loading) remove_splash_screen();  // Pokemon found!  Remove splash screen
  }

  menu_layer_reload_data(menu_layer);
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void register_callbacks(){
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  // TODO: sizes?
  app_message_open(2048,2048);
}



// ------------------------------------------------------------------------ //
//  Callback Handler Functions
// ------------------------------------------------------------------------ //
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Iterate through all pokemon, remove old dead, update cell position
  // Refresh countdown and compass once per second
  cleanup_pokemon_list();
  layer_mark_dirty(window_get_root_layer(list_window));
}


static void bluetooth_callback(bool connected) {
  if(connected) {
    displayToast("Bluetooth reconnected");
  } else {
    displayToast("Bluetooth disconnected");
  }
}

void compass_handler(CompassHeadingData heading){
  compass_heading = heading.true_heading;
  // Commenting out below to save battery.  Tick timer already updates once per second.
  //layer_mark_dirty(window_get_root_layer(list_window));  // Refresh every time compass updates
  //layer_mark_dirty(window_get_root_layer(compass_window));  // TODO: Compass Window?
}



// ------------------------------------------------------------------------ //
//  Main Functions
// ------------------------------------------------------------------------ //
void init(){
  // ----------------- //
  // Init variables
  // ----------------- //
  team = TEAM_MYSTIC;
  user.lat = 0; user.lon = 0;
  
  // crude loading message
  // NUM_POKEMON=1 and DEX=0 means "message".  EXPIRES=0 means use message "Loading..."
  NUM_POKEMON = 1;
  pokemon[0].dex = 0;
  pokemon[0].expires = MSG_LOADING;  // currently obsolete with splash screen covering loading message
  
  
  // ----------------- //
  //  Setup Handlers
  // ----------------- //
  register_callbacks();

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  connection_service_subscribe((ConnectionHandlers) {.pebble_app_connection_handler = bluetooth_callback});

  compass_service_subscribe(compass_handler);
  
  
  // ----------------- //
  // Header/Footer and Font
  // ----------------- //
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PKMN_10));

  ui_top_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UI_TOP);
  ui_bottom_bitmap = gbitmap_create_with_resource(RESOURCE_ID_UI_BOTTOM);

  #ifdef PBL_COLOR
  GColor *pal_top = gbitmap_get_palette(ui_top_bitmap);
  GColor *pal_bottom = gbitmap_get_palette(ui_bottom_bitmap);

  switch (team) {
    case TEAM_NONE:
    pal_top[0] = GColorDarkGray;
    pal_bottom[2] = GColorDarkGray;
    break;

    case TEAM_MYSTIC:
    pal_top[0] = GColorCobaltBlue;
    pal_bottom[2] = GColorCobaltBlue;
    break;

    case TEAM_INSTINCT:
    pal_top[0] = GColorOrange;
    pal_bottom[2] = GColorOrange;
    break;
  }
  #endif

  
  
  
  // ---------------------------------- //
  // Setup splash window
  // ---------------------------------- //
	splash_window = window_create();
	window_set_background_color(splash_window, GColorBlack);
  window_stack_push(splash_window, true);

  // ----------------- //
  // Splash Bitmap Layer
  // ----------------- //
	splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_POGO);
	GRect window_bounds = layer_get_bounds(window_get_root_layer(splash_window));
	GRect bitmap_bounds = gbitmap_get_bounds(splash_bitmap);
	bitmap_layer = bitmap_layer_create( GRect((window_bounds.size.w - bitmap_bounds.size.w)/2, (window_bounds.size.h - bitmap_bounds.size.h)/2, bitmap_bounds.size.w, bitmap_bounds.size.h) );
	bitmap_layer_set_bitmap(bitmap_layer, splash_bitmap);
	layer_add_child(window_get_root_layer(splash_window), bitmap_layer_get_layer(bitmap_layer));
  
  // ----------------- //
  // Splash Text Layer
  // ----------------- //
	loading_layer = text_layer_create(GRect(0, window_bounds.size.w/2 + bitmap_bounds.size.w/2, window_bounds.size.w, 28));
	text_layer_set_text(loading_layer, "Loading...");
	text_layer_set_background_color(loading_layer, GColorBlack);
	text_layer_set_text_color(loading_layer, GColorWhite);
	text_layer_set_text_alignment(loading_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(splash_window), text_layer_get_layer(loading_layer));
	
  
  
  // ---------------------------------- //
  // Setup Main Window
  // ---------------------------------- //
  list_window = window_create();
  window_set_click_config_provider(list_window, config);
  window_set_background_color(list_window, GColorWhite);
  //window_stack_push(list_window, true);  // Commented out as we now load splash screen first

  
  // ----------------- //
  // Setup menu layer
  // ----------------- //
  menu_layer = menu_layer_create(GRect(0, 16, PBL_IF_RECT_ELSE(144,180), PBL_IF_RECT_ELSE(168,180) - 16));
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .draw_row = draw_pokemon,
    .get_num_rows = get_num_pokemon,
    .get_cell_height = pokemon_cell_height,
    .get_header_height = menu_get_header_height,
    .draw_header = menu_draw_header,
    .get_num_sections = menu_get_num_sections
  });
  //menu_layer_set_click_config_onto_window(menu_layer, list_window);
  layer_add_child(window_get_root_layer(list_window), menu_layer_get_layer(menu_layer));
  menu_layer_reload_data(menu_layer);
  menu_layer_set_selected_index(menu_layer, (MenuIndex){0,0}, MenuRowAlignNone, false);

  
  // ----------------- //
  // Setup overlay layer
  // ----------------- //
  overlay_layer = layer_create(layer_get_bounds(window_get_root_layer(list_window)));
  layer_set_update_proc(overlay_layer, draw_overlay_layer);
  layer_add_child(window_get_root_layer(list_window), overlay_layer);

  
  // ----------------- //
  // Setup status bar
  // ----------------- //
  status_bar_layer = status_bar_layer_create();
  int16_t width = layer_get_bounds(overlay_layer).size.w;
  GRect frame = GRect(0, -1, width, STATUS_BAR_LAYER_HEIGHT); // -1 so the overlay still shows up beneath it
  layer_set_frame(status_bar_layer_get_layer(status_bar_layer), frame);
  status_bar_layer_set_colors(status_bar_layer, GColorCobaltBlue, GColorWhite);
  layer_add_child(overlay_layer, status_bar_layer_get_layer(status_bar_layer));


  // ----------------- //
  // Setup alert layer
  // ----------------- //
  alert_layer = layer_create(GRect(PBL_IF_ROUND_ELSE(40, 34), 200, 100, 100));
  alert_text_layer = text_layer_create(GRect(0, 0, 100, 100));
  text_layer_set_text(alert_text_layer, alert_text_buffer);
  text_layer_set_text_alignment(alert_text_layer, GTextAlignmentCenter);
  layer_add_child(alert_layer, text_layer_get_layer(alert_text_layer));
  layer_add_child(window_get_root_layer(splash_window), alert_layer);

  // ----------------- //
  // Setup alert background layer
  // ----------------- //
  alert_background_bitmap_layer = bitmap_layer_create(GRect(0, 0, 100, 100));
  bitmap_layer_set_compositing_mode(alert_background_bitmap_layer, GCompOpSet);
  alert_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ALERT_BACKGROUND);
  bitmap_layer_set_bitmap(alert_background_bitmap_layer, alert_background_bitmap);
  layer_add_child(alert_layer, bitmap_layer_get_layer(alert_background_bitmap_layer));
}


void deinit() {
  window_destroy(splash_window);
  
  
  window_destroy(list_window);
  //window_destroy(compass_window);
  menu_layer_destroy(menu_layer);
  layer_destroy(overlay_layer);
  status_bar_layer_destroy(status_bar_layer);
  fonts_unload_custom_font(custom_font);
}


int main() {
  init();
  app_event_loop();
  deinit();
}