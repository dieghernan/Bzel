// Clay Config: see https://github.com/pebble/clay
module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Theme settings"
      },
      {
        "type": "select",
        "messageKey": "ClockMode",
        "label": "Minute Mode",
        "defaultValue": "1",
        "options": [
          {
            "label": "Digital",
            "value": "1"
          },
          {
            "label": "Dot",
            "value": "2"
          },       
        ]
      },
      {
        "type": "toggle",
        "messageKey": "DisplayDate",
        "label": "Date",
        "defaultValue": false
      },     
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x555555",
        "label": "Background"
      },
      {
        "type": "color",
        "messageKey": "ForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Bezel"
      },
      {
        "type": "color",
        "messageKey": "HourColor",
        "defaultValue": "0xFFFFFF",
        "label": "Hour and Complications"
      },
      {
        "type": "color",
        "messageKey": "MinColor",
        "defaultValue": "0x555555",
        "label": "Minute"
      },    
      {"type": "section",
       "items": [
         {
           "type": "heading",
           "defaultValue": "Night Theme",
           "size":4
         } ,
         {
           "type": "toggle",
           "messageKey": "NightTheme",
           "label": "Activate Night Theme",
           "defaultValue": false,
           "description": "When activate requires GPS. Theme applied between sunset and sunrise."
         },
         {
           "type": "color",
           "messageKey": "BackgroundColorNight",
           "defaultValue": "0xFFFFFF",
           "label": "Background"
         },
         {
           "type": "color",
           "messageKey": "ForegroundColorNight",
           "defaultValue": "0x555555",
           "label": "Bezel"
         },
         {
           "type": "color",
           "messageKey": "HourColorNight",
           "defaultValue": "0x555555",
           "label": "Hour and Complications"
         },
         {
           "type": "color",
           "messageKey": "MinColorNight",
           "defaultValue": "0xFFFFFF",
           "label": "Minute"
         }       
       ]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Connection settings"
      },
      {
        "type": "toggle",
        "messageKey": "DisplayLoc",
        "label": "Location",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "DisplayTemp",
        "label": "Weather",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "WeatherUnit",
        "label": "Temperature in Fahrenheit",
        "defaultValue": false,
        "description": "Applicable if Weather toggle is active"
      },
      {
        "type": "select",
        "messageKey": "WeatherProv",
        "defaultValue": "yahoo",
        "label": "Weather Provider",
        "description": "WU and OWM requires authentification. See next section",
        "options": [
          {
            "label": "Yahoo",
            "value": "yahoo"
          },
          {
            "label": "OpenWeatherMap",
            "value": "owm"
          },
          {
            "label": "WeatherUnderground",
            "value": "wu"
          }
        ]
      },
      {
        "type": "input",
        "messageKey": "APIKEY_User",
        "defaultValue": "",
        "label": "API Key",
        "description": "Paste here your API Key. If blank the watch will attempt to request pmkey.xyz",
        "attributes": {
          "placeholder": "eg: xxxxxxxxxxxxx"
        }
      },
      {
        "type": "input",
        "messageKey": "EmailPMKEY",
        "defaultValue": "",
        "label": "pmkey.xyz User",
        "description": "pmkey.xyz is a free service for Pebble users that allows you to store safely all your API keys in a single place. Check <a href=https://www.pmkey.xyz/>pmkey.xyz</a> ",
        "attributes": {
          "placeholder": "eg: john.doe@pmkey.xyz",
          "type": "email"
        }
      },
      {
        "type": "input",
        "messageKey": "PINPMKEY",
        "defaultValue": "",
        "label": "pmkey.xyz PIN",
        "attributes": {
          "placeholder": "eg: 12345"
        }
      },
      {
        "type": "slider",
        "messageKey": "UpSlider",
        "defaultValue": 30,
        "label": "Update frequency (minutes)",
        "description": "More frequent requests will drain your phone battery faster",
        "min": 15,
        "max": 120,
        "step": 15},
    ]
      },
      {
      "type": "submit",
      "defaultValue":"Ok"
      },
      {
      "type": "heading",
      "defaultValue": "version v1.0",
      "size":6
      },
      {
      "type": "heading",
      "defaultValue": "Made in Madrid, Spain",
      "size":6
      }
    ];