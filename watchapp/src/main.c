#include <pebble.h>
  
#define KEY_EVENTSTART 0
#define KEY_EVENTEND 1
#define KEY_EVENTLOCATION 2
#define KEY_EVENTTIMETO 3
#define KEY_EVENTTITLE 4

// Declare window
static Window *s_main_window;

// Declare text layers
static TextLayer *s_time_layer;
static TextLayer *s_name_layer;
static TextLayer *s_timerange_layer;
static TextLayer *s_timeuntil_layer;
static TextLayer *s_location_layer;

// Declare fonts
static GFont consolas_regular_font_14;
static GFont consolas_bold_font_40;
static GFont consolas_bold_font_18;
static GFont consolas_bold_font_14;

// Declare background
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
    
  // Create a long-lived buffer
  static char buffer[] = "00:00";
  
  // Write the current hours and minutes into the buffer
  if (clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);

}

static void main_window_load(Window *window) {
  // Create GFont
  consolas_regular_font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONSOLAS_REGULAR_14));
  consolas_bold_font_40 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONSOLAS_BOLD_40));
  consolas_bold_font_18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONSOLAS_BOLD_18));
  consolas_bold_font_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONSOLAS_BOLD_14));
  
  
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_NEXTEVENT_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Time textlayer
  s_time_layer = text_layer_create(GRect(0, 11, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, consolas_bold_font_40);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Name textlayer
  s_name_layer = text_layer_create(GRect(0, 97, 144, 50));
  text_layer_set_background_color(s_name_layer, GColorClear);
  text_layer_set_text_color(s_name_layer, GColorBlack);
  text_layer_set_font(s_name_layer, consolas_bold_font_18);
  text_layer_set_text_alignment(s_name_layer, GTextAlignmentCenter);
  text_layer_set_text(s_name_layer, "Loading...");

  // Timerange textlayer
  s_timerange_layer = text_layer_create(GRect(8, 82, 144, 50));
  text_layer_set_background_color(s_timerange_layer, GColorClear);
  text_layer_set_text_color(s_timerange_layer, GColorBlack);
  text_layer_set_font(s_timerange_layer, consolas_regular_font_14);
  text_layer_set_text_alignment(s_timerange_layer, GTextAlignmentLeft);
  //text_layer_set_text(s_timerange_layer, "9-10:30");
  
  // Timeuntil textlayer
  s_timeuntil_layer = text_layer_create(GRect(-4, 119, 144, 50));
  text_layer_set_background_color(s_timeuntil_layer, GColorClear);
  text_layer_set_text_color(s_timeuntil_layer, GColorBlack);
  text_layer_set_font(s_timeuntil_layer, consolas_regular_font_14);
  text_layer_set_text_alignment(s_timeuntil_layer, GTextAlignmentRight);
  //text_layer_set_text(s_timeuntil_layer, "in 1 week");

  // Location textlayer
  s_location_layer = text_layer_create(GRect(0, 144, 144, 50));
  text_layer_set_background_color(s_location_layer, GColorClear);
  text_layer_set_text_color(s_location_layer, GColorWhite);
  text_layer_set_font(s_location_layer, consolas_bold_font_14);
  text_layer_set_text_alignment(s_location_layer, GTextAlignmentCenter);
  //text_layer_set_text(s_location_layer, "Building 1");  
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_name_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_timerange_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_timeuntil_layer)); 
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_location_layer));
  
}

static void main_window_unload(Window *window) {
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_name_layer);
  text_layer_destroy(s_timerange_layer);
  text_layer_destroy(s_timeuntil_layer);
  text_layer_destroy(s_location_layer);
  
  // Unload GFonts
  fonts_unload_custom_font(consolas_regular_font_14);
  fonts_unload_custom_font(consolas_bold_font_40);
  fonts_unload_custom_font(consolas_bold_font_18);
  fonts_unload_custom_font(consolas_bold_font_14);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get NextEvent update every 5 minutes
  if(tick_time->tm_min % 5 == 0) {
    
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
    
    // Send the message!
    app_message_outbox_send();
  }  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
  static char eventstart_buffer[32];
  static char eventend_buffer[32];
  static char eventlocation_buffer[32];
  static char eventtimeto_buffer[32];
  static char eventtitle_buffer[32];
  static char assembledrange[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_EVENTSTART:
      snprintf(eventstart_buffer, sizeof(eventstart_buffer), "%s", t->value->cstring);
      break;
    case KEY_EVENTEND:
      snprintf(eventend_buffer, sizeof(eventend_buffer), "%s", t->value->cstring);
      break;
    case KEY_EVENTLOCATION:
      snprintf(eventlocation_buffer, sizeof(eventlocation_buffer), "%s", t->value->cstring);
      break;
    case KEY_EVENTTIMETO:
      snprintf(eventtimeto_buffer, sizeof(eventtimeto_buffer), "%s", t->value->cstring);
      break;
    case KEY_EVENTTITLE:
      snprintf(eventtitle_buffer, sizeof(eventtitle_buffer), "%s", t->value->cstring);
      break;      
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  text_layer_set_text(s_name_layer, eventtitle_buffer);
  text_layer_set_text(s_location_layer, eventlocation_buffer);
  text_layer_set_text(s_timeuntil_layer, eventtimeto_buffer);
  snprintf(assembledrange, sizeof(assembledrange), "%s-%s", eventstart_buffer, eventend_buffer);
  text_layer_set_text(s_timerange_layer, assembledrange);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());  

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Make sure the time is displayed from the start
  update_time();
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

