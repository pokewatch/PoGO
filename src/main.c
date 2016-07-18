#include <pebble.h>

Window *list, *compass;
MenuLayer *menu;
Layer *overlay;

GBitmap *nearby[9];
GBitmap *top, *bottom;

int distances[9];
int angles[9];
int pokedex[9];

int NUM_POKEMON = 0;

#define VALOR 0
#define MYSTIC 1
#define INSTINCT 2
int TEAM = 1;

GFont custom_font;

void temp_draw(Layer *layer, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorCobaltBlue);
	graphics_fill_rect(ctx, GRect(0,0,144,16), 0, GCornerNone);
	
	graphics_context_set_text_color(ctx, GColorWhite);
	graphics_draw_text(ctx, "12:34", fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0,-2,144,16), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	
	graphics_context_set_stroke_color(ctx, GColorWhite);
	for(int i = 0; i < 144; i++){
		if(i%2 == 0) graphics_draw_pixel(ctx, GPoint(i, 15));
	}
	
	graphics_draw_bitmap_in_rect(ctx, top, GRect(0,16,144,11));
	graphics_draw_bitmap_in_rect(ctx, bottom, GRect(0,168-11,144,11));
}

void draw_pokemon(GContext *ctx, const Layer *cell_layer, MenuIndex *index, void *data){
	if(menu_cell_layer_is_highlighted(cell_layer)){
		graphics_context_set_fill_color(ctx, GColorPastelYellow);
		graphics_fill_rect(ctx, layer_get_bounds(cell_layer), 0, GCornerNone);
	}
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	graphics_draw_bitmap_in_rect(ctx, nearby[index->row], GRect(4+28-(gbitmap_get_bounds(nearby[index->row]).size.w/2),28-(gbitmap_get_bounds(nearby[index->row]).size.h/2), gbitmap_get_bounds(nearby[index->row]).size.w, gbitmap_get_bounds(nearby[index->row]).size.h));
	
	graphics_context_set_text_color(ctx, GColorBlack);
	graphics_draw_text(ctx, "Pokémon\n\n12 m", custom_font, GRect(64, 12, 144-64-4, 30), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
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
	
	list = window_create();
	window_set_background_color(list, GColorWhite);
	layer_set_update_proc(window_get_root_layer(list), temp_draw);
	
	menu = menu_layer_create(GRect(0,27,144,168-27-11));
	menu_layer_set_callbacks(menu, NULL, (MenuLayerCallbacks){
		.draw_row = draw_pokemon,
		.get_num_rows = get_num_pokemon,
		.get_cell_height = pokemon_cell_height,
		.select_click = pokemon_click
	});
	menu_layer_set_click_config_onto_window(menu, list);
	menu_layer_pad_bottom_enable(menu, false);
	
	layer_add_child(window_get_root_layer(list), menu_layer_get_layer(menu));
	
	overlay = layer_create(layer_get_bounds(window_get_root_layer(list)));
	layer_set_update_proc(overlay, temp_draw);
	
	layer_add_child(window_get_root_layer(list), overlay);
	
	//DUMMY DATA
	NUM_POKEMON = 3;
	nearby[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke21);
	nearby[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke75);
	nearby[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_poke103);

	distances[0] = 35;
	distances[1] = 49;
	distances[2] = 103;
	
	angles[0] = 27;
	angles[1] = 80;
	angles[2] = 125;
	
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