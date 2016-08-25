#include <pebble.h>
#include "pokedex.h"

// TODO: replace crude hack with https://github.com/smallstoneapps/data-processor ;)
#define KEY_POKEMON1ID 0
#define KEY_POKEMON1EXPIRATIONTIME 1
#define KEY_POKEMON1DISTANCE 2
#define KEY_POKEMON1BEARING 3
// . . .
//#define KEY_POKEMON9ID 32
//#define KEY_POKEMON9EXPIRATIONTIME 33
//#define KEY_POKEMON9DISTANCE 34
//#define KEY_POKEMON9BEARING 35
#define KEY_REQUESTTYPE 36
#define KEY_DISPLAYMESSAGE 37

Window *splash, *list, *compass;
MenuLayer *menu;
StatusBarLayer *status_bar;
Layer *overlay;

GBitmap *splash_bitmap;
BitmapLayer *bitmap_layer;
TextLayer *loading_layer;
Layer *alert;
TextLayer *alertText;

//alert layer
BitmapLayer *alertBackgroundLayer;
GBitmap *alertBackground;

//GBitmap *nearby[9];
GBitmap *top, *bottom;

//int distances[9];
//int angles[9];
//int pokedex[9];

int NUM_POKEMON = 0;

#define VALOR 0
#define MYSTIC 1
#define INSTINCT 2
int TEAM = 1;

bool loading = true;

GFont custom_font;

typedef struct {
	int angle;
	int dex;
	GBitmap *sprite;
	char name[64];
	char distance[8];
	char listBuffer[128];
	GPath *compass;
} Pokemon;

Pokemon nearby[9];

static const GPathInfo MINI_COMPASS_INFO = {
	.num_points = 4,
	.points = (GPoint []) { {0,-9}, {-6,9}, {0,3}, {6,9} }
};

void temp_draw(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorCobaltBlue);
	graphics_fill_rect(ctx, GRect(0,0,PBL_IF_RECT_ELSE(144,180),16), 0, GCornerNone);

	#ifdef PBL_RECT
	graphics_context_set_stroke_color(ctx, GColorWhite);
	for(int i = 0; i < 144; i++){
		if(i%2 == 0) graphics_draw_pixel(ctx, GPoint(i, 15));
	}
	#endif

	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, top, GRect(PBL_IF_RECT_ELSE(0,18),16,144,11));
	graphics_draw_bitmap_in_rect(ctx, bottom, GRect(PBL_IF_RECT_ELSE(0,18),PBL_IF_RECT_ELSE(168,180)-11,144,11));

	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, "12:34", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0,PBL_IF_RECT_ELSE(-2,2),PBL_IF_RECT_ELSE(144,180),16), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

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

void draw_pokemon(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data){
	if(index->row == 0){
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
		return;
	}

	if(menu_cell_layer_is_highlighted(cell_layer)){
		graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorLightGray,GColorWhite));
		graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
		#ifndef PBL_COLOR
		graphics_context_set_stroke_color(ctx, GColorBlack);
		graphics_context_set_stroke_width(ctx, 2);
		graphics_draw_line(ctx, GPoint(2,4), GPoint(2,56));
		#endif
	}

	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, nearby[index->row-1].sprite, GRect(2+30-(gbitmap_get_bounds(nearby[index->row-1].sprite).size.w/2),30-(gbitmap_get_bounds(nearby[index->row-1].sprite).size.h/2), gbitmap_get_bounds(nearby[index->row-1].sprite).size.w, gbitmap_get_bounds(nearby[index->row-1].sprite).size.h));

	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, nearby[index->row-1].listBuffer, custom_font, GRect(64, 14, 180, 30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, nearby[index->row-1].compass);
}

static uint16_t get_num_pokemon(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return NUM_POKEMON + 1;
}

static int16_t pokemon_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	if(cell_index->row == 0) return 20;
	return 60;
}

void down(ClickRecognizerRef ref, void *context){
	MenuIndex current = menu_layer_get_selected_index(menu);
	if(current.row == NUM_POKEMON) return;
	else {
		menu_layer_set_selected_next(menu, false, MenuRowAlignCenter, true);
	}
}

void up(ClickRecognizerRef ref, void *context){
	MenuIndex current = menu_layer_get_selected_index(menu);
	if(current.row == 1) return;
	else{
		menu_layer_set_selected_next(menu, true, MenuRowAlignCenter, true);
	}
}

void back(ClickRecognizerRef ref, void *context) {
    window_stack_pop_all(false);
}

void config(void *context){
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, down);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, up);
	window_single_click_subscribe(BUTTON_ID_BACK, back);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {

  // call API on the 15s (for now until distance refresh implemented)
  //if (tick_time->tm_sec % 15 == 0) {
  // TODO: restore temporary decrease in frequency when demo mode is no longer nec.
  if (tick_time->tm_sec % 15 == 0) {

  	APP_LOG(APP_LOG_LEVEL_INFO, "tick_handler() 0/15/30/45");

  	// Begin dictionary
  	DictionaryIterator *iter;
  	app_message_outbox_begin(&iter);

  	// RequestType is currently meaningless, but w/b used for e.g. "full" API call vs. distance refresh

  	// Add key-value pairs
  	dict_write_cstring(iter, KEY_REQUESTTYPE, "");

  	// Send the message!
  	app_message_outbox_send();

  }
}

void displayToast(char *string) {
		GRect from_frame =  GRect(PBL_IF_ROUND_ELSE(40, 22), 180, 100, 100);
		GRect to_frame = GRect(PBL_IF_ROUND_ELSE(40, 22), 34, 100, 100);

		static char buf[] = "thisisfillertextpleaseignorethisistheonlyfixifound00000000000";    /* <-- implicit NUL-terminator at the end here */
    snprintf(buf, sizeof(buf), "%s", string);

		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %s", buf);
		text_layer_set_text(alertText, buf);

		vibes_double_pulse();

		animate_layer(alert, &from_frame, &to_frame, 1000, 0);
		//animate_layer(alert, &to_frame, &from_frame, 1000, 3000);
		// TODO: remove temporary increase when demo mode is no longer nec.
		animate_layer(alert, &to_frame, &from_frame, 1000, 5000);
}

static void bluetooth_callback(bool connected) {
	if(connected) {
		displayToast("Bluetooth reconnected");
	} else {
		displayToast("Bluetooth disconnected");
	}
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

  time_t now = time(NULL);
  time_t expiration = now;
  long int expiration_delta;
  int distance = 0;
  int bearing = 0;


  Tuple *tuple;
  tuple = dict_find(iterator, KEY_DISPLAYMESSAGE);
  if(tuple) {
  	if(tuple->value->cstring != 0) {
			APP_LOG(APP_LOG_LEVEL_ERROR, "String is %s" , tuple->value->cstring);
			displayToast(tuple->value->cstring);
   	}
  } else {

	// TODO: use https://github.com/smallstoneapps/data-processor instead of this quick crude hack!
	for(int i = 0; i < 9; i++){

	  // Read tuples
	  Tuple *pokemon_id_tuple = dict_find(iterator, KEY_POKEMON1ID + (i * 4));
	  if(pokemon_id_tuple) {
	  	nearby[i].dex = pokemon_id_tuple->value->int32;
	  }

	  // break on first 0 ID - indicates end of data
	  if (nearby[i].dex == 0) {
	  	break;
	  }

	  Tuple *pokemon_expiration_tuple = dict_find(iterator, KEY_POKEMON1EXPIRATIONTIME + (i * 4));
	  if(pokemon_expiration_tuple) {
	    expiration = pokemon_expiration_tuple->value->int32;
	  }

	  expiration_delta = expiration - now;

	  Tuple *pokemon_distance_tuple = dict_find(iterator, KEY_POKEMON1DISTANCE + (i * 4));
	  if(pokemon_distance_tuple) {
	  	distance = pokemon_distance_tuple->value->int32;
	  }

	  Tuple *pokemon_bearing_tuple = dict_find(iterator, KEY_POKEMON1BEARING + (i * 4));
	  if(pokemon_bearing_tuple) {
	  	bearing = pokemon_bearing_tuple->value->int32;
	  }
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "bearing: %d", bearing);

	  // TODO: add check for overall validity first (inc. e.g. already expired and not worth showing)

	  // TODO: refactor ASAP!
	  NUM_POKEMON = i + 1;

	  // need to destroy old bitmap first
	  if(nearby[i].sprite != NULL) {
	  	gbitmap_destroy(nearby[i].sprite);
	  }

	  // TODO: recycle instead, since it will realistically be just a few pokemon based on data so far
	  // TODO: and do a better job of clean-up in general...?

	  nearby[i].sprite = gbitmap_create_with_resource(poke_images[nearby[i].dex]);
	  //strncpy(nearby[0].listBuffer, poke_names[nearby[0].dex], sizeof(nearby[0].listBuffer));

	  // TODO: better
	  if (expiration_delta < 0) {
  		// expired pokemon should really be destroyed (or never present in data anyway!), but for now...
	  	snprintf(nearby[i].listBuffer, sizeof(nearby[i].listBuffer), "%s\n-:--\n- m", poke_names[nearby[i].dex]);
	  } else if (distance > 99999) {
  		// format "k" if 100km+ (saving two characters vs. " km")
	  	snprintf(nearby[i].listBuffer, sizeof(nearby[i].listBuffer), "%s\n%d:%02d\n%dk", poke_names[nearby[i].dex],
	  	  (int) expiration_delta / 60, (int) expiration_delta % 60, distance / 1000);
	  } else if (distance > 999) {
  		// format " km" if 1000m+
	  	snprintf(nearby[i].listBuffer, sizeof(nearby[i].listBuffer), "%s\n%d:%02d\n%d km", poke_names[nearby[i].dex],
	  	  (int) expiration_delta / 60, (int) expiration_delta % 60, distance / 1000);
	  } else {
	  	snprintf(nearby[i].listBuffer, sizeof(nearby[i].listBuffer), "%s\n%d:%02d\n%d m", poke_names[nearby[i].dex],
	  	  (int) expiration_delta / 60, (int) expiration_delta % 60, distance);
	  }

	  nearby[i].angle = bearing * TRIG_MAX_ANGLE / 360;
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "nearby[i].angle: %d", nearby[i].angle);

	  // draw compass
	  nearby[i].compass = gpath_create(&MINI_COMPASS_INFO);
	  gpath_move_to(nearby[i].compass, GPoint(124, 40));
	  //nearby[i].angle = TRIG_MAX_ANGLE/(12*i);
	  gpath_rotate_to(nearby[i].compass, nearby[i].angle);

	}

	menu_layer_reload_data(menu);

	if(loading){
	  if(NUM_POKEMON >= 1) menu_layer_set_selected_index(menu, (MenuIndex){0,1}, MenuRowAlignNone, false);
	  window_stack_push(list, true);
	  vibes_short_pulse();
		layer_add_child(window_get_root_layer(list), alert);
	  loading = false;
	}
  }
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


void compass_handler(CompassHeadingData heading){
	for(int i = 0; i < NUM_POKEMON; i++){
		gpath_rotate_to(nearby[i].compass, heading.true_heading + nearby[i].angle);
	}
	layer_mark_dirty(window_get_root_layer(list));
}


void init(){

	splash = window_create();
	window_set_background_color(splash, GColorBlack);
	splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_POGO);
	GRect window_bounds = layer_get_bounds(window_get_root_layer(splash));
	GRect bitmap_bounds = gbitmap_get_bounds(splash_bitmap);
	bitmap_layer = bitmap_layer_create(GRect( (window_bounds.size.w - bitmap_bounds.size.w)/2, (window_bounds.size.h - bitmap_bounds.size.h)/2, bitmap_bounds.size.w, bitmap_bounds.size.h) );
	bitmap_layer_set_bitmap(bitmap_layer, splash_bitmap);
	layer_add_child(window_get_root_layer(splash), bitmap_layer_get_layer(bitmap_layer));
	loading_layer = text_layer_create(GRect(0,window_bounds.size.w/2 + bitmap_bounds.size.w/2, PBL_IF_RECT_ELSE(144,180), 28));
	text_layer_set_text(loading_layer, "Loading...");
	text_layer_set_background_color(loading_layer, GColorBlack);
	text_layer_set_text_color(loading_layer, GColorWhite);
	text_layer_set_text_alignment(loading_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(splash), text_layer_get_layer(loading_layer));
	window_stack_push(splash, true);

	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	// TODO: sizes?
	app_message_open(2048,2048);

  	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

	connection_service_subscribe((ConnectionHandlers) {
  	.pebble_app_connection_handler = bluetooth_callback
	});


	custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PKMN_10));

	top = gbitmap_create_with_resource(RESOURCE_ID_UI_TOP);
	bottom = gbitmap_create_with_resource(RESOURCE_ID_UI_BOTTOM);

	GColor *pal_top = gbitmap_get_palette(top);
	GColor *pal_bottom = gbitmap_get_palette(bottom);

	#ifdef PBL_COLOR
	switch(TEAM){
		case MYSTIC:
			pal_top[0] = GColorCobaltBlue;
			pal_bottom[2] = GColorCobaltBlue;
		break;

		case INSTINCT:
			pal_top[0] = GColorOrange;
			pal_bottom[2] = GColorOrange;
		break;
	}
	#endif

	list = window_create();
	window_set_click_config_provider(list, config);
	window_set_background_color(list, GColorWhite);

	menu = menu_layer_create(GRect(0, 16, PBL_IF_RECT_ELSE(144,180), PBL_IF_RECT_ELSE(168,180) - 16));
	menu_layer_set_callbacks(menu, NULL, (MenuLayerCallbacks){
		.draw_row = draw_pokemon,
		.get_num_rows = get_num_pokemon,
		.get_cell_height = pokemon_cell_height,
	});
	//menu_layer_set_click_config_onto_window(menu, list);

	layer_add_child(window_get_root_layer(list), menu_layer_get_layer(menu));

	overlay = layer_create(layer_get_bounds(window_get_root_layer(list)));
	layer_set_update_proc(overlay, temp_draw);

	status_bar = status_bar_layer_create();
	int16_t width = layer_get_bounds(overlay).size.w;
	GRect frame = GRect(0, -1, width, STATUS_BAR_LAYER_HEIGHT); // -1 so the overlay still shows up beneath it
	layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
	status_bar_layer_set_colors(status_bar, GColorCobaltBlue, GColorWhite);
	layer_add_child(overlay, status_bar_layer_get_layer(status_bar));


	layer_add_child(window_get_root_layer(list), overlay);

/*
	//DUMMY DATA
	NUM_POKEMON = 5;

	nearby[0].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke25);
	strncpy(nearby[0].listBuffer, "Pikachu\n\n12 m", sizeof(nearby[0].listBuffer));

	nearby[1].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke83);
	strncpy(nearby[1].listBuffer, "Farfetchd\n\n53 m", sizeof(nearby[1].listBuffer));

	nearby[2].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke128);
	strncpy(nearby[2].listBuffer, "Tauros\n\n121 m", sizeof(nearby[2].listBuffer));

	nearby[3].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke35);
	strncpy(nearby[3].listBuffer, "Clefairy\n\n135 m", sizeof(nearby[3].listBuffer));

	nearby[4].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke138);
	strncpy(nearby[4].listBuffer, "Omanyte\n\n163 m", sizeof(nearby[4].listBuffer));
*/
	// crude loading message
	/*
	NUM_POKEMON = 1;

	nearby[0].sprite = gbitmap_create_with_resource(poke_images[0]);
	strncpy(nearby[0].listBuffer, "Loading...", sizeof(nearby[0].listBuffer));
	*/
	compass_service_subscribe(compass_handler);

	//menu_layer_reload_data(menu);
	//menu_layer_set_selected_index(menu, (MenuIndex){0,1}, MenuRowAlignNone, false);

	//window_stack_push(list, true);

	alert = layer_create(GRect(PBL_IF_ROUND_ELSE(40, 34), 200, 100, 100));

	alertText = text_layer_create(GRect(0, 0, 100, 100));
  	text_layer_set_text(alertText, "");
  	text_layer_set_text_alignment(alertText, GTextAlignmentCenter);
  	layer_add_child(alert, text_layer_get_layer(alertText));

	alertBackgroundLayer = bitmap_layer_create(GRect(0, 0, 100, 100));
	bitmap_layer_set_compositing_mode(alertBackgroundLayer, GCompOpSet);
    alertBackground = gbitmap_create_with_resource(RESOURCE_ID_ALERT_BACKGROUND);
    bitmap_layer_set_bitmap(alertBackgroundLayer, alertBackground);
    layer_add_child(alert, bitmap_layer_get_layer(alertBackgroundLayer));

	layer_add_child(window_get_root_layer(splash), alert);
}

void deinit(){
	window_destroy(list);
	window_destroy(compass);
	menu_layer_destroy(menu);
	layer_destroy(overlay);
	fonts_unload_custom_font(custom_font);
}

int main(){
	init();
	app_event_loop();
	deinit();
}
