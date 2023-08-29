#include <GyverEncoder.h>
#include <U8glib.h>
#include <EEPROM.h>

#define INIT_ADDR 1023  // номер резервной ячейки
#define INIT_KEY 1     // ключ первого запуска. 0-254, на выбор

#define SW 8
#define DT 9
#define CLK 10
#define PumpPin 11
#define Light1Pin 7
#define Light2Pin 6
#define LineStep 14

long timer = 0;

struct Settings{
  long pump_delay = 43200000; //12h
  long pump_uptime = 5000;
  bool pump_always_on = false;
  bool use_pump = true;
  bool pump_only_on_light = false;

  long vent_delay = 43200000; //12h
  long vent_uptime = 5000;
  bool vent_always_on = false;
  bool use_vent = true;
  bool vent_only_on_light = false;

  long fan_delay = 43200000; //12h
  long fan_uptime = 5000;
  bool fan_always_on = false;
  bool use_fan = true;
  bool fan_only_on_light = false;

  
  long aeration_delay = 43200000; //12h
  long aeration_uptime = 5000;
  bool aeration_always_on = false;
  bool use_aeration = true;
  bool aeration_only_on_light = false;

  uint8_t morning_start = 8;
  uint8_t light_time = 12;
  bool full_light = true;
  bool use_light = true;
};

Settings settings;


bool light_state = false;
bool pump_state = false;
bool vent_state = false;
bool fan_state = false;
bool aeration_state = false;

long light_timer = 0;
long pump_timer = 0;
long vent_timer = 0;
long fan_timer = 0;
long aeration_timer = 0;


uint8_t menu = 0;
uint8_t screen = 0;
uint8_t parameter = 3;
uint8_t menu_param = 0;

bool redraw = true;
bool modify = false;

U8GLIB_SSD1306_128X64 display(U8G_I2C_OPT_NONE);  // I2C / TWI 
Encoder enc(CLK, DT, SW);


void setup() {

  if (EEPROM.read(INIT_ADDR) != INIT_KEY) {
    EEPROM.write(INIT_ADDR, INIT_KEY);
    
    settings.pump_delay = 10;
    settings.pump_uptime = 1000;
    settings.pump_always_on = false;
    settings.use_pump = true;
    settings.pump_only_on_light = true;

    settings.vent_delay = 1000;
    settings.vent_uptime = 1000;
    settings.vent_always_on = false;
    settings.use_vent = true;
    settings.vent_only_on_light = false;

    settings.fan_delay = 1000;
    settings.fan_uptime = 1000;
    settings.fan_always_on = false;
    settings.use_fan = true;
    settings.fan_only_on_light = false;
    
    settings.aeration_delay = 1000;
    settings.aeration_uptime = 1000;
    settings.aeration_always_on = false;
    settings.use_aeration = true;
    settings.aeration_only_on_light = false;

    settings.morning_start = 8;
    settings.light_time = 12;
    settings.full_light = true;
    settings.use_light = true;

    EEPROM.put(0, settings);
  }
  EEPROM.get(0, settings);
  display.setFont(u8g_font_unifontr);
  enc.setType(TYPE2);
  enc.setFastTimeout(50);
  // pinMode(PumpPin, OUTPUT);
  Serial.begin(9600);

}

void DisplayUpdate(){
  if(screen != 0){
    display.drawHLine(0, (parameter % 4) * 14 + 15, 128);
  }
  switch(screen){
    case 0:
      display.drawStr(0, 14, "Light");
      display.setPrintPos(95, 14);
      display.print(light_state? "On" :"Off");
      
      display.drawStr(0, 28, "Full Light");
      display.setPrintPos(95, 28);
      display.print(settings.full_light? "On" :"Off");

      display.drawStr(0, 42, "Vent");
      display.setPrintPos(95, 42);
      display.print(vent_state? "On" :"Off");
      break;
    case 1:
      display.drawStr(0, 14, "Use Light");
      display.setPrintPos(95, 14);
      display.print(settings.use_light? "On" :"Off");

      display.drawStr(0, 28, "Full Light");
      display.setPrintPos(95, 28);
      display.print(settings.full_light? "On" :"Off");
      
      display.drawStr(0, 42, "Light Time");
      display.setPrintPos(95, 42);
      display.print(settings.light_time);

      display.drawStr(0, 56, "Morning At");
      display.setPrintPos(95, 56);
      display.print(settings.morning_start);
      break;
    case 2:
      display.drawStr(0, 14, "Use Pump");
      display.setPrintPos(95, 14);
      display.print(settings.use_pump? "Yes": "No");
      
      display.drawStr(0, 28, "Pump Always");
      display.setPrintPos(95, 28);
      display.print(settings.pump_always_on? "Yes" :"No");

      DisplayMSSettings("Pump Delay", settings.pump_delay, 0, 42, 110, 42);
      DisplayMSSettings("Pump Up.t", settings.pump_uptime, 0, 56, 110, 56);
      break;
    case 3:
      display.drawStr(0, 14, "Use Vent");
      display.setPrintPos(95, 14);
      display.print(settings.use_vent? "Yes": "No");
      
      display.drawStr(0, 28, "Vent Always");
      display.setPrintPos(95, 28);
      display.print(settings.vent_always_on? "Yes" :"No");

      DisplayMSSettings("Vent Delay", settings.vent_delay, 0, 42, 110, 42);
      DisplayMSSettings("Vent Up.t", settings.vent_uptime, 0, 56, 110, 56);

      break;
    case 4:
      display.drawStr(0, 14, "Use Fan");
      display.setPrintPos(95, 14);
      display.print(settings.use_fan? "Yes": "No");
      
      display.drawStr(0, 28, "Fan Always");
      display.setPrintPos(95, 28);
      display.print(settings.fan_always_on? "Yes" :"No");

      DisplayMSSettings("Fan Delay", settings.fan_delay, 0, 42, 110, 42);
      DisplayMSSettings("Fan Up.t", settings.fan_uptime, 0, 56, 110, 56);
      break;
    case 5:
      display.drawStr(0, 14, "Use Aero");
      display.setPrintPos(95, 14);
      display.print(settings.use_aeration? "Yes": "No");
      
      display.drawStr(0, 28, "Aero Always");
      display.setPrintPos(95, 28);
      display.print(settings.aeration_always_on? "Yes" :"No");
      
      DisplayMSSettings("Aero Delay", settings.aeration_delay, 0, 42, 110, 42);
      DisplayMSSettings("Aero Up.t", settings.aeration_uptime, 0, 56, 110, 56);     
      break;
    case 6:
      display.drawStr(0, 14, "Pump");
      display.setPrintPos(70, 14);
      display.print(settings.use_pump? "Always": "OnLight");
      
      display.drawStr(0, 28, "Vent");
      display.setPrintPos(70, 28);
      display.print(settings.pump_always_on? "Always": "OnLight");

      display.drawStr(0, 42, "Fan");
      display.setPrintPos(70, 42);
      display.print(settings.pump_always_on? "Always": "OnLight");

      display.drawStr(0, 56, "Aero");
      display.setPrintPos(70, 56);
      display.print(settings.pump_always_on? "Always": "OnLight");      
      break;
    default:
      break;
  }
}
void DisplayMSSettings(String text, long parameter, u8g_uint_t  xtext, u8g_uint_t  ytext, u8g_uint_t  xparam, u8g_uint_t  yparam){
  int divider = 1000;
  String smh = ",s:";
  if(parameter / 60000 < 1)
  {
    int divider = 1000;
    smh = ",s:"; 
  }
  else if(parameter / 60000 >= 1 && parameter / 60000 <= 59)
  { 
    divider = 60000;
    smh = ",m:";
  }
  else
  {
    divider = 3600000;
    smh = ",h:";
  }
  text += smh;
  display.drawStr(xtext, ytext, text.c_str());
  display.setPrintPos(xparam, yparam);
  display.print(parameter / divider);
  Serial.println(parameter / divider);
}


void ChangeValue(bool increase, bool fast){
  switch (parameter){
    case 4:
      settings.use_light = !settings.use_light;
      break;
    case 5:
      settings.full_light = !settings.full_light;
      break;
    case 6:
      settings.light_time = TimeSettingsTweak(settings.light_time, increase, fast);
      break;
    case 7:
      settings.morning_start = TimeSettingsTweak(settings.morning_start, increase, fast);
      break;
    case 8:
      settings.use_pump = !settings.use_pump;
      break;
    case 9:
      settings.pump_always_on = !settings.pump_always_on;
      break;
    case 10:
      settings.pump_delay = MilisTimeTweak(settings.pump_delay, increase, fast);
      break;
    case 11:
      settings.pump_uptime = MilisTimeTweak(settings.pump_uptime, increase, fast);
      break;
    case 12:
      settings.use_vent = !settings.use_vent;
      break;
    case 13:
      settings.vent_always_on = !settings.vent_always_on;
      break;
    case 14:
      settings.vent_delay = MilisTimeTweak(settings.vent_delay, increase, fast);
      break;
    case 15:
      settings.vent_uptime = MilisTimeTweak(settings.vent_uptime, increase, fast);
      break;
    case 16:
      settings.use_fan = !settings.use_fan;
      break;
    case 17:
      settings.fan_always_on = !settings.fan_always_on;
      break;
    case 18:
      settings.fan_delay = MilisTimeTweak(settings.fan_delay, increase, fast);
      break;
    case 19:
      settings.fan_uptime = MilisTimeTweak(settings.fan_uptime, increase, fast);
      break;
    case 20:
      settings.use_aeration = !settings.use_aeration;
      break;
    case 21:
      settings.aeration_always_on = !settings.aeration_always_on;
      break;
    case 22:
      settings.aeration_delay = MilisTimeTweak(settings.aeration_delay, increase, fast);
      break;
    case 23:
      settings.aeration_uptime = MilisTimeTweak(settings.aeration_uptime, increase, fast);
      break;
    case 24:
      settings.pump_only_on_light = !settings.pump_only_on_light;
      break;
    case 25:
      settings.vent_only_on_light = !settings.vent_only_on_light;
      break;
    case 26:
      settings.fan_only_on_light = !settings.fan_only_on_light;
      break;
    case 27:
      settings.aeration_only_on_light = !settings.aeration_only_on_light;
      break;
    default:
      break;
  }
}

uint8_t TimeSettingsTweak(uint8_t param_to_tweak, bool increase, bool fast){
  param_to_tweak = increase? param_to_tweak + (fast ? 4 : 1) : param_to_tweak - (fast ? 4 : 1);
  if(param_to_tweak < 0)
  {
    param_to_tweak = 0;
  }
  else if(param_to_tweak > 23)
  {
    param_to_tweak = 23;
  }
  return param_to_tweak;
}

long MilisTimeTweak(long param_to_tweak, bool increase, bool fast){
  if(param_to_tweak / 60000 < 1)
  {
    
    param_to_tweak = increase? param_to_tweak + (fast? (10000) : 1000) : param_to_tweak - (fast? (10000) : 1000);
    
  }
  else if(param_to_tweak / 60000 >= 1 && param_to_tweak / 60000 <= 59)
  {
    param_to_tweak = increase? param_to_tweak + (fast? (60000 * 10) : 60000) : param_to_tweak - (fast? (60000 * 10) : 60000);
  }
  else
  {
    param_to_tweak = increase? param_to_tweak + (fast? (3600000 * 10): 3600000) : param_to_tweak - (fast? (3600000 * 10): 3600000);
  }
  if(param_to_tweak < 0) param_to_tweak = 0;
  return param_to_tweak;
}


void pump(bool state){
  pump_state = state;
  switch(state){
    case true:
      pump_timer = millis();
      break;
    case false:
      timer = millis();
      break; 
  }      
  digitalWrite(PumpPin, pump_state);
}

void loop() 
{
  // if(!pump_state && millis() - timer >= settings.pump_delay){
  //   pump(true);
  // }
  //enc.tick();
  // if(pump_state && millis() - pump_timer >= settings.pump_uptime ){
  //   pump(false);  
  // }
  enc.tick();
  if (redraw) 
  {
    display.firstPage();
    do 
    {
        DisplayUpdate();
    } 
    while (display.nextPage());
    redraw = false;
  }

  if (enc.isClick()) {
    if(screen > 0){
      if(modify){
        modify = false;
        EEPROM.put(0, settings);
      }
      else{
        modify = true;
      }
    }
    redraw = true;
  }

  if(enc.isRight())
  {
    if(modify)
    {
      ChangeValue(true, false);
    }
    else
    {
      parameter = parameter > 26? 3: parameter + 1;
      screen = parameter / 4;
    }
    redraw = true;
  }
  if(enc.isRightH())
  {
    if(modify)
    {
      ChangeValue(true, true);
    }
    redraw = true;
  }
  if(enc.isLeftH())
  {
    if(modify)
    {
      ChangeValue(false, true);
    }
    redraw = true;
  } 
  if(enc.isLeft())
  {
    if(modify)
    {
      ChangeValue(false, false);
    }
    else
    {
      parameter = parameter < 4? 27: parameter - 1;
      screen = parameter / 4;
    }
    redraw = true;
  }
}
