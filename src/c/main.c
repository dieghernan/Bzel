#include <pebble.h>
#include "main.h"
#include "weekday.h"
#include "weathericon.h"
//Static and initial vars
static GFont FontHour, FontMinute, FontDate, FontTemp, FontCond, FontCiti, FontSymbol;
char tempstring[44], condstring[44], citistring[44];
static Window * s_window;
static Layer * s_canvas;
static int s_hours, s_minutes, s_weekday, s_day, s_loop, s_countdown, s_steps;
static GRect bounds, inner,hour_rect,ampmrect, loc_rect, loc_rect_alt, iconstep, iconstep_alt,numstep,numstep_alt;
static GRect temprect,temprect_alt, condrect, condrect_alt, circle_round ;
const char* batticon[]={
  "h",
  "g",
  "f",
  "e",
  "d",
  "d"
};
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
  settings.TopComp=0;
  settings.BottomComp=0;
  settings.MinuteMarks=0;
}
//////End Configuration///
///////////////////////////
static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) {
    // display the step count
    s_steps=(int)health_service_sum_today(HealthMetricStepCount);
  }
}
static GColor ColorSelect(GColor ColorDay, GColor ColorNight){
  if (settings.NightTheme && settings.IsNightNow ){
    return ColorNight;
  }
  else{
    return ColorDay;
  }
}
static int returntop(int selecttop, int selectbottom){
  int newcode=selecttop*10+selectbottom;
  if (!settings.BTOn){
    if (newcode==0 || newcode==4){
      return 0;
    }
    else if(newcode>=40) {
      return 3;
    }
    else {
      return 1;
    }
  }
  else if (!settings.GPSOn){
    //Top
    if (newcode>=30){
      return 3;
    }
    else if (newcode==0 || newcode==3 || newcode==4){
      return 0;
    }
    else {
      return 2;
    }
  }
  else {
    //Top
    if (selecttop==0){
      return 0;
    }
    else {
      return 3;
    }
  }
}
static int returnbottom(int selecttop, int selectbottom){
  int newcode=selecttop*10+selectbottom;
  //Returs a code for each line: 0 - NULL, 1 - BTOff 2 - GPS Off 3 - OK
  //Case 1
  if (!settings.BTOn){
    //Bottom
    if (newcode>=41 && newcode<=43){
      return 1;
    }
    else if (newcode==4 || newcode==14 || newcode==24 || newcode==34){
      return 3;
    }
    else {
      return 0;
    }
  }
  // Case 2
  else if (!settings.GPSOn){
    //Bottom
    if (newcode==31 || newcode==32 || newcode==41 || newcode==42){
      return 2;
    }
    else if (newcode==3 || newcode==4 || newcode==13 || newcode==14 || newcode==23 || newcode==24 || newcode==34 || newcode==43){
      return 3;
    }
    else {
      return 0;
    }
  }
  //Case 3
  else {
    //Bottom
    if (selectbottom==0 || selecttop==selectbottom){
      return 0;
    }
    else {
      return 3;
    }
  }
};
#if defined (PBL_ROUND)
static void blockmodeforround(GContext * ctx){
  graphics_context_set_stroke_width(ctx, 1);
  for (int i = 0; i < 60; i++){
    int deg1=-3+6*i;
    int deg2=deg1+6;
    GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(deg1));
    GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(deg1));
    GPoint p2=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(deg2));
    GPoint p3=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(deg2));
    if (settings.MinuteMarks==0){
      if (i<=s_minutes){
        graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor,settings.MinColorNight));
        graphics_context_set_stroke_color(ctx, ColorSelect(settings.ForegroundColor,settings.ForegroundColorNight));
      }
      else {
        graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor,settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor,settings.MinColorNight));        
      }      
    }
    else if (settings.MinuteMarks==1){
      if (i<=s_minutes){
        if (i<s_minutes && i%5==0 && i>0){
          graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));        
        }
        else {
          graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        }
      }
      else {
        if (i%5==0 && (i-s_minutes)>1){
          graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));          
        }
        else{
          graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));              
        }
      }
    }
    else if (settings.MinuteMarks==2){
      if (i<=s_minutes){
        if (i<s_minutes && i%5==0 && i>0){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));        
        }
        else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        }
      }
      else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));          
      }      
    }
    else if (settings.MinuteMarks==3){
      if (i<=s_minutes){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
      }
      else {
        if (i%5==0 && (i-s_minutes)>1){
          graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));          
        }
        else{
          graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));              
        }        
      }      
    }
    graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 33, DEG_TO_TRIGANGLE(deg1), DEG_TO_TRIGANGLE(deg2));
    graphics_draw_line(ctx, p0, p1);
    graphics_draw_line(ctx, p2, p3);  
  }  
}
static void mm1_bezel_round(GContext * ctx){
  int minloc=s_minutes/5;
  if (settings.MinuteMarks==1){
    for (int i = 0; i <= minloc; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor,settings.BackgroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }
    for (int i = minloc+1; i <= 12; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.ForegroundColor,settings.ForegroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }      
  }
  else if (settings.MinuteMarks==2){
     for (int i = 0; i <= minloc; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor,settings.MinColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }    
  }
  else if (settings.MinuteMarks==3){
    for (int i = minloc+1; i <= 12; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor,settings.BackgroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }
  }
}
static void mm1_all_round(GContext * ctx){
  graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  graphics_context_set_stroke_width(ctx, 1);
  if (settings.MinuteMarks==1){
    for (int i = 0; i < 12; i++){
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }
  }
  else if (settings.MinuteMarks==2){
    int off=0;
    if (s_minutes%5>0){
      off=1;
    }
    for (int i = 0; i <= s_minutes/5+off; i++){
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }
    GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0));
    GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(0));
    graphics_draw_line(ctx, p0, p1);
  }
  else if (settings.MinuteMarks==3){
    for (int i = s_minutes/5; i <= 12; i++){
      GPoint p0=gpoint_from_polar(circle_round, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30));
      GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(i*30));
      graphics_draw_line(ctx, p0, p1);
    }    
  }    
}
#endif
#if defined (PBL_RECT)
static GPoint pointref(GRect ref, int min){
  int x1;
  int y1;
  if (min<=7){
    x1=ref.origin.x+ref.size.w*(8+min)/16;
    y1=ref.origin.y;
  }
  else if (min<=22){
    x1=ref.origin.x+ref.size.w;
    y1=ref.origin.y+ref.size.h*(min-7)/16;
  }
  else if (min<=37){
    x1=ref.origin.x+ref.size.w*(38-min)/16;
    y1=ref.origin.y+ref.size.h;
  }
  else if (min<=52){
    x1=ref.origin.x;
    y1=ref.origin.y+ref.size.h*(53-min)/16;
  }
  else {
    x1=ref.origin.x+(ref.size.w/2)*(min-52)/8;
    y1=ref.origin.y;
  }
  GPoint retpoint = GPoint(x1, y1);
  return retpoint;  
}
static void arcmodeforsquare(GContext * ctx, GRect ref, int min, int wmin, int hmin){
  graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
  graphics_context_set_stroke_width(ctx, 4);
  // Corners of square
  GPoint c1= GPoint(ref.origin.x+wmin/2, ref.origin.y+hmin/2);
  GPoint c2=GPoint(ref.origin.x+ref.size.w-wmin/2,c1.y);
  GPoint c3=GPoint(c2.x, ref.origin.y+ref.size.h-hmin/2);
  GPoint c4=GPoint(c1.x, c3.y);
  GPoint pointon0=GPoint(ref.origin.x+ref.size.w/2,c1.y);
  int math;
  //Drawing
  if (min<=7){
    math=pointon0.x+(c2.x-pointon0.x)*min/8;
    graphics_draw_line(ctx, pointon0, GPoint(math, c2.y));
  }
  else if (min<=22){
    graphics_draw_line(ctx, pointon0, c2);
    math=c2.y+(c3.y-c2.y)*(min-7)/16;
    graphics_draw_line(ctx, c2, GPoint(c2.x,math));
  }
  else if (min<=37){
    graphics_draw_line(ctx, pointon0, c2);
    graphics_draw_line(ctx, c2, c3);
    math=c4.x+(c3.x-c4.x)*(38-min)/16;
    graphics_draw_line(ctx, GPoint(math,c4.y), c3);
  }
  else if (min<=52){
    graphics_draw_line(ctx, pointon0, c2);
    graphics_draw_line(ctx, c2, c3);
    graphics_draw_line(ctx, c3, c4);
    math=c1.y+(c4.y-c1.y)*(53-min)/16;
    graphics_draw_line(ctx, GPoint(c1.x, math), c4);
  }
  else {
    graphics_draw_line(ctx, pointon0, c2);
    graphics_draw_line(ctx, c2, c3);
    graphics_draw_line(ctx, c3, c4);
    graphics_draw_line(ctx, c4, c1);
    math=c1.x+(pointon0.x-c1.x)*(min-52)/8;
    graphics_draw_line(ctx, c1,GPoint(math,c1.y));
  }
}
static void bezelmodeforsquare(GContext * ctx,GRect ref,int min, int x, int y){
  graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
  int wmin=32;
  int hmin=22;
  if (min<=7){
    graphics_fill_rect(ctx, GRect(x, 0, ref.size.w-x, 22),0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ref.size.w-wmin, hmin, wmin, ref.size.h-hmin),0,GCornerNone);
    graphics_fill_rect(ctx, GRect(0, ref.size.h-hmin, ref.size.w, hmin), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, wmin, ref.size.h), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, ref.size.w/2, hmin), 0, GCornerNone);
  }
  else if (min<=22){
    graphics_fill_rect(ctx, GRect(ref.size.w-wmin, y, wmin, ref.size.h-y),0,GCornerNone);
    graphics_fill_rect(ctx, GRect(0, ref.size.h-hmin, ref.size.w-wmin, hmin), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, wmin, ref.size.h), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, ref.size.w/2, hmin), 0, GCornerNone);
  }
  else if (min<=37){
    graphics_fill_rect(ctx, GRect(0, ref.size.h-hmin, x, hmin), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, wmin, ref.size.h-hmin), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0, 0, ref.size.w/2, hmin), 0, GCornerNone);
  }
  else if (min<=52){
    graphics_fill_rect(ctx, GRect(0, 0, wmin, y), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(wmin, 0, ref.size.w/2-wmin, hmin), 0, GCornerNone);
  }
  else {
    graphics_fill_rect(ctx, GRect(x, 0, ref.size.w/2-x, hmin), 0, GCornerNone);
  }
}
static int x_min(int min, GRect ref, int wmin){
  int x_ref=ref.origin.x;
  int w_ref=ref.size.w;
  int respect=10;
  int math;
  if (min<=7){
    math=(w_ref/2-wmin/2-respect/2)*min/7;
    return x_ref+w_ref/2+math;
  }
  else if (min<=22){
    return x_ref+w_ref-wmin/2;
  }
  else if (min<=37){
    math=(w_ref-wmin-respect)*(37-min)/(37-23);
    return x_ref+wmin/2+respect/2+math;
  }
  else if (min<=52){
    return x_ref+wmin/2;
  }
  else {
    math=(w_ref/2-wmin/2-respect/2)*(min-53)/(60-53);
    return x_ref+wmin/2+respect/2+math;
  }
};
static int y_min (int min, GRect ref, int hmin){
  int y_ref=ref.origin.y;
  int h_ref=ref.size.h;  
  int respect=10;
  int math;
  if (min <=7 || min>=53){
    return y_ref+hmin/2;
  }
  else if (min>=23 && min<=37){
    return y_ref+h_ref-hmin/2;
  }
  else if (min<=22){
    math=(h_ref-hmin-respect)*(min-8)/(22-8);
    return y_ref+hmin/2+respect/2+math;
  }
  else {
    math=(h_ref-hmin-respect)*(52-min)/(52-38);
    return y_ref+hmin/2+respect/2+math;
  }
}
static void mm1_bezel_square(GContext * ctx){
  int minloc=s_minutes/5;
  if (settings.MinuteMarks==1){
    for (int i = 0; i <= minloc; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor,settings.BackgroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }
    for (int i = minloc+1; i <= 12; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.ForegroundColor,settings.ForegroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }      
  }
  else if (settings.MinuteMarks==2){
     for (int i = 0; i <= minloc; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor,settings.BackgroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }    
  }
  else if (settings.MinuteMarks==3){
    for (int i = minloc+1; i <= 12; i++){
      graphics_context_set_stroke_color(ctx, ColorSelect(settings.ForegroundColor,settings.ForegroundColorNight));
      graphics_context_set_stroke_width(ctx, 1);
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }
  }
}
static void blockmodeforsquare(GContext * ctx){
  APP_LOG(APP_LOG_LEVEL_DEBUG,"rectref is x %d y %d w %d h %d",bounds.origin.x,bounds.origin.y,bounds.size.w, bounds.size.h);  
  int steptop=bounds.size.w/16;
  int stepside=8;
  graphics_context_set_stroke_width(ctx, 1);
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Steps are  top %d side %d ",steptop,stepside);
  for (int i = 0; i <=59; i++){
    GRect block;
    if (i==0){
      block=GRect(bounds.origin.x+7*steptop, bounds.origin.y, steptop*2, 22);      
    }
    else if(i<=7) {
      block=GRect(bounds.origin.x+(i+8)*steptop, bounds.origin.y, steptop, 22);
    }
    else if (i<15){
      block=GRect(bounds.origin.x+bounds.size.w-32, 22+stepside*(i-8), 32, stepside);
    }
    else if (i==15){
      block=GRect(bounds.origin.x+bounds.size.w-32, 22+stepside*(i-8), 32, 12);
    }
    else if (i<=22){
      block=GRect(bounds.origin.x+bounds.size.w-32, 90+stepside*(i-16), 32, stepside);
    }
    else if (i<30){
      block=GRect(bounds.origin.x+bounds.size.w-steptop*(i-22), bounds.origin.y+bounds.size.h, steptop, -22);
    }
    else if (i==30){
      block=GRect(bounds.origin.x+bounds.size.w-steptop*(9), bounds.origin.y+bounds.size.h, steptop*2, -22);
    }
    else if(i<=37) {
       block=GRect(bounds.origin.x+bounds.size.w-steptop*(i-21), bounds.origin.y+bounds.size.h, steptop, -22);
    }
    else if (i<45){
      block=GRect(bounds.origin.x,bounds.origin.y+bounds.size.h-22-stepside*(i-37), 32, stepside);
    }
    else if (i==45){
      block=GRect(bounds.origin.x, 22+stepside*(7), 32, 12);
    }
    else if (i<=52){
      block=GRect(bounds.origin.x, 22+stepside*(52-i), 32, stepside);
    }
    else {
      block=GRect(bounds.origin.x+steptop*(i-53), bounds.origin.y, steptop, 22);
    }
    //Rewrite 22
     if (i==22){
      block=GRect(bounds.origin.x+bounds.size.w-32, 90+stepside*(i-16), 32, stepside);
    }    
    if (settings.MinuteMarks==0){
      if (i<=s_minutes){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
      }
      else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
      }
    }
    else if (settings.MinuteMarks==1){
      if (i<=s_minutes){
        if (i<s_minutes && i%5==0 && i>0){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));        
        }
        else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        }
      }
      else {
        if (i%5==0 && (i-s_minutes)>1){
          graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));          
        }
        else{
          graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));              
        }
      }
    }
    else if (settings.MinuteMarks==2){
      if (i<=s_minutes){
        if (i<s_minutes && i%5==0  && i>0){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));        
        }
        else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        }
      }
      else {
        graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));          
      }      
    }
    else if (settings.MinuteMarks==3){
      if (i<=s_minutes){
        graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
        graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
      }
      else {
        if (i%5==0 && (i-s_minutes)>1){
          graphics_context_set_fill_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));          
        }
        else{
          graphics_context_set_fill_color(ctx,ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
          graphics_context_set_stroke_color(ctx,ColorSelect(settings.MinColor, settings.MinColorNight));              
        }        
      }      
    }
    GPoint c1=GPoint(block.origin.x, block.origin.y);
    GPoint c2=GPoint(block.origin.x+block.size.w, block.origin.y);
    GPoint c3=GPoint(block.origin.x+block.size.w,block.origin.y+block.size.h);    
    GPoint c4=GPoint(block.origin.x, block.origin.y+block.size.h);
    graphics_fill_rect(ctx, block, 0, GCornerNone);
    if (i>= 8 && i<=22){
      graphics_draw_line(ctx, c1, c2);
      graphics_draw_line(ctx, c3, c4);
    }
    else if (i>=38 && i<=52){
      graphics_draw_line(ctx, c1, c2);
      graphics_draw_line(ctx, c3, c4);
    }
    else {
      graphics_draw_line(ctx, c1, c4);
      graphics_draw_line(ctx, c2, c3);
    }
    if (i==22){
      graphics_draw_line(ctx, c3, c4);
    }
  }
}
static void mm1_all_square(GContext * ctx){
  graphics_context_set_stroke_color(ctx, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  graphics_context_set_stroke_width(ctx, 1);
  if (settings.MinuteMarks==1){
    for (int i = 0; i < 12; i++){
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }  
  }
 else if (settings.MinuteMarks==2){
   int off=0;
   if (s_minutes%5>0){
     off=1;
   }
    for (int i = 0; i <= s_minutes/5+off; i++){
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }
    GPoint p0=pointref(bounds, 0);
    GPoint p1=pointref(bounds, 0);
    graphics_draw_line(ctx, p0, p1);
  }
  else if (settings.MinuteMarks==3){
    for (int i = s_minutes/5; i <= 12; i++){
      GPoint p0=pointref(inner, i*5);
      GPoint p1=pointref(bounds, i*5);
      graphics_draw_line(ctx, p0, p1);
    }    
  }    
}
#endif
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
  //Main and hour
  bounds=layer_get_bounds(layer);
  #if defined(PBL_ROUND)
  APP_LOG(APP_LOG_LEVEL_INFO, "Round Pebble");
  circle_round=grect_inset(bounds, GEdgeInsets(31));
  graphics_context_set_fill_color(ctx, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  graphics_fill_radial(ctx, circle_round, GOvalScaleModeFitCircle, 12, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
  graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 32, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  inner = grect_inset(bounds, GEdgeInsets(15));    
  // Hour
  hour_rect=grect_centered_from_polar(GRect(bounds.size.w/2,bounds.size.h/2, 0, 0),
                                      GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), GSize(50, 42));
  ampmrect = GRect(hour_rect.origin.x + hour_rect.size.w + 1,
                   hour_rect.origin.y +hour_rect.size.h - 20,
                   0,                   
                   8);
  #else
  APP_LOG(APP_LOG_LEVEL_INFO, "Square Pebble");
  // Prepare canvas
  graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
  graphics_fill_rect(ctx, bounds, 0, GCornersAll);
  graphics_context_set_fill_color(ctx, ColorSelect(settings.BackgroundColor, settings.BackgroundColorNight));
  inner=grect_inset(bounds, GEdgeInsets(22,32,22,32));
  graphics_fill_rect(ctx, inner, 0, GCornersAll);
  hour_rect=grect_centered_from_polar(GRect(bounds.origin.x+bounds.size.w/2, bounds.origin.y+bounds.size.h/2, 0, 0),
                                      GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), GSize(50, 42));
  ampmrect = GRect(hour_rect.origin.x + hour_rect.size.w-30,
                   hour_rect.origin.y +hour_rect.size.h-10,
                   30,
                   18);
  #endif
  graphics_context_set_text_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
  char hournow[4];
  int hourtorect = hourtodraw(clock_is_24h_style(), s_hours);
  snprintf(hournow, sizeof(hournow), "%02d", hourtorect);
  graphics_draw_text(ctx, hournow, FontHour, hour_rect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  if (!clock_is_24h_style()){
    char ampm[2];
    if (s_hours < 12){
      strcpy(ampm, "am");
    }
    else{
      strcpy(ampm, "pm");
    }
    int round=PBL_IF_ROUND_ELSE(1, 0);
    if (round==1){
      graphics_draw_text(ctx, ampm, FontDate, ampmrect, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    }
    else {
      graphics_draw_text(ctx, ampm, FontDate, ampmrect, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
    }
  }
  //Minute
  #if defined(PBL_ROUND)
  if (settings.ClockMode==1){
    graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    char minnow[4];
    snprintf(minnow, sizeof(minnow), "%02d",s_minutes);
    graphics_draw_text(ctx, minnow, FontMinute,
                       grect_centered_from_polar(inner,GOvalScaleModeFitCircle,DEG_TO_TRIGANGLE(360*s_minutes/60),GSize(32, 22)),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  else if (settings.ClockMode==2){
    mm1_all_round(ctx);
    //Dot
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    GPoint dotpos=gpoint_from_polar(grect_inset(inner, GEdgeInsets(2)), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(360*s_minutes/60));
    graphics_fill_circle(ctx, dotpos, 7);
  }
  else if (settings.ClockMode==3){
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, 33, DEG_TO_TRIGANGLE(360*s_minutes/60), DEG_TO_TRIGANGLE(360));
    mm1_bezel_round(ctx);
  }
  else if (settings.ClockMode==4){
    mm1_all_round(ctx);
    graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_arc(ctx, grect_inset(inner, GEdgeInsets(2)), GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360*s_minutes/60));
  }
  else if (settings.ClockMode==5){
    GRect digidot_rect=grect_centered_from_polar(inner,GOvalScaleModeFitCircle,DEG_TO_TRIGANGLE(360*s_minutes/60),GSize(32, 22));
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_fill_radial(ctx, digidot_rect, GOvalScaleModeFillCircle, 100, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
    graphics_context_set_text_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
    char minnow[4];
    snprintf(minnow, sizeof(minnow), "%02d",s_minutes);
    graphics_draw_text(ctx, minnow, FontMinute,
                       digidot_rect,
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  else if (settings.ClockMode==6){
    mm1_all_round(ctx);
    GRect fixline=grect_inset(bounds, GEdgeInsets(29));
    GPoint p0=gpoint_from_polar(fixline, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(360*s_minutes/60));
    GPoint p1=gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(360*s_minutes/60));
    graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_line(ctx, p0, p1);    
  }
  else if (settings.ClockMode==7){
    blockmodeforround(ctx);
  }
  #else
  int xtodraw=x_min(s_minutes, bounds, 32);
  int ytodraw=y_min(s_minutes, bounds, 22);
  if (settings.ClockMode==1){
    graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    char minnow[4];
    snprintf(minnow, sizeof(minnow), "%02d",s_minutes);
    graphics_draw_text(ctx, minnow, FontMinute,
                       GRect(xtodraw-32/2, ytodraw-22/2, 32, 22),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  else if (settings.ClockMode==2){
    mm1_all_square(ctx);
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    GPoint dotpos=GPoint(xtodraw, ytodraw);
    graphics_fill_circle(ctx, dotpos, 7);
  }
  else if (settings.ClockMode==3){
    bezelmodeforsquare(ctx, bounds, s_minutes, xtodraw, ytodraw);
    mm1_bezel_square(ctx);
  }
  else if (settings.ClockMode==4){
    mm1_all_square(ctx);
    arcmodeforsquare(ctx, bounds, s_minutes, 32, 22);
  }
  else if (settings.ClockMode==5){
    GRect digidot_rect=GRect(xtodraw-32/2, ytodraw-22/2, 32, 22);
    graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_fill_rect(ctx, digidot_rect, 0, GCornerNone);
    graphics_context_set_text_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
    char minnow[4];
    snprintf(minnow, sizeof(minnow), "%02d",s_minutes);
    graphics_draw_text(ctx, minnow, FontMinute,
                       digidot_rect,
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
  }
  else if (settings.ClockMode==6){
    mm1_all_square(ctx);
    GRect fixline=grect_inset(bounds, GEdgeInsets(20,30,20,30));;
    GPoint dotinner=pointref(fixline, s_minutes);
    GPoint dotoutter=pointref(bounds, s_minutes);
    graphics_context_set_stroke_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    graphics_context_set_stroke_width(ctx, 4);
    graphics_draw_line(ctx, dotinner, dotoutter);
  }
  else if (settings.ClockMode==7){
    blockmodeforsquare(ctx);
  }
  #endif
  #if defined (PBL_ROUND)
  if (settings.DisplayDate){
    char datenow[44];
    const char * sys_locale = i18n_get_system_locale();
    fetchwday(s_weekday, sys_locale, datenow);
    char convertday[4];
    snprintf(convertday, sizeof(convertday), " %02d", s_day);
    strcat(datenow, convertday);
    graphics_context_set_text_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
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
  #else
  if (settings.DisplayDate){
    if (s_minutes<18 && settings.ClockMode==3){
      graphics_context_set_text_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
      graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    }
    else {
      graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
      graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));
    }
    if (settings.ClockMode==7){
      if (s_minutes<=18){
        graphics_context_set_fill_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_text_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));        
      }
      else {
        graphics_context_set_text_color(ctx, ColorSelect(settings.ForegroundColor, settings.ForegroundColorNight));
        graphics_context_set_fill_color(ctx, ColorSelect(settings.MinColor, settings.MinColorNight));        
      }
    }
    
    char datenow[44];
    const char * sys_locale = i18n_get_system_locale();
    fetchwday(s_weekday, sys_locale, datenow);
    char convertday[4];
    snprintf(convertday, sizeof(convertday), " %02d", s_day);
    strcat(datenow, convertday);
    GRect date_rect_right=GRect(inner.origin.x+inner.size.w, hour_rect.origin.y+8, 32, 30);
    GRect date_rect_left=GRect(inner.origin.x-date_rect_right.size.w, date_rect_right.origin.y,
                               date_rect_right.size.w, date_rect_right.size.h);
    
    
    if (s_minutes>12 && s_minutes<18){
      graphics_fill_rect(ctx, date_rect_left, 0, GCornerNone);
      graphics_draw_text(ctx, datenow, FontDate, date_rect_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    else {
      graphics_fill_rect(ctx, date_rect_right, 0, GCornerNone);
      graphics_draw_text(ctx, datenow, FontDate, date_rect_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
  }
  #endif
  // Complications
  #if defined (PBL_ROUND)
  loc_rect=GRect(hour_rect.origin.x-17, hour_rect.origin.y-20, hour_rect.size.w+34, 25);
  temprect = GRect(hour_rect.origin.x - 10,hour_rect.origin.y + hour_rect.size.h + 1,
                   hour_rect.size.w / 2 + 9,(inner.size.h / 2 - hour_rect.size.h / 2) / 2);
  condrect = GRect(hour_rect.origin.x + hour_rect.size.w / 2 + 1,temprect.origin.y,
                   hour_rect.size.w / 2 +5,(inner.size.h / 2 - hour_rect.size.h / 2) / 2);
  iconstep=GRect(temprect.origin.x, loc_rect.origin.y, 20, loc_rect.size.h);
  //Alt location
  loc_rect_alt=GRect(loc_rect.origin.x+5, temprect.origin.y, loc_rect.size.w-10, loc_rect.size.h+10);
  temprect_alt=GRect(temprect.origin.x, hour_rect.origin.y-temprect.size.h+5, temprect.size.w, temprect.size.h);
  condrect_alt=GRect(condrect.origin.x, temprect_alt.origin.y, condrect.size.w, condrect.size.h);
  iconstep_alt=GRect(iconstep.origin.x, loc_rect_alt.origin.y, 20, loc_rect_alt.size.h);
  numstep=GRect(iconstep.origin.x+iconstep.size.w, iconstep.origin.y, loc_rect_alt.size.w-iconstep_alt.size.w, loc_rect.size.h);
  numstep_alt=GRect(iconstep_alt.origin.x+iconstep_alt.size.w, iconstep_alt.origin.y, loc_rect_alt.size.w-iconstep_alt.size.w, loc_rect_alt.size.h);
  #else
  loc_rect=GRect(inner.origin.x+4, hour_rect.origin.y-35, inner.size.w-8, 35);
  temprect = GRect(hour_rect.origin.x - 10,inner.origin.y+inner.size.h-30,
                   hour_rect.size.w / 2 + 9,20);
  condrect = GRect(hour_rect.origin.x + hour_rect.size.w / 2 + 1,temprect.origin.y,
                   inner.size.w / 2 - 1,30);
  iconstep=GRect(loc_rect.origin.x, loc_rect.origin.y, 20, loc_rect.size.h);
  numstep=GRect(iconstep.origin.x+iconstep.size.w,iconstep.origin.y,loc_rect.size.w-iconstep.size.w,iconstep.size.h);
  // Alt location
  loc_rect_alt=GRect(loc_rect.origin.x, temprect.origin.y, loc_rect.size.w, loc_rect.size.h-5);
  temprect_alt=GRect(temprect.origin.x, hour_rect.origin.y-condrect.size.h, temprect.size.w, temprect.size.h);
  condrect_alt=GRect(condrect.origin.x, temprect_alt.origin.y, condrect.size.w, condrect.size.h);
  iconstep_alt=GRect(loc_rect_alt.origin.x, loc_rect_alt.origin.y, 20, loc_rect_alt.size.h);
  numstep_alt=GRect(iconstep_alt.origin.x+iconstep_alt.size.w,iconstep_alt.origin.y,loc_rect_alt.size.w-iconstep_alt.size.w,iconstep_alt.size.h);
  #endif  
  //Settings by Default
  graphics_context_set_text_color(ctx, ColorSelect(settings.HourColor, settings.HourColorNight));
  // Store batt
  int battery_level = battery_state_service_peek().charge_percent;
  char battperc[20];
  snprintf(battperc, sizeof(battperc), "%d", battery_level);
  strcat(battperc, "%");
  char letterbatt[2];
  strcpy(letterbatt, batticon[battery_level/20]);
  int reftop=returntop(settings.TopComp, settings.BottomComp);
  int refbot=returnbottom(settings.TopComp, settings.BottomComp);
  bool Health=PBL_IF_HEALTH_ELSE(true, false);
  //Draw Comps
  //Top
  if (reftop==1){
    graphics_draw_text(ctx, "a", FontSymbol, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
  else if (reftop==2){
    graphics_draw_text(ctx, "b", FontSymbol, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
  else if (reftop==3){
    if (settings.TopComp==4){
      bool Sq=PBL_IF_RECT_ELSE(true, false);
      if (Sq){
        graphics_draw_text(ctx, letterbatt, FontSymbol, iconstep, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
      }
      else {
        graphics_draw_text(ctx, letterbatt, FontSymbol, iconstep, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      }
      graphics_draw_text(ctx, battperc, FontTemp, numstep, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
    else if (settings.TopComp==3){
      if (Health){
        char stepstring[8];
        snprintf(stepstring, sizeof(stepstring), "%d", s_steps);
        graphics_draw_text(ctx, "c", FontSymbol, iconstep, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
        graphics_draw_text(ctx, stepstring, FontTemp , numstep, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else {
        graphics_draw_text(ctx, "Health NA", FontTemp , loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }
    else if (settings.TopComp==1){
      graphics_draw_text(ctx, citistring, FontCiti, loc_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    else if (settings.TopComp==2){
      graphics_draw_text(ctx, tempstring, FontTemp, temprect_alt, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, condstring, FontCond, condrect_alt, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
  }
  //Bottom
  if (refbot==1){
    graphics_draw_text(ctx, "a", FontSymbol, loc_rect_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
  else if (refbot==2){
    graphics_draw_text(ctx, "a", FontSymbol, loc_rect_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
  else if (refbot==3){
    if (settings.BottomComp==4){
      bool Sq=PBL_IF_RECT_ELSE(true, false);
      if (Sq){
        graphics_draw_text(ctx, letterbatt, FontSymbol, iconstep_alt, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
      }
      else {
        graphics_draw_text(ctx, letterbatt, FontSymbol, iconstep_alt, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
      }
      graphics_draw_text(ctx, battperc, FontTemp, numstep_alt, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
    }
    else if (settings.BottomComp==3){
      if (Health){
        char stepstring[8];
        snprintf(stepstring, sizeof(stepstring), "%d", s_steps);
        graphics_draw_text(ctx, "c", FontSymbol, iconstep_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
        graphics_draw_text(ctx, stepstring, FontTemp , numstep_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      else {
        graphics_draw_text(ctx, "Health NA", FontTemp , loc_rect_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
    }
    else if (settings.BottomComp==1){
      graphics_draw_text(ctx, citistring, FontCiti, loc_rect_alt, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
    else if (settings.BottomComp==2){
      graphics_draw_text(ctx, tempstring, FontTemp, temprect, GTextOverflowModeTrailingEllipsis, GTextAlignmentRight, NULL);
      graphics_draw_text(ctx, condstring, FontCond, condrect, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
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
  } 
  else{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Missing info at loop %d, GPS false", s_loop);
    settings.GPSOn = false; }
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
    } 
    else settings.DisplayDate = true;
  }
  Tuple * topcomp_t=dict_find(iter, MESSAGE_KEY_TopComp);
  if (topcomp_t){
    settings.TopComp=atoi(topcomp_t->value->cstring);
  }
  Tuple * bottomcomp_t=dict_find(iter, MESSAGE_KEY_BottomComp);
  if (bottomcomp_t){
    settings.BottomComp=atoi(bottomcomp_t->value->cstring);
  }
  if (settings.TopComp==1 || settings.BottomComp==1){
    settings.DisplayLoc=true;
  }
  else {
    settings.DisplayLoc=false;
  }
  if (settings.TopComp==2 || settings.BottomComp==2){
    settings.DisplayTemp=true;
  }
  else {
    settings.DisplayTemp=false;
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
  Tuple * markmode_t = dict_find(iter, MESSAGE_KEY_MinuteMarks);
  if (markmode_t){
    settings.MinuteMarks=atoi(markmode_t->value->cstring);
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
  }
  else{
    s_countdown = s_countdown - 1;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Countdown to update %d", s_countdown);
  // Evaluate if is day or night
  int nowthehouris = s_hours * 100 + s_minutes;
  if (settings.HourSunrise <= nowthehouris && nowthehouris <= settings.HourSunset){
    settings.IsNightNow = false;
  } 
  else{
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
  app_message_open(512, 512);
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Try health!");
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Health OK");
  }
  else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Health not available!");
  }
}
static void deinit(){
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks();
  health_service_events_unsubscribe();
}
int main(){
  init();
  app_event_loop();
  deinit();
}