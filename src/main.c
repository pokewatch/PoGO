#include <pebble.h>

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
	
	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, "12:34", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0,PBL_IF_RECT_ELSE(-2,6),PBL_IF_RECT_ELSE(144,180),16), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	
	graphics_context_set_stroke_color(ctx, GColorWhite);
	for(int i = 0; i < PBL_IF_RECT_ELSE(144,180); i++){
		if(i%2 == 0) graphics_draw_pixel(ctx, GPoint(i, 15));
	}
	
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, top, GRect(PBL_IF_RECT_ELSE(0,18),16,144,11));
	graphics_draw_bitmap_in_rect(ctx, bottom, GRect(PBL_IF_RECT_ELSE(0,18),168-11,144,11));
}

void draw_pokemon(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data){
	if(menu_cell_layer_is_highlighted(cell_layer)){
		graphics_context_set_fill_color(ctx, GColorPastelYellow);
		graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
		#ifndef PBL_COLOR
		graphics_context_set_stroke_color(ctx, GColorBlack);
		graphics_context_set_stroke_width(ctx, 2);
		graphics_draw_line(ctx, GPoint(2,2), GPoint(2,54));
		#endif
	}
	
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, nearby[index->row].sprite, GRect(4+28-(gbitmap_get_bounds(nearby[index->row].sprite).size.w/2),28-(gbitmap_get_bounds(nearby[index->row].sprite).size.h/2), gbitmap_get_bounds(nearby[index->row].sprite).size.w, gbitmap_get_bounds(nearby[index->row].sprite).size.h));
	
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, nearby[index->row].listBuffer, custom_font, GRect(64, 12, 144-64-4, 30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, mini_compass);
}

static uint16_t get_num_pokemon(MenuLayer *menu_layer, uint16_t section_index, void *data){
	return NUM_POKEMON;
}

static int16_t pokemon_cell_height(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	return 56;
}

static void pokemon_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data){
	//set_pokemon(pokedex[cell_index->row]);
	//window_stack_push(compass);
	//start_high_frequency_timer();
}

void init(){
	app_message_open(2048,2048);
	//app_message_register_inbox_received(inbox);
	
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
	window_set_background_color(list, GColorWhite);
	
	menu = menu_layer_create(GRect(0, 16, PBL_IF_RECT_ELSE(144,180), PBL_IF_RECT_ELSE(168,180) - 16));
	menu_layer_set_callbacks(menu, NULL, (MenuLayerCallbacks){
		.draw_row = draw_pokemon,
		.get_num_rows = get_num_pokemon,
		.get_cell_height = pokemon_cell_height,
		.select_click = pokemon_click
	});
	menu_layer_set_click_config_onto_window(menu, list);
	
	layer_add_child(window_get_root_layer(list), menu_layer_get_layer(menu));
	
	mini_compass = gpath_create(&MINI_COMPASS_INFO);
	gpath_move_to(mini_compass, GPoint(124, 38));
	gpath_rotate_to(mini_compass, TRIG_MAX_ANGLE/12);
	
	overlay = layer_create(layer_get_bounds(window_get_root_layer(list)));
	layer_set_update_proc(overlay, temp_draw);
	
	layer_add_child(window_get_root_layer(list), overlay);
	
	//DUMMY DATA
	NUM_POKEMON = 3;

	nearby[0].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke25);
	strncpy(nearby[0].listBuffer, "Pikachu\n\n12 m", sizeof(nearby[0].listBuffer));

	nearby[1].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke83);
	strncpy(nearby[1].listBuffer, "Farfetchd\n\n53 m", sizeof(nearby[1].listBuffer));
	
	nearby[2].sprite = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke128);
	strncpy(nearby[2].listBuffer, "Tauros\n\n121 m", sizeof(nearby[2].listBuffer));
				
	menu_layer_reload_data(menu);
	
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