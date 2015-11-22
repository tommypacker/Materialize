#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_status_bar;
static BitmapLayer *s_weather_icon;
static GBitmap *s_weather_bitmap;
static GFont s_font;
static GFont s_font_small;

static void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%I:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  update_time();
}

static void battery_handler(BatteryChargeState new_state){
  static char s_battery_buffer[16];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d Percent", new_state.charge_percent);
  text_layer_set_text(s_status_bar, s_battery_buffer);
}

static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect weather_bounds = GRect(0,PBL_IF_ROUND_ELSE(52,105), bounds.size.w, 50);

  s_status_bar = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(52,5), bounds.size.w, 50));
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,35), bounds.size.w, 50));

  //Load custom minimal font
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_REGULAR_DOS_48));
  s_font_small = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ROBOTO_REGULAR_DOS_12));

  s_weather_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_SUNNY);
  s_weather_icon = bitmap_layer_create(weather_bounds);

  window_set_background_color(s_main_window, GColorVividCerulean);

  //Set time text layer attributes
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  //Set status bar layer attributes
  text_layer_set_background_color(s_status_bar, GColorClear);
  text_layer_set_text_color(s_status_bar, GColorWhite);
  //text_layer_set_text(s_status_bar, "Battery status");
  text_layer_set_font(s_status_bar, s_font_small);
  text_layer_set_text_alignment(s_status_bar, GTextAlignmentCenter);

  bitmap_layer_set_alignment(s_weather_icon, GAlignCenter);
  bitmap_layer_set_background_color(s_weather_icon, GColorClear);
  bitmap_layer_set_compositing_mode(s_weather_icon, GCompOpSet);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_status_bar));
  bitmap_layer_set_bitmap(s_weather_icon, s_weather_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_weather_icon));

  battery_handler(battery_state_service_peek());
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_status_bar);
  gbitmap_destroy(s_weather_bitmap);
  bitmap_layer_destroy(s_weather_icon);
  fonts_unload_custom_font(s_font);
}

static void init(){
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();

  battery_state_service_subscribe(battery_handler);
}

static void deinit(){
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}