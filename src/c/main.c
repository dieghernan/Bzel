#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include "weathericon.h"
//Static and initial vars
static GFont FontHour, FontMinute, FontDate, FontTemp, FontCond, FontCiti, FontSymbol;
char tempstring[44], condstring[44], citistring[44];
static Window * s_window;
static Layer * s_canvas;
static int s_hours, s_minutes, s_weekday, s_day,  s_loop, s_countdown;
//////Init Configuration///
//Init Clay
ClaySettings settings;
// Initialize the default settings
static void prv_default_settings(){
  settings.BackgroundColor = GColorDarkGray;
  settings.ForegroundColor = GColorWhite;
  settings.HourColor=GColorWhite;
  settings.MinColor=GColorDarkGray;
  settings.BackgroundColorNight = GColorWhite;
  settings.ForegroundColorNight = GColorDarkGray;
  settings.HourColorNight=GColorDarkGray;
  settings.MinColorNight=GColorWhite;
  settings.WeatherUnit = false;
  settings.WeatherCond = 0;
  settings.UpSlider = 30;
  settings.HourSunrise = 600;
  settings.HourSunset = 1700;
  settings.DisplayLoc = false;
  settings.DisplayDate = false;
  settings.DisplayLoc = false;
  settings.BTOn = true;
  settings.GPSOn = false;
  settings.NightTheme = true;
  settings.IsNightNow = false;
  settings.ClockMode=1;
}
//////End Configuration///
///////////////////////////
static int hourtodraw(bool hourformat, int hournow){
  if (hourformat){
    return hournow;
  } 
  else{
    if (hournow == 0){
      return 12;
    } else if (hournow <= 12){
      return hournow;
    } else{
      return hournow - 12;
    }
  }
};
static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && settings.IsNightNow ){
    return ColorNight;
  } 
  else{
    return ColorDay;
  }
}
// Callback for js request
void request_watchjs(){
  //Starting loop at 0
  s_loop = 0;
  // Begin dictionary
  DictionaryIterator * iter;
  app_message_outbox_begin( & iter);
  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);
  // Send the message!
  app_message_outbox_send();
}
///BT Connection
static void bluetooth_callback(bool connected){
  settings.BTOn = connected;
}
static void onreconnection(bool before, bool now){
  if (!before && now){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BT reconnected, requesting weather at %d", s_minutes);
    request_watchjs();
  }
}
//Update main layer
static void layer_update_proc(Layer * layer, GContext * ctx){
//BT Handlers
  //If it was disconnected fetch new values
  onreconnection(settings.BTOn, connection_service_peek_pebble_app_connection());
  // Update connection toggle
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  // Prepare canvas
  //Create Background
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  graphics_fill_rect(ctx, bounds, 0, GCornersAll);
  graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
  graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 32, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  GRect inner = grect_inset(bounds, GEdgeInsets(15));
  // Hour
  graphics_context_set_text_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
  GRect hour_rect=grect_centered_from_polar(GRect(bounds.size.w/2, bounds.size.h/2, 0, 0), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), GSize(50, 42));
  char hournow[4];
  int hourtorect = hourtodraw(clock_is_24h_style(), s_hours);
  snprintf(hournow, sizeof(hournow), "%02d", hourtorect);
  graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  // am or pm
  if (!clock_is_24h_style()){
    char ampm[2];
    if (s_hours < 12){
      strcpy(ampm, "am");
    } 
    else{
      strcpy(ampm, "pm");
    }
    GRect ampmrect = GRect(hour_rect.origin.x + hour_rect.size.w + 1,
                           hour_rect.origin.y +hour_rect.size.h - 20,
                           30,
                           8);
    graphics_draw_text(ctx, ampm, FontDate, ampmrect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    
  }  
  // Minute
  if (settings.ClockMode==1){
    graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    char minnow[4];  
    snprintf(minnow, sizeof(minnow), "%02d",s_minutes);
    graphics_draw_text(ctx, minnow, FontMinute, 
                       grect_centered_from_polar(inner,GOvalScaleModeFitCircle,DEG_TO_TRIGANGLE(360*s_minutes/60),GSize(32, 22)),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  else if (settings.ClockMode==2){
    //Dot
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    GPoint dotpos=gpoint_from_polar(grect_inset(inner, GEdgeInsets(2)), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(360*s_minutes/60));
    graphics_fill_circle(ctx, dotpos, 7);    
  }  
  graphics_context_set_text_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
  // Date
  if (settings.DisplayDate){
    char datenow[44];
    const char * sys_locale = i18n_get_system_locale();
    fetchwday(s_weekday, sys_locale, datenow);
    char convertday[4];
    snprintf(convertday, sizeof(convertday), " %02d", s_day);
    strcat(datenow, convertday);
    graphics_context_set_fill_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
    GRect date_rect_right=GRect(hour_rect.origin.x+hour_rect.size.w+1, hour_rect.origin.y+8, 30, hour_rect.size.h);
    GRect date_rect_left=GRect(hour_rect.origin.x-date_rect_right.size.w-1, date_rect_right.origin.y, 
                               date_rect_right.size.w, date_rect_right.size.h);
    if (clock_is_24h_style()){
      graphics_draw_text(ctx, datenow, FontDate, date_rect_right, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    }
    else {
      graphics_draw_text(ctx, datenow, FontDate, date_rect_left, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    }
  }
  // Complications
  GRect loc_rect=GRect(hour_rect.origin.x-17, hour_rect.origin.y-20, hour_rect.size.w+34, 25);
  GRect temprect = GRect(hour_rect.origin.x - 10,hour_rect.origin.y + hour_rect.size.h + 1,
                         hour_rect.size.w / 2 + 9,(inner.size.h / 2 - hour_rect.size.h / 2) / 2);
  GRect condrect = GRect(hour_rect.origin.x + hour_rect.size.w / 2 + 1,temprect.origin.y,
                         hour_rect.size.w / 2 - 1,(inner.size.h / 2 - hour_rect.size.h / 2) / 2);
  
  if (settings.DisplayLoc || settings.DisplayTemp){
    if (!settings.BTOn){
      graphics_draw_text(ctx, "a", FontSymbol, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    else if (!settings.GPSOn){
      graphics_draw_text(ctx, "b", FontSymbol, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    else{
      if (settings.DisplayLoc){
        graphics_draw_text(ctx, citistring, FontCiti, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);        
      }
      if (settings.DisplayTemp){
        graphics_draw_text(ctx, tempstring, FontTemp, temprect, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
        graphics_draw_text(ctx, condstring, FontCond, condrect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      }
    }
  }  
}
/////////////////////////////////////////
////Init: Handle Settings and Weather////
/////////////////////////////////////////
// Read settings from persistent storage
static void prv_load_settings(){
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Save the settings to persistent storage
static void prv_save_settings(){
  persist_write_data(SETTINGS_KEY, & settings, sizeof(settings));
}
// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator * iter, void * context){
  s_loop = s_loop + 1;
  if (s_loop == 1){
    //Clean vars
    strcpy(tempstring, "");
    strcpy(condstring, "");
    strcpy(citistring, "");
  }
  // Background Color
  Tuple * bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t){
    settings.BackgroundColor = GColorFromHEX(bg_color_t-> value -> int32);
  }
  Tuple * nbg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColorNight);
  if (nbg_color_t){
    settings.BackgroundColorNight = GColorFromHEX(nbg_color_t -> value -> int32);
  }
  // Foreground Color
  Tuple * fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  if (fg_color_t){
    settings.ForegroundColor = GColorFromHEX(fg_color_t -> value -> int32);
  }
  Tuple * nfg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColorNight);
  if (nfg_color_t){
    settings.ForegroundColorNight = GColorFromHEX(nfg_color_t -> value -> int32);
  }
  Tuple * hour_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hour_color_t){
    settings.HourColor = GColorFromHEX(hour_color_t -> value -> int32);
  }
  Tuple * nhour_color_t = dict_find(iter, MESSAGE_KEY_HourColorNight);
  if (nhour_color_t){
    settings.HourColorNight = GColorFromHEX(nhour_color_t -> value -> int32);
  }
  Tuple * min_color_t = dict_find(iter, MESSAGE_KEY_MinColor);
  if (min_color_t){
    settings.MinColor = GColorFromHEX(min_color_t -> value -> int32);
  }
  Tuple * nmin_color_t = dict_find(iter, MESSAGE_KEY_MinColorNight);
  if (nmin_color_t){
    settings.MinColorNight = GColorFromHEX(nmin_color_t -> value -> int32);
  }
  //Control of data from http
  // Weather Cond
  Tuple * wcond_t = dict_find(iter, MESSAGE_KEY_WeatherCond);
  if (wcond_t){
    get_conditions((int) wcond_t -> value -> int32, condstring, settings.IsNightNow);
  }
  // Weather Temp
  Tuple * wtemp_t = dict_find(iter, MESSAGE_KEY_WeatherTemp);
  if (wtemp_t){
    snprintf(tempstring, sizeof(tempstring), "%s", wtemp_t -> value -> cstring);
  }
  //Hour Sunrise and Sunset
  Tuple * sunrise_t = dict_find(iter, MESSAGE_KEY_HourSunrise);
  if (sunrise_t){
    settings.HourSunrise = (int) sunrise_t -> value -> int32;
  }
  Tuple * sunset_t = dict_find(iter, MESSAGE_KEY_HourSunset);
  if (sunset_t){
    settings.HourSunset = (int) sunset_t -> value -> int32;
  }
  // Location
  Tuple * neigh_t = dict_find(iter, MESSAGE_KEY_NameLocation);
  if (neigh_t){
    snprintf(citistring, sizeof(citistring), "%s", neigh_t -> value -> cstring);
  }
  //Control of data gathered for http
  APP_LOG(APP_LOG_LEVEL_DEBUG, "After loop %d temp is %s Cond is %s and City is %s Sunrise is %d Sunset is %d", s_loop, tempstring, condstring, citistring, settings.HourSunrise, settings.HourSunset);
  if (strcmp(tempstring, "") != 0 && strcmp(condstring, "") != 0 && strcmp(citistring, "")){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "GPS fully working at loop %d", s_loop);
    settings.GPSOn = true;
  } else{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing info at loop %d, GPS false", s_loop);
    settings.GPSOn = false;
  }
  //End data gathered
  // Get display handlers
  Tuple * frequpdate = dict_find(iter, MESSAGE_KEY_UpSlider);
  if (frequpdate){
    settings.UpSlider = (int) frequpdate -> value -> int32;
    //Restart the counter
    s_countdown = settings.UpSlider;
  }
  Tuple * disdate_t = dict_find(iter, MESSAGE_KEY_DisplayDate);
  if (disdate_t){
    if (disdate_t -> value -> int32 == 0){
      settings.DisplayDate = false;
    } else settings.DisplayDate = true;
  }
  Tuple * distemp_t = dict_find(iter, MESSAGE_KEY_DisplayTemp);
  if (distemp_t){
    if (distemp_t -> value -> int32 == 0){
      settings.DisplayTemp = false;
    } else settings.DisplayTemp = true;
  }
  Tuple * disloc_t = dict_find(iter, MESSAGE_KEY_DisplayLoc);
  if (disloc_t){
    if (disloc_t -> value -> int32 == 0){
      settings.DisplayLoc = false;
    } else settings.DisplayLoc = true;
  }
  Tuple * disntheme_t = dict_find(iter, MESSAGE_KEY_NightTheme);
  if (disntheme_t){
    if (disntheme_t -> value -> int32 == 0){
      settings.NightTheme = false;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "NTHeme off");
    } else settings.NightTheme = true;
  }
  Tuple * clockmode_t = dict_find(iter, MESSAGE_KEY_ClockMode);
  if (clockmode_t){
    settings.ClockMode=atoi(clockmode_t->value->cstring);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode %d",settings.ClockMode);
  }
  //Update colors
  layer_mark_dirty(s_canvas);
  window_set_background_color(s_window, ColorSelect( settings.BackgroundColor, settings.BackgroundColorNight));
  // Save the new settings to persistent storage
  prv_save_settings();
}
//Load main layer
static void window_load(Window * window){
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, layer_update_proc);
  layer_add_child(window_layer, s_canvas);
}
static void window_unload(Window * window){
  layer_destroy(s_canvas);
  window_destroy(s_window);
  fonts_unload_custom_font(FontTemp);
  fonts_unload_custom_font(FontHour);
  fonts_unload_custom_font(FontMinute);
  fonts_unload_custom_font(FontCiti);
  fonts_unload_custom_font(FontCond);
  fonts_unload_custom_font(FontSymbol);
  fonts_unload_custom_font(FontDate);
}
void main_window_push(){
  s_window = window_create();
  window_set_background_color(s_window, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}
void main_window_update(int hours, int minutes, int weekday, int day){
  s_hours = hours;
  s_minutes = minutes;
  s_day = day;
  s_weekday = weekday;
  layer_mark_dirty(s_canvas);
}
static void tick_handler(struct tm * time_now, TimeUnits changed){
  main_window_update(time_now -> tm_hour, time_now -> tm_min, time_now -> tm_wday, time_now -> tm_mday);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick at %d", time_now -> tm_min);
  s_loop = 0;
  if (s_countdown == 0){
    //Reset
    s_countdown = settings.UpSlider;
  } else{
    s_countdown = s_countdown - 1;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Countdown to update %d", s_countdown);
  // Evaluate if is day or night
  int nowthehouris = s_hours * 100 + s_minutes;
  if (settings.HourSunrise <= nowthehouris && nowthehouris <= settings.HourSunset){
    settings.IsNightNow = false;
  } else{
    settings.IsNightNow = true;
  }
  // Extra catch on sunrise and sunset
  if (nowthehouris == settings.HourSunrise || nowthehouris == settings.HourSunset){
    s_countdown = 1;
  }
  if (settings.GPSOn && settings.NightTheme){
    //Extra catch around 1159 to gather information of today
    if (nowthehouris == 1159 && s_countdown > 5){
      s_countdown = 1;
    };
    // Change Color of background
    layer_mark_dirty(s_canvas);
    window_set_background_color(s_window, ColorSelect( settings.BackgroundColor, settings.BackgroundColorNight));
  }
  // Get weather update every requested minutes and extra request 5 minutes earlier
  if (s_countdown == 0 || s_countdown == 5){
    if (settings.DisplayTemp || settings.DisplayLoc){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Update weather at %d", time_now -> tm_min);
      request_watchjs();
    }
  }
  //If GPS was off request weather every 15 minutes
  if (!settings.GPSOn){
    if (settings.DisplayTemp || settings.DisplayLoc){
      if (settings.UpSlider > 15){
        if (s_countdown % 15 == 0){
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Attempt to request GPS on %d", time_now -> tm_min);
          request_watchjs();
        }
      }
    }
  }
}
static void init(){
  prv_load_settings();
  // Listen for AppMessages
  //Starting loop at 0
  s_loop = 0;
  s_countdown = settings.UpSlider;
  //Clean vars
  strcpy(tempstring, "");
  strcpy(condstring, "");
  strcpy(citistring, "");
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hours=t->tm_hour;
  s_minutes=t->tm_min;
  s_day=t->tm_mday;
  s_weekday=t->tm_wday;
  //Register and open
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  // Load Fonts
  FontHour = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GBOLD_34));
  FontMinute = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GBOLD_18));
  FontDate = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLIGHT_12));
  FontTemp = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLIGHT_14));
  FontCond = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WICON_22));
  FontCiti = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GLIGHT_10));
  FontSymbol = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SYMBOL_16));
  main_window_push();
  // Register with Event Services
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_callback
  });
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks(); //Destroy the callbacks for clean up
}
int main(){
  init();
  app_event_loop();
  deinit();
}