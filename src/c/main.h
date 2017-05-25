#pragma once
#include <pebble.h>
#define SETTINGS_KEY 1
// A structure containing our settings
typedef struct ClaySettings {
  GColor BezelColor;
  GColor BezelColorN;
  GColor MinColor;
  GColor MinColorN;
  GColor BackColor;
  GColor BackColorN;
  GColor HourColor;
  GColor HourColorN;
  bool WeatherUnit;
  int WeatherCond;
  int UpSlider;
  int HourSunrise;
  int HourSunset;
  int ClockMode;
  char* WeatherTemp;
  bool DisplayDate;
  bool DisplayTemp;
  bool DisplayLoc;
  bool BTOn;
  bool GPSOn;
  bool NightTheme;
  bool IsNightNow;
} __attribute__((__packed__)) ClaySettings;