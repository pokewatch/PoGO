#include <pebble.h>
#include "pokedex.h"

// TODO: replace crude hack with https://github.com/smallstoneapps/data-processor ;)
#define KEY_POKEMON1ID 0
#define KEY_POKEMON1EXPIRATIONTIME 1
#define KEY_POKEMON1DISTANCE 2
// . . .
//#define KEY_POKEMON9ID 24
//#define KEY_POKEMON9EXPIRATIONTIME 5
//#define KEY_POKEMON9DISTANCE 26

Window *list, *compass;
MenuLayer *menu;
Layer *overlay;

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

static GPath *mini_compass = NULL;
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

void draw_pokemon(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data){
	if(index->row == 0){
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
		return;
	}
	
	if(menu_cell_layer_is_highlighted(cell_layer)){
		graphics_context_set_fill_color(ctx, GColorLightGray);
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
	gpath_draw_filled(ctx, mini_compass);
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

void config(void *context){
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 200, down);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, 200, up);
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

  time_t now = time(NULL);
  time_t expiration = now;
  long int expiration_delta;
  int distance = 0;

  // TODO: use https://github.com/smallstoneapps/data-processor instead of this quick crude hack!
  for(int i = 0; i < 9; i++){

  	// Read tuples
  	Tuple *pokemon_id_tuple = dict_find(iterator, KEY_POKEMON1ID + (i * 3));
  	if(pokemon_id_tuple) {
  	  nearby[i].dex = pokemon_id_tuple->value->int32;
  	}

  	// break on first 0 ID - indicates end of data
  	if (nearby[i].dex == 0) {
  		break;
  	}

  	Tuple *pokemon_expiration_tuple = dict_find(iterator, KEY_POKEMON1EXPIRATIONTIME + (i * 3));
  	if(pokemon_expiration_tuple) {
      expiration = pokemon_expiration_tuple->value->int32;
  	}

  	expiration_delta = expiration - now;

  	Tuple *pokemon_distance_tuple = dict_find(iterator, KEY_POKEMON1DISTANCE + (i * 3));
  	if(pokemon_distance_tuple) {
  	  distance = pokemon_distance_tuple->value->int32;
  	}

  	// TODO: add check for overall validity first (inc. e.g. already expired and not worth showing)

  	// TODO: refactor ASAP!
  	NUM_POKEMON = i + 1;

  	// add single Pokemon
  	nearby[i].sprite = gbitmap_create_with_resource(poke_images[nearby[i].dex]);
  	//strncpy(nearby[0].listBuffer, poke_names[nearby[0].dex], sizeof(nearby[0].listBuffer));
  	snprintf(nearby[i].listBuffer, sizeof(nearby[i].listBuffer), "%s\n(%d:%02d)\n%dm", poke_names[nearby[i].dex], 
  	  (int) expiration_delta / 60, (int) expiration_delta % 60, distance); 

  }
  
  menu_layer_reload_data(menu);

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


void init(){

	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);

	// Open AppMessage
	// TODO: sizes?
	app_message_open(2048,2048);


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
	
	mini_compass = gpath_create(&MINI_COMPASS_INFO);
	gpath_move_to(mini_compass, GPoint(124, 40));
	gpath_rotate_to(mini_compass, TRIG_MAX_ANGLE/12);
	
	overlay = layer_create(layer_get_bounds(window_get_root_layer(list)));
	layer_set_update_proc(overlay, temp_draw);
	
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
	NUM_POKEMON = 1;

	nearby[0].sprite = gbitmap_create_with_resource(poke_images[0]);
	strncpy(nearby[0].listBuffer, "Loading...", sizeof(nearby[0].listBuffer));

	menu_layer_reload_data(menu);
	menu_layer_set_selected_index(menu, (MenuIndex){0,1}, MenuRowAlignNone, false);
	
	window_stack_push(list, true);	
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