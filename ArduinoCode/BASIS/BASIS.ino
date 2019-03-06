//DHT
#include "DHT.h"
//SDCARD
#include <SPI.h>
#include <SD.h>
//LCD
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Fonts/FreeSerif12pt7b.h>

//DHT
#define DHTPIN 22
#define DHTTYPE DHT22
//LIGHT
#define LIGHTPIN A6
//ESP
//#define ESPReset 23
//LCD
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFCC6
#define GREY    0xE71C
#define PINK    0xFE7D
#define DGREEN  0x54C0
#define LCDPOWERPIN A7

//Timer
#define touchRef  100
#define tempRef   10000
#define servRef   1000
#define powerRef  3000
#define minuteRef 60000
#define tempSDRef 300000

//DHT
DHT dht(DHTPIN, DHTTYPE);

//LCD
MCUFRIEND_kbv tft;
const int XP=7,XM=A1,YP=A2,YM=6; //320x480 ID=0x6814
const int TS_LEFT=114,TS_RT=933,TS_TOP=906,TS_BOT=145;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
String mess = "Schatteke!";
uint16_t ID;
bool lcdRestart = false;
//Touchscreen
Adafruit_GFX_Button on_btn, off_btn;
int pixel_x, pixel_y;     //Touch_getXY() updates global vars

bool Touch_getXY(void) {
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.y, TS_LEFT, TS_RT, 0, 480); //.kbv makes sense to me
        pixel_y = map(p.x, TS_TOP, TS_BOT, 0, 320);
    }
    return pressed;
}

#define ERR             -1
#define HOME            0
#define GRAPH           1
#define FUTURE          2
#define SETTINGS        3
#define SETTINGSCOLOR   4
#define SETTINGSSENSORS 5
#define SETTINGSGENERAL 6
#define SETTINGSINFO    7
#define COLORRED        8
#define COLORBLUE       9
#define COLORGREEN      10
#define COLORYELLOW     11
#define COLORCYAN       12
#define COLORMAGENTA    13
#define COLORORANGE     14
#define COLORGREY       15
#define COLORPINK       16
#define COLORDGREEN     17
#define SENSOR1L        18
#define SENSOR1M        19
#define SENSOR2L        20
#define SENSOR2M        21
#define GENERALCLKO     22
#define GENERALCLKHU    23
#define GENERALCLKMU    24
#define GENERALCLKHD    25
#define GENERALCLKMD    26
#define GENERALDC       27
#define GENERALDADU     28
#define GENERALDADDU    29
#define GENERALDAMU     30
#define GENERALDAYU     31
#define GENERALDADD     32
#define GENERALDADDD    33
#define GENERALDAMD     34
#define GENERALDAYD     35
#define INFORIGHT       36
#define INFOLEFT        37
#define GRAPHLEFT       38
#define GRAPHRIGHT      39

int activeMenu = HOME;
int generalClockActive = true;
bool automaticClock = true;
int xDateButtons[4] = {250, 282, 325, 370};
int infoPage = 1;

int getTouchedObject(int x, int y) {
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);
  if ((0 < x) && (x < 120) && (280 < y) && (y < 320)) {
    return HOME;
  }
  if ((120 < x) && (x < 240) && (280 < y) && (y < 320)) {
    return GRAPH;
  }
  if ((240 < x) && (x < 360) && (280 < y) && (y < 320)) {
    return FUTURE;
  }
  if ((360 < x) && (x < 480) && (280 < y) && (y < 320)) {
    return SETTINGS;
  }
  if ((activeMenu == SETTINGS) || (activeMenu == SETTINGSCOLOR) || (activeMenu == SETTINGSSENSORS) || (activeMenu == SETTINGSGENERAL) || (activeMenu == SETTINGSINFO)) {
    if ((0 < x) && (x < 160) && (50 < y) && (y < 90)) {
      return SETTINGSCOLOR;
    }
    if ((0 < x) && (x < 160) && (90 < y) && (y < 130)) {
      return SETTINGSSENSORS;
    }
    if ((0 < x) && (x < 160) && (130 < y) && (y < 170)) {
      return SETTINGSGENERAL;
    }
    if ((0 < x) && (x < 160) && (170 < y) && (y < 210)) {
      return SETTINGSINFO;
    }
  }
  if (activeMenu == SETTINGSCOLOR) {
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 2; j++) {
        if ((180+i*60 < x) && (x < 60*i+230) && (90+j*50 < y) && (y < j*50+130)) {
          return 8 + i + j*5;
        }
      }
    }
  }
  if (activeMenu == SETTINGSSENSORS) {
    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        if ((180+i*220 < x) && (x < i*220+213) && (90+j*80 < y) && (y < j*80+130)) {
          return 18 + i + j*2;
        }
      }
    }
  }
  if (activeMenu == SETTINGSGENERAL) {
    if ((400 < x) && (x < 460) && (70 < y) && (y < 110)) {
      return GENERALCLKO;
    }
    if (!automaticClock) {
      if ((180 < x) && (x < 460) && (230 < y) && (y < 270)) {
        return GENERALDC;
      }
      if (generalClockActive) {
        for (int i = 0; i < 2; i++) {
          for (int j = 0; j < 2; j++) {
            if ((300+i*40 < x) && (x < i*40+320) && (130+j*60 < y) && (y < j*60+150)) {
              return 23 + i + 2*j;
            }
          }
        }
      }
      else {
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 2; j++) {
            if ((xDateButtons[i] < x) && (x < xDateButtons[i]+20) && (130+j*60 < y) && (y < j*60+150)) {
              return 28 + i + j*4;
            }
          }
        }
      }
    }
  }
  if (activeMenu == SETTINGSINFO) {
    if (infoPage == 1) {
      if ((350 < x) && (x < 383) && (230 < y) && (y < 270)) {
        return INFORIGHT;
      }
    }
    if (infoPage == 2) {
      if ((250 < x) && (x < 283) && (230 < y) && (y < 270)) {
        return INFOLEFT;
      }
    }
  }
  if (activeMenu == GRAPH) {
    if ((5 < x) && (x < 38) && (5 < y) && (y < 45)) {
      return GRAPHLEFT;
    }
    if ((442 < x) && (x < 475) && (5 < y) && (y < 45)) {
      return GRAPHRIGHT;
    }
  }
  return -1;
}

uint16_t COL = RED;
String BOTTOMBARTEXT[4] = {"Home", "Graph", "Forecast", "Settings"};

void drawBottomBar(void) {
  tft.setFont(&FreeSerif12pt7b);
  for (int i = 0; i < 4; i++) {
    tft.setTextSize(1);
    tft.setCursor(i*120+5,307);
    if (i == activeMenu) {
      tft.fillRect(i*120,280,120,40,WHITE);
      tft.fillRect(i*120+5,285,110,30,COL);
      tft.setTextColor(BLACK, WHITE);
    }
    else {
      tft.fillRect(i*120,280,120,40,COL);
      tft.fillRect(i*120+5,285,110,30,WHITE);
      tft.setTextColor(BLACK, COL);
    }
    tft.print(BOTTOMBARTEXT[i]);
  }
}

int dayIndex = 5;
String DAYS[7] = {"Ma", "Di", "Wo", "Do", "Vr", "Za", "Zo"};
int day = 22;
int month = 9;
int year = 18;
int hour = 16;
int minute = 56;
float tempIn;
float tempOut = 15.4;
float tempSer = 16.8;
float humIn;
float humOut = 74;
float humSer = 96;
int lightIn;
int lightOut = 714;
float pressure = 1013.25;
long millisLastMinute;

//Forecast data:
float tempMorn[10] = {};
float tempDay[10] = {};
float tempEve[10] = {};
float tempNight[10] = {};

void drawGenHomeScreen(void) {
  tft.setFont(&FreeSerif12pt7b);
  tft.fillRect(0,0,480,280,WHITE);
  tft.setTextSize(2);
  tft.setCursor(60,35);
  tft.setTextColor(COL);
  tft.print("In");
  tft.setCursor(160,35);
  tft.print("Uit");
  tft.setCursor(260,35);
  tft.print("Ser");
  tft.drawLine(0,40,350,40,BLACK);
  tft.drawLine(350,40,350,0,BLACK);

  //Date
  tft.setTextSize(1);
  tft.setCursor(351,25);
  tft.print(DAYS[dayIndex]);
  tft.print(" ");
  if (String(day).length() < 2) {
    tft.print("0");
  }
  tft.print(day);
  tft.print("/");
  if (String(month).length() < 2) {
    tft.print("0");
  }
  tft.print(month);
  tft.print("/");
  if (String(year).length() < 2) {
    tft.print("0");
  }
  tft.print(year);
  tft.setCursor(390,50);
  if (String(hour).length() < 2) {
    tft.print("0");
  }
  tft.print(hour);
  tft.print(":");
  if (String(minute).length() < 2) {
    tft.print("0");
  }
  tft.print(minute);
  tft.drawLine(350,40,350,60,BLACK);
  tft.drawLine(350,60,480,60,BLACK);

  //Temperature
  tft.fillRoundRect(15,65,10,10,5,COL);
  tft.fillRoundRect(18,45,4,25,1,COL);
  tft.drawLine(20,55,20,70,BLACK);
  tft.fillRoundRect(17,67,6,6,3,BLACK);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setCursor(50,52);
  tft.setTextColor(BLACK,WHITE);
  tft.print(tempIn);
  tft.print(" C");
  tft.setCursor(150,52);
  tft.print(tempOut);
  tft.print(" C");
  tft.setCursor(250,52);
  tft.print(tempSer);
  tft.print(" C");
  tft.drawLine(0,80,350,80,BLACK);
  tft.drawLine(350,80,350,60,BLACK);

  //Moist
  tft.fillRoundRect(10,95,20,20,10,COL);
  tft.fillRect(10,95,20,10,WHITE);
  tft.fillTriangle(10,105,20,85,30,105,COL);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setCursor(50,92);
  tft.setTextColor(BLACK,WHITE);
  tft.print(humIn);
  tft.print(" %");
  tft.setCursor(150,92);
  tft.print(humOut);
  tft.print(" %");
  tft.setCursor(250,92);
  tft.print(humSer);
  tft.print(" %");
  tft.drawLine(0,120,350,120,BLACK);
  tft.drawLine(350,120,350,80,BLACK);

  //Light
  tft.fillRoundRect(10,130,20,20,10,COL);
  tft.drawLine(20,128,20,123,BLACK);
  tft.drawLine(20,152,20,157,BLACK);
  tft.drawLine(3,140,8,140,BLACK);
  tft.drawLine(32,140,37,140,BLACK);
  tft.drawLine(12,132,9,129,BLACK);
  tft.drawLine(28,132,31,129,BLACK);
  tft.drawLine(28,148,31,151,BLACK);
  tft.drawLine(12,148,9,151,BLACK);
  tft.setCursor(50,132);
  tft.print(lightIn);
  tft.setCursor(150,132);
  tft.print(lightOut);
  tft.drawLine(0,160,250,160,BLACK);
  tft.drawLine(250,160,250,120,BLACK);

  //Pressure
  tft.setTextSize(2);
  tft.setCursor(365,85);
  tft.setTextColor(COL,WHITE);
  tft.print("Luchtdruk");
  tft.setCursor(280,132);
  tft.setTextColor(BLACK,WHITE);
  tft.print(pressure);
  tft.print(" hPa");
  tft.print("  N");
  tft.drawLine(250,160,480,160,BLACK);

  //DayForecast
  tft.setCursor(10,172);
  tft.setTextColor(COL,WHITE);
  tft.print("Vandaag");
  tft.setCursor(20,260);
  tft.print("Ochtend");
  tft.setCursor(148,260);
  tft.print("Middag");
  tft.setCursor(272,260);
  tft.print("Avond");
  tft.setCursor(395,260);
  tft.print("Nacht");
  for (int i = 1; i < 5; i++) {
    tft.drawLine(i*120,190,i*120,260,BLACK);
    //drawSun(i*120-110,195);
    tft.setCursor(i*120-80,235);
    tft.setTextColor(BLACK,WHITE);
    switch (i) {
      case 1:
        tft.print(String(tempMorn[0]).substring(0,4));
        break;
      case 2:
        tft.print(String(tempDay[0]).substring(0,4));
        break;
      case 3:
        tft.print(String(tempEve[0]).substring(0,4));
        break;
      case 4:
        tft.print(String(tempNight[0]).substring(0,4));
        break;
    }
    tft.print(" C");
  }
  drawSymbol(1*120-110,195,"11d");
  drawSymbol(2*120-110,195,"01d");
  drawSymbol(3*120-110,195,"13d");
  drawSymbol(4*120-110,195,"10n");
}

int graphIndex = 0;
#define numberGraphs 11
String graphTypes[numberGraphs] = {"Temp In", "Temp Uit", "Temp Ser", "Vocht In", "Vocht Uit", "Vocht Ser", "Licht In", "Licht Uit", "Luchtdruk", "Paneel Vol", "Batt Vol"};

void drawGraph(void) {
  tft.fillRect(0,0,480,280,WHITE);
  tft.setFont(NULL);
  tft.setTextSize(2);
  tft.setCursor(43,17);
  tft.setTextColor(COL,WHITE);
  tft.print(graphTypes[graphIndex]);
  tft.setTextColor(BLACK,WHITE);
  tft.setTextWrap(false);
  for (int i = graphIndex+1; i < numberGraphs; i++) {
    tft.print("  ");
    tft.print(graphTypes[i]);
  }
  for (int i = 0; i < graphIndex; i++) {
    tft.print("  ");
    tft.print(graphTypes[i]);
  }
  drawArrowButton(true,5,5);
  tft.fillRect(442,5,38,40,WHITE);
  drawArrowButton(false,442,5);
  tft.drawLine(0,50,480,50,BLACK);
}

void drawSun(int x, int y) { //40x40
  tft.fillRoundRect(x+8,y+8,24,24,12,YELLOW);
  for (float a = 0; a < 6.28; a = a + 6.28/8) {
    int xa = x+20 + int(cos(a)*15);
    int xb = x+20 + int(cos(a)*20);
    int ya = y+20 + int(sin(a)*15);
    int yb = y+20 + int(sin(a)*20);
    tft.drawLine(xa,ya,xb,yb,ORANGE);
  }
}

void drawSymbol(int x, int y, String symbol) { //50x50
  File file = SD.open("symbols/" + symbol + ".bm", FILE_READ);
  char readBuffer[100];
  uint8_t xi = 0;
  uint8_t yi = 0;
  for (int i = 0; i < 50; i++) {
    file.read(readBuffer, 100);
    xi = 0;
    for (int j = 0; j < 50; j++) {
      uint16_t colr = (uint16_t) (readBuffer[j*2] << 8) + (uint8_t) readBuffer[j*2+1];
      if (colr != WHITE) {
        tft.drawLine(x+xi,y+yi,x+xi,y+yi,colr);
      }
      xi ++;
    }
    yi ++;
  }
  
}

void drawSettings(bool full) {
  if (full) {
    tft.setFont(&FreeSerif12pt7b);
    tft.fillRect(0,0,480,280,WHITE);
    tft.setTextColor(COL,WHITE);
    tft.setTextSize(2);
    tft.setCursor(10,35);
    tft.setTextColor(COL);
    tft.print("Instellingen");
  }
  else {
    tft.fillRect(0,51,480,229,WHITE);
    tft.setTextSize(2);
  }
  tft.drawLine(0,50,480,50,BLACK);
  tft.setFont(NULL);
  tft.setTextColor(BLACK,WHITE);
  tft.setCursor(10,62);
  tft.print("Kleur");
  tft.setCursor(10,102);
  tft.print("Sensors");
  tft.setCursor(10,142);
  tft.print("Klok");
  tft.setCursor(10,182);
  tft.print("Informatie");
  for (int i = 0; i < 4; i++) {
    tft.drawLine(0,90+i*40,160,90+i*40,BLACK);
    tft.drawLine(160,90+i*40,160,50+i*40,BLACK);
    tft.fillTriangle(140,80+i*40,140,60+i*40,150,70+i*40,COL);
  }
}

void drawColorSettings(void) {
  tft.fillRect(0,50,159,39,COL);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setTextColor(BLACK,COL);
  tft.setCursor(10,62);
  tft.print("Kleur");
  tft.fillTriangle(140,80,140,60,150,70,BLACK);
  tft.setCursor(180,62);
  tft.setTextColor(BLACK,WHITE);
  tft.print("Kies een kleur:");
  tft.fillRect(180,90,50,40,RED);
  tft.fillRect(240,90,50,40,BLUE);
  tft.fillRect(300,90,50,40,GREEN);
  tft.fillRect(360,90,50,40,YELLOW);
  tft.fillRect(420,90,50,40,CYAN);
  tft.fillRect(180,140,50,40,MAGENTA);
  tft.fillRect(240,140,50,40,ORANGE);
  tft.fillRect(300,140,50,40,GREY);
  tft.fillRect(360,140,50,40,PINK);
  tft.fillRect(420,140,50,40,DGREEN);
}

int SENSOR1RefreshRate = 5; //minutes
int SENSOR2RefreshRate = 5; //minutes

void printSensorRefreshRate(bool sens1) {
  tft.setTextColor(COL,WHITE);
  tft.setFont(NULL);
  tft.setTextSize(2);
  if (sens1) {
    tft.setCursor(250,102);
    tft.print(SENSOR1RefreshRate);
  }
  else {
    tft.setCursor(250,182);
    tft.print(SENSOR2RefreshRate);
  }
  tft.print(" minuten  ");
}

void drawSensorSettings(void) {
  tft.fillRect(0,90,159,39,COL);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setTextColor(BLACK,COL);
  tft.setCursor(10,102);
  tft.print("Sensors");
  tft.fillTriangle(140,120,140,100,150,110,BLACK);
  tft.setCursor(180,62);
  tft.setTextColor(BLACK,WHITE);
  tft.print("SENSOR1 refresh rate:");
  drawArrowButton(true, 180, 90);
  drawArrowButton(false, 400, 90);
  tft.setCursor(180,142);
  tft.print("SENSOR2 refresh rate:");
  drawArrowButton(true, 180,170);
  drawArrowButton(false, 400, 170);
  printSensorRefreshRate(true);
  printSensorRefreshRate(false);
}

void drawArrowButton(bool left, int x, int y) {//33x40
  tft.fillRect(x+1,y+1,30,37,COL);
  tft.drawRect(x,y,33,40,BLACK);
  if (left) {
    tft.fillTriangle(x+21,y+10,x+21,y+30,x+11,y+20,BLACK);
  }
  else {
    tft.fillTriangle(x+11,y+10,x+11,y+30,x+21,y+20,BLACK);
  }
}

void drawGeneralSettings(void) {
  tft.fillRect(0,130,159,39,COL);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setTextColor(BLACK,COL);
  tft.setCursor(10,142);
  tft.print("Klok");
  tft.fillTriangle(140,160,140,140,150,150,BLACK);
  tft.setCursor(180,62);
  tft.setTextColor(BLACK,WHITE);
  tft.print("Klok automatisch");
  tft.setCursor(180,87);
  tft.print("instellen:");
  drawOnOffButton(automaticClock,400,70);
  if (!automaticClock) {
    drawClockSettings();
    drawDateClockButton(generalClockActive,180,230);
  }
}

void drawClockSettings(void) {
  tft.fillRect(161,130,319,150,WHITE);
  if (generalClockActive) {
    drawUpDownButton(true,290,130);
    drawUpDownButton(true,330,130);
    drawUpDownButton(false,290,190);
    drawUpDownButton(false,330,190);
    printGeneralTime();
    }
  else {
    drawUpDownButton(true,250,130);
    drawUpDownButton(true,282,130);
    drawUpDownButton(true,325,130);
    drawUpDownButton(true,370,130);
    drawUpDownButton(false,250,190);
    drawUpDownButton(false,282,190);
    drawUpDownButton(false,325,190);
    drawUpDownButton(false,370,190);
    printGeneralDate();
  }
}

void printGeneralDate(void) {
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextSize(1);
  tft.setTextColor(COL,WHITE);
  tft.fillRect(250,150,160,40,WHITE);
  tft.setCursor(250,177);
  tft.print(DAYS[dayIndex]);
  tft.print(" ");
  if (String(day).length() < 2) {
    tft.print("0");
  }
  tft.print(day);
  tft.print(" / ");
  if (String(month).length() < 2) {
    tft.print("0");
  }
  tft.print(month);
  tft.print(" / ");
  if (String(year).length() < 2) {
    tft.print("0");
  }
  tft.print(year);
}

void printGeneralTime(void) {
  tft.setFont(&FreeSerif12pt7b);
  tft.setTextSize(1);
  tft.setTextColor(COL,WHITE);
  tft.fillRect(288,150,70,40,WHITE);
  tft.setCursor(288,177);
  if (String(hour).length() < 2) {
    tft.print("0");
  }
  tft.print(hour);
  tft.print(" : ");
  if (String(minute).length() < 2) {
    tft.print("0");
  }
  tft.print(minute);
}

void drawDateClockButton(bool clockActive, int x, int y) { //280x40
  tft.drawRect(x,y,280,40,BLACK);
  tft.setFont(NULL);
  tft.setTextSize(2);
  if (clockActive) {
    tft.fillRoundRect(x+1,y+1,139,38,4,COL);
    tft.setTextColor(BLACK,COL);
    tft.setCursor(x+45,y+13);
    tft.print("Klok");
    tft.setTextColor(BLACK,WHITE);
    tft.setCursor(x+180,y+13);
    tft.print("Datum");
  }
  else {
    tft.fillRoundRect(x+140,y+1,139,38,4,COL);
    tft.setTextColor(BLACK,WHITE);
    tft.setCursor(x+45,y+13);
    tft.print("Klok");
    tft.setTextColor(BLACK,COL);
    tft.setCursor(x+180,y+13);
    tft.print("Datum");
  }
}

void drawUpDownButton(bool up, int x, int y) { //20x20
  tft.drawRect(x,y,20,20,BLACK);
  tft.fillRect(x+1,y+1,18,18,COL);
  if (up) {
    tft.fillTriangle(x+3,y+16,x+17,y+16,x+10,y+3,BLACK);
  }
  else {
    tft.fillTriangle(x+3,y+3,x+17,y+3,x+10,y+16,BLACK);
  }
}

void drawOnOffButton(bool on, int x, int y) { //60x40
  tft.fillRect(x,y,60,40,WHITE);
  tft.drawRect(x,y,60,40,BLACK);
  tft.setFont(NULL);
  tft.setTextSize(3);
  if (on) {
    tft.fillRoundRect(x+1,y+1,29,38,4,COL);
    tft.setCursor(x+8,y+10);
    tft.setTextColor(BLACK,COL);
    tft.print("1");
    tft.setTextColor(BLACK,WHITE);
    tft.setCursor(x+38,y+10);
    tft.print("0");
  }
  else {
    tft.fillRoundRect(x+30,y+1,29,38,4,COL);
    tft.setCursor(x+8,y+10);
    tft.setTextColor(BLACK,WHITE);
    tft.print("1");
    tft.setTextColor(BLACK,COL);
    tft.setCursor(x+38,y+10);
    tft.print("0");
  }
}

String lastUpdateSENSOR1 = "12:43:30";
String lastUpdateSENSOR2 = "12:38:42";
String lastUpdatePC = "12:32:56";
float panelVoltage = 4.3;
float battVoltage = 3.6;
String IPAddres = "";

void drawInfoSettings(void) {
  tft.fillRect(0,170,159,39,COL);
  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setTextColor(BLACK,COL);
  tft.setCursor(10,182);
  tft.print("Informatie");
  tft.fillTriangle(140,200,140,180,150,190,BLACK);
  if (infoPage == 1) {
    tft.setCursor(180,62);
    tft.setTextColor(BLACK,WHITE);
    tft.print("Laatste update SENSOR1:");
    tft.setCursor(180,87);
    tft.print(lastUpdateSENSOR1);
    tft.setCursor(180,122);
    tft.print("Laatste update SENSOR2:");
    tft.setCursor(180,147);
    tft.print(lastUpdateSENSOR2);
    tft.setCursor(180,182);
    tft.print("Laatste update PC:");
    tft.setCursor(180,207);
    tft.print(lastUpdatePC);
  
    tft.setCursor(297,240);
    tft.setTextColor(COL,WHITE);
    tft.print("1/2");
    //drawArrowButton(true,250,230);
    drawArrowButton(false,350,230);
    return;
  }
  if (infoPage == 2) {
    tft.setCursor(180,62);
    tft.setTextColor(BLACK,WHITE);
    tft.print("Paneel voltage:");
    tft.setCursor(180,87);
    tft.print(panelVoltage);
    tft.print(" V");
    tft.setCursor(180,122);
    tft.print("Batterij voltage:");
    tft.setCursor(180,147);
    tft.print(battVoltage);
    tft.print(" V");
    tft.setCursor(180,182);
    tft.print("IP adres: ");
    tft.print(IPAddres);
    tft.setCursor(297,240);
    tft.setTextColor(COL,WHITE);
    tft.print("2/2");
    drawArrowButton(true,250,230);
    //drawArrowButton(false,350,230);
    return;
  }
}

int getMaxDay(int month) {
  if ((month == 1) || (month == 3) || (month == 5) ||(month == 7) || (month == 8) || (month == 10) || (month == 12)) {
    return 31;
  }
  if (month == 2) {
    return 28;
  }
  return 30;
}

//Device: 0 (ESP)
//    type1: 0
//        type2: 0
//            IP address
//Device: P (PC)
//    type1: 0
//        type2: 0
//            Time adjustment
//        type2: 1
//            Request for data
//            mes: P01 numberFiles(2),number1(2),number2(2),...,numberN(2)
//            answer: B01 dayCounter(2) fileBASISfileSENSOR1fileSENSOR2 ... ENDENDEND
//            file structure: lengthName(2)name(lengthName)lengthFile(10)file(lengthFile)
//        type2: 2
//            Forecast adjustment
//            mes: P02 numberDays(2),{day1},{day2},...,{dayn}
//            day structure: tempMorn*10(3),tempDay*10(3),tempEve*10(3),tempNight*10(3),tempMax*10(3),tempMin*10(3),icon(3),
//                           descriptionLength(2),description(descriptionLength),humidity*10(4),pressure*100(6),clouds*10(4),
//                           speed*100(4),deg(3),rain*100(4)
//            answer: B02 OK
//        type2: 3
//            Request for live data
//            mes: P03
//            answer: B03 tempBuit#tempSer#tempBin#moistBuit#moistSer#moistBin#Press#LightBuit#PV#BV
//        type2: 4
//            Adjust dayCounter
//            mes: P04 dayCounter(2)
//            answer: B04 OK
//Device: 1 (SENSOR1)
//    type1: 0
//        type2: 0
//            Sensor values, answer = refresh rate
//Device: 2 (SENSOR2)
//    type1: 0
//        type2: 0
//            Sensor values, answer = refresh rate
void handleMess(String mes) {
  Serial.println("KOMT HIER");
  Serial.println(mes);
  char device = mes.charAt(0);
  char type1 = mes.charAt(1);
  char type2 = mes.charAt(2);
  mes = mes.substring(4);
  switch (device) {
    case '0':
      switch (type1) {
        case '0':
          switch (type2) {
            case '0':
              //IP address
              IPAddres = mes;
              break;
          }
          break;
      }
      break;
    case 'P':
      Serial.println("Heeft PC");
      switch (type1) {
        case '0':
        Serial.println("heeft 0");
          switch (type2) {
            case '0':
              //Time adjustment
              handleTimeAdjustment(mes);
              Serial1.println("B" + String(type1) + String(type2) + " OK");
              break;
            case '1':
              //Request for data
              Serial.println("data request");
              handleDataRequest(mes);
              break;
            case '2':
              //Forecast adjustment
              Serial.println("yup");
              handleForecastAdjustment(mes);
              break;
            case '3':
              handleLiveDataRequest(mes);
              break;
            case '4':
              handleSetDayCounter(mes);
              break;
          }
          break;
      }
      break;
    case '1':
      switch (type1) {
        case '0':
          switch (type2) {
            case '0':
              //Sensor values
              handleSENSOR1Values(mes);
              Serial1.println("B" + String(type1) + String(type2) + " " + String(SENSOR1RefreshRate));
              break;
          }
          break;
      }
      break;
    case '2':
      switch (type1) {
        case '0':
          switch (type2) {
            case '0':
              //Sensor values
              handleSENSOR2Values(mes);
              Serial1.println("B" + String(type1) + String(type2) + " " + String(SENSOR2RefreshRate));
              break;
          }
          break;
      }
      break;
  }
}

//            mes: P02 numberDays(2),{day1},{day2},...,{dayn}
//            day structure: tempMorn*10(3),tempDay*10(3),tempEve*10(3),tempNight*10(3),tempMax*10(3),tempMin*10(3),icon(3),
//                           descriptionLength(2),description(descriptionLength),humidity*10(4),pressure*100(6),clouds*10(4),
//                           speed*100(4),deg(3),rain*100(4)
void handleForecastAdjustment(String mes) {
  int numberDays = mes.substring(0,2).toInt();
  mes = mes.substring(3);
  for (int i = 0; i < numberDays; i++) {
    tempMorn[i] = (float) (mes.substring(1,4).toInt()/10.0);
    Serial.println("FORECAST: " + String(tempMorn[i]));
    tempDay[i] = (float) (mes.substring(5,8).toInt()/10.0);
    tempEve[i] = (float) (mes.substring(9,12).toInt()/10.0);
    tempNight[i] = (float) (mes.substring(13,16).toInt()/10.0);
  }
  String answer = "B02 OK";
  Serial1.println(answer);
}

void handleSetDayCounter(String mes) {
  dayCounter = mes.substring(0,2).toInt();
  String answer = "B04 OK";
  Serial1.print(answer);
}

int dayCounter;
#define bufferSize 1000

void handleLiveDataRequest(String mes) { //B03 tempBuit#tempSer#tempBin#moistBuit#moistSer#moistBin#Press#LightBuit#LightBin#PV#BV
  String answer = "B03 ";
  answer += String(tempOut) + "#";
  answer += "00.00#";             //Serre temp
  answer += String(tempIn) + "#";
  answer += String(humOut) + "#";
  answer += "00.00#";             //Serre moist
  answer += String(humIn) + "#";
  answer += String(pressure) + "#";
  answer += String(lightOut) + "#";
  answer += String(lightIn) + "#";
  answer += String(panelVoltage) + "#";
  answer += String(battVoltage) + "#";
  Serial1.println(answer);
}

void handleDataRequest(String mes) { //NN,nn,nn,nn,...
  String answer = "B01 ";
  if (String(dayCounter).length() < 2) {
    answer += "0";
  }
  answer += String(dayCounter);
  //Serial1.print(answer);
  Serial.println("geprint " + answer);
  int num = mes.substring(0,2).toInt();
  if (num == 0) {
    Serial1.println(answer);
    return;
  }
  Serial1.print(answer + " ");
  for (int i = 0; i < num; i++) {
    int tn = mes.substring(3+i*3,5+i*3).toInt();
    transferFile("BASIS" + String(tn) + ".txt");
    transferFile("SENSOR1" + String(tn) + ".txt");
    transferFile("SENSOR2" + String(tn) + ".txt");
    if (i == num-1) {
      Serial1.println("ENDENDEND");
    }
    else {
      Serial1.print(" ");
    }
  }
  
}

void transferFile(String fileName) { //lengthName name lengthFile file
  char readBuffer[bufferSize];
  if (SD.exists(fileName)) {
    File file = SD.open(fileName, FILE_READ);
    String fileName = file.name();
    Serial1.print(extendDigits(fileName.length(), 2));
    Serial1.print(fileName);
    long fileSize = file.size();
    long timesRead = long(fileSize / bufferSize);
    Serial1.print(extendDigits(fileSize, 10));
    for (long i = 0; i < timesRead; i++) {
      file.read(readBuffer, bufferSize);
      Serial1.write(readBuffer,bufferSize);
    }
    file.read(readBuffer, fileSize-timesRead*bufferSize);
    Serial1.write(readBuffer, fileSize-timesRead*bufferSize);
  }
}

String extendDigits(long value, int numberDigits) {
  String result = String(value);
  for (int i = 2; i <= numberDigits; i++) {
    if (result.length() < i) {
      result = "0" + result;
    }
  }
  return result;
}

void handleSENSOR1Values(String mes) { //LLLL#TT.TT#VV.VV#TT.TT#DDDD.DDDD#P.PP#B.BB
  String parts[7];
  int startI = 0;
  int partI = 0;
  for (int i = 0; i < mes.length(); i++) {
    if (mes.charAt(i) == '#') {
      parts[partI] = mes.substring(startI,i);
      partI ++;
      startI = i+1;
    }
  }
  lightOut = parts[0].toInt();
  float tempOut1 = parts[1].toFloat();
  humOut = parts[2].toFloat();
  float tempOut2 = parts[3].toFloat();
  tempOut = (tempOut1 + tempOut2) / 2;
  pressure = parts[4].toFloat();
  panelVoltage = parts[5].toFloat();
  battVoltage = parts[6].toFloat();
  String data = String(lightOut) + "," + String(humOut) + "," + String(tempOut) + "," + String(pressure) + "," + String(panelVoltage) + "," + String(battVoltage);
  writeToSD("SENSOR1", data);
  handleLastUpdate(1);
  if ((activeMenu == HOME) & (~lcdRestart)) {
    tft.setFont(NULL);
    tft.setTextSize(2);
    tft.setTextColor(BLACK,WHITE);
    tft.setCursor(150,132);
    tft.print(lightOut);
    tft.setCursor(150,92);
    tft.print(humOut);
    tft.print(" % ");
    tft.setCursor(150,52);
    tft.print(tempOut);
    tft.print(" C ");
    tft.setCursor(280,132);
    tft.print(pressure);
    tft.print(" hPa");
    if (pressure > 1020) {
      tft.print(" H  ");
    }
    else {
      if (pressure < 1003) {
        tft.print(" L  ");
      }
      else {
        tft.print(" N  ");
      }
    }
  }
}

void handleSENSOR2Values(String mes) {
  
}

void handleLastUpdate(int device) {
  String upd = "";
  if (String(hour).length() < 2) {
    upd += "0";
  }
  upd += String(hour) + ":";
  if (String(minute).length() < 2) {
    upd += "0";
  }
  upd += String(minute) + ":";
  int sec = int((millis() - millisLastMinute)/1000);
  if (String(sec).length() < 2) {
    upd += "0";
  }
  upd += String(sec);
  switch (device) {
    case 0:
      //PC
      lastUpdatePC = upd;
      break;
    case 1:
      //SENSOR1
      lastUpdateSENSOR1 = upd;
      break;
    case 2:
      //SENSOR2
      lastUpdateSENSOR2 = upd;
      break;
  }
}

void handleTimeAdjustment(String mes) { //hhmm dDDMMYY
  if (automaticClock) {
    hour = mes.substring(0,2).toInt();
    minute = mes.substring(2,4).toInt();
    dayIndex = mes.substring(5,6).toInt();
    day = mes.substring(6,8).toInt();
    month = mes.substring(8,10).toInt();
    year = mes.substring(10,12).toInt();
    if ((activeMenu == HOME) & (~lcdRestart)) {
      tft.setFont(&FreeSerif12pt7b);
      tft.setTextColor(COL,WHITE);
      tft.fillRect(351,0,129,59,WHITE);
      tft.setTextSize(1);
      tft.setCursor(351,25);
      tft.print(DAYS[dayIndex]);
      tft.print(" ");
      if (String(day).length() < 2) {
        tft.print("0");
      }
      tft.print(day);
      tft.print("/");
      if (String(month).length() < 2) {
        tft.print("0");
      }
      tft.print(month);
      tft.print("/");
      if (String(year).length() < 2) {
        tft.print("0");
      }
      tft.print(year);
      tft.setCursor(390,50);
      if (String(hour).length() < 2) {
        tft.print("0");
      }
      tft.print(hour);
      tft.print(":");
      if (String(minute).length() < 2) {
        tft.print("0");
      }
      tft.print(minute);
    }
  }
  handleLastUpdate(0);
}

void writeToSD(String fileName, String line) {
  File dataFile = SD.open(fileName + String(dayCounter) + ".txt", FILE_WRITE);
    if (dataFile) {
      String timeLine = " ";
      if (String(hour).length() < 2) {
        timeLine += "0";
      }
      timeLine += String(hour) + ":";
      if (String(minute).length() < 2) {
        timeLine += "0";
      }
      timeLine += String(minute) + ":";
      String sec = String(int((millis() - millisLastMinute)/1000));
      if (sec.length() < 2) {
        timeLine += "0";
      }
      timeLine += sec  + " ";
      if (String(day).length() < 2) {
        timeLine += "0";
      }
      timeLine += String(day) + "/";
      if (String(month).length() < 2) {
        timeLine += "0";
      }
      timeLine += String(month) + "/";
      if (String(year).length() < 2) {
        timeLine += "0";
      }
      timeLine += String(year);
      dataFile.println(line + timeLine);
      dataFile.close();
      Serial.println("Write " + fileName + " to SD: " + line);
    }
    else {
      Serial.println("error opening " + fileName + ".txt");
    }
}

int maxDayCounter = 10;

void handleDayIndex(void) {
  if (dayCounter == maxDayCounter-1) {
    dayCounter = 0;
  }
  else {
    dayCounter ++;
  }
  SD.remove("DayCounter.txt");
  File dataFile = SD.open("DayCounter.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(String(dayCounter));
    dataFile.close();
  }
  SD.remove("BASIS" + String(dayCounter) + ".txt");
  SD.remove("SENSOR1" + String(dayCounter) + ".txt");
  SD.remove("SENSOR2" + String(dayCounter) + ".txt");
}

//SDCARD
Sd2Card card;
SdVolume volume;
SdFile root;
const int chipSelect = 53;

//Timer
unsigned long tim = millis();
unsigned long touchDead = tim + touchRef;
unsigned long tempDead = tim + tempRef;
unsigned long servDead = tim + servRef;
unsigned long powerDead = tim + powerRef;
unsigned long minuteDead = tim + minuteRef;
unsigned long tempSDDead = tim + tempSDRef;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  //DHT
  dht.begin();

  //LIGHT
  pinMode(LIGHTPIN, INPUT);

  //SDCARD
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //return;
  }
  Serial.println("card initialized.");

  //ESP
//  pinMode(ESPReset, OUTPUT);
//  digitalWrite(ESPReset, HIGH);
//  delay(500);

  //LCD
  ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  pinMode(LCDPOWERPIN, INPUT);

  drawBottomBar();
  
  tft.setFont(&FreeSerif12pt7b);
  tft.fillRect(20,0,300,50,GREEN);
  tft.setCursor(20,40);
  tft.setTextColor(RED, BLACK);
  tft.setTextSize(2);
  tft.print(mess);
  //Touchscreen
  
  //Initialize dayCounter
  char readBuffer[2];
  if (SD.exists("DayCounter.txt")) {
    File file = SD.open("DayCounter.txt", FILE_READ);
    file.read(readBuffer, 2);
    dayCounter = ((int) readBuffer[0] - 48)*10 + (int) readBuffer[1] - 48;
  }
  else {
    dayCounter = 0;
  }
}



void loop() {
  if ((millis() > touchDead) && !(lcdRestart)) {
    //Handle touchscreen
    if (Touch_getXY()) {
      int object = getTouchedObject(pixel_x, pixel_y);
      Serial.print("Touched! ");
      Serial.println(object);
      if (object != -1) {
        activeMenu = object;
      }
      switch (object) {
        case HOME:
          //activeMenu = 0;
          drawBottomBar();
          drawGenHomeScreen();
          break;
        case GRAPH:
          //activeMenu = 1;
          drawBottomBar();
          drawGraph();
          break;
        case FUTURE:
          //activeMenu = 2;
          drawBottomBar();
          break;
        case SETTINGS:
          //activeMenu = 3;
          drawBottomBar();
          drawSettings(true);
          break;
        case SETTINGSCOLOR:
          drawSettings(false);
          drawColorSettings();
          break;
        case SETTINGSSENSORS:
          drawSettings(false);
          drawSensorSettings();
          break;
        case SETTINGSGENERAL:
          drawSettings(false);
          drawGeneralSettings();
          break;
        case SETTINGSINFO:
          drawSettings(false);
          drawInfoSettings();
          break;
        case COLORRED:
          COL = RED;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORBLUE:
          COL = BLUE;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORGREEN:
          COL = GREEN;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORYELLOW:
          COL = YELLOW;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORCYAN:
          COL = CYAN;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORMAGENTA:
          COL = MAGENTA;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORORANGE:
          COL = ORANGE;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORGREY:
          COL = GREY;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORPINK:
          COL = PINK;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case COLORDGREEN:
          COL = DGREEN;
          activeMenu = SETTINGSCOLOR;
          drawBottomBar();
          drawSettings(true);
          drawColorSettings();
          break;
        case SENSOR1L:
          if (SENSOR1RefreshRate > 1) {
            SENSOR1RefreshRate--;
          }
          activeMenu = SETTINGSSENSORS;
          printSensorRefreshRate(true);
          break;
        case SENSOR1M:
          if (SENSOR1RefreshRate < 1440) {
            SENSOR1RefreshRate++;
          }
          activeMenu = SETTINGSSENSORS;
          printSensorRefreshRate(true);
          break;
        case SENSOR2L:
          if (SENSOR2RefreshRate > 1) {
            SENSOR2RefreshRate--;
          }
          activeMenu = SETTINGSSENSORS;
          printSensorRefreshRate(false);
          break;
        case SENSOR2M:
          if (SENSOR2RefreshRate < 1440) {
            SENSOR2RefreshRate++;
          }
          activeMenu = SETTINGSSENSORS;
          printSensorRefreshRate(false);
          break;
        case GENERALCLKO:
          activeMenu = SETTINGSGENERAL;
          automaticClock = !automaticClock;
          drawOnOffButton(automaticClock,400,70);
          if (!automaticClock) {
            drawClockSettings();
            drawDateClockButton(generalClockActive,180,230);
          }
          else {
            tft.fillRect(161,130,319,150,WHITE);
          }
          break;
        case GENERALCLKHU:
          activeMenu = SETTINGSGENERAL;
          if (hour < 23) {
            hour ++;
          }
          else {
            hour = 0;
          }
          printGeneralTime();
          break;
        case GENERALCLKHD:
          activeMenu = SETTINGSGENERAL;
          if (hour > 0) {
            hour --;
          }
          else {
            hour = 23;
          }
          printGeneralTime();
          break;
        case GENERALCLKMU:
          activeMenu = SETTINGSGENERAL;
          if (minute < 59) {
            minute ++;
          }
          else {
            minute = 0;
          }
          printGeneralTime();
          break;
        case GENERALCLKMD:
          activeMenu = SETTINGSGENERAL;
          if (minute > 0) {
            minute --;
          }
          else {
            minute = 59;
          }
          printGeneralTime();
          break;
        case GENERALDC:
          activeMenu = SETTINGSGENERAL;
          generalClockActive = !generalClockActive;
          drawClockSettings();
          drawDateClockButton(generalClockActive,180,230);
          break;
        case GENERALDADU:
          activeMenu = SETTINGSGENERAL;
          if (dayIndex < 6) {
            dayIndex ++;
          }
          else {
            dayIndex = 0;
          }
          printGeneralDate();
          break;
        case GENERALDADDU:
          activeMenu = SETTINGSGENERAL;
          if (day < getMaxDay(month)) {
            day ++;
          }
          else {
            day = 1;
          }
          printGeneralDate();
          break;
        case GENERALDAMU:
          activeMenu = SETTINGSGENERAL;
          if (month < 12) {
            month ++;
          }
          else {
            month = 1;
          }
          printGeneralDate();
          break;
        case GENERALDAYU:
          activeMenu = SETTINGSGENERAL;
          if (year < 99) {
            year ++;
          }
          else {
            year = 0;
          }
          printGeneralDate();
          break;
        case GENERALDADD:
          activeMenu = SETTINGSGENERAL;
          if (dayIndex > 0) {
            dayIndex --;
          }
          else {
            dayIndex = 6;
          }
          printGeneralDate();
          break;
        case GENERALDADDD:
          activeMenu = SETTINGSGENERAL;
          if (day > 1) {
            day --;
          }
          else {
            day = getMaxDay(month);
          }
          printGeneralDate();
          break;
        case GENERALDAMD:
          activeMenu = SETTINGSGENERAL;
          if (month > 1) {
            month --;
          }
          else {
            month = 12;
          }
          printGeneralDate();
          break;
        case GENERALDAYD:
          activeMenu = SETTINGSGENERAL;
          if (year > 1) {
            year --;
          }
          else {
            year = 99;
          }
          printGeneralDate();
          break;
        case INFORIGHT:
          activeMenu = SETTINGSINFO;
          infoPage = 2;
          drawSettings(false);
          drawInfoSettings();
          break;
        case INFOLEFT:
          activeMenu = SETTINGSINFO;
          infoPage = 1;
          drawSettings(false);
          drawInfoSettings();
          break;
        case GRAPHLEFT:
          activeMenu = GRAPH;
          if (graphIndex > 0) {
            graphIndex --;
          }
          else {
            graphIndex = numberGraphs-1;
          }
          drawGraph();
          break;
        case GRAPHRIGHT:
          activeMenu = GRAPH;
          if (graphIndex < numberGraphs-1) {
            graphIndex ++;
          }
          else {
            graphIndex = 0;
          }
          drawGraph();
          break;
      }
    }
    touchDead = touchDead + touchRef;
  }
  if (millis() > tempDead) {
    //Handle sensor reading
    humIn = dht.readHumidity();
    tempIn = dht.readTemperature();
    lightIn = analogRead(LIGHTPIN);
    int lcdPower = analogRead(LCDPOWERPIN);
    if (isnan(humIn) || isnan(tempIn)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
//    String data = String(tempIn) + "," + String(humIn) + ",";
//    if (String(lightIn).length() < 2) {
//      data += "0";
//    }
//    if (String(lightIn).length() < 3) {
//      data += "0";
//    }
//    if (String(lightIn).length() < 4) {
//      data += "0";
//    }
//    data += String(lightIn);
//    writeToSD("BASIS", data);
//    Serial.print("Humidity: ");
//    Serial.print(humIn);
//    Serial.print(" %\t");
//    Serial.print("Temperature: ");
//    Serial.print(tempIn);
//    Serial.println(" *C ");
//    Serial.print("Light: ");
//    Serial.print(lightIn);
//    Serial.print(" %\t");
//    Serial.print("LCD: ");
//    Serial.println(lcdPower);

    if ((activeMenu == 0) & (~lcdRestart)) {
      tft.setTextSize(2);
      tft.setFont(NULL);
      tft.setCursor(50,52);
      tft.setTextColor(BLACK,WHITE);
      tft.print(tempIn);
      tft.print(" C");
      tft.setCursor(50,92);
      tft.print(humIn);
      tft.print(" %");
      tft.setCursor(50,132);
      tft.print(lightIn);
      tft.print("   ");
    }

    tempDead = tempDead + tempRef;
  }
  if (millis() > tempSDDead) {
    humIn = dht.readHumidity();
    tempIn = dht.readTemperature();
    lightIn = analogRead(LIGHTPIN);
    int lcdPower = analogRead(LCDPOWERPIN);
    if (isnan(humIn) || isnan(tempIn)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    String data = String(tempIn) + "," + String(humIn) + ",";
    if (String(lightIn).length() < 2) {
      data += "0";
    }
    if (String(lightIn).length() < 3) {
      data += "0";
    }
    if (String(lightIn).length() < 4) {
      data += "0";
    }
    data += String(lightIn);
    writeToSD("BASIS", data);
    tempSDDead = tempSDDead + tempSDRef;
  }
  if (millis() > servDead) {
    //Handle server
    //Check if incomming message
    while (Serial1.available() > 0) {
      Serial.println("hier gekomen");
      mess = Serial1.readStringUntil('\r');
      Serial.println(mess);
      handleMess(mess);
      //Serial.print("Ontvangen: "); Serial.println(mess);
      //Serial1.print("jaja dat gaat goed");
    }

    servDead = servDead + servRef;
  }
  if (millis() > powerDead) {
    //Handle LCD power
    //Serial.println("power");
    int lcdPower = analogRead(LCDPOWERPIN);
    if (lcdPower < 700) {
      for (int i = 2; i < 14; i++) {
        digitalWrite(i, LOW);
      }
      digitalWrite(A0,LOW);
      digitalWrite(A1,LOW);
      digitalWrite(A2,LOW);
      digitalWrite(A3,LOW);
      digitalWrite(A4,LOW);
      digitalWrite(A5,LOW);
      lcdRestart = true;
    }
    else {
      if (lcdRestart) {
        tft.begin(ID);
        tft.setRotation(1);
        tft.fillScreen(WHITE);
        lcdRestart = false;
        
        drawBottomBar();
      }
      
    }

    powerDead = powerDead + powerRef;
  }
  if (millis() > minuteDead) {
    millisLastMinute = millis();
    if (minute == 59) {
      minute = 0;
      if (hour == 23) {
        handleDayIndex();
        hour = 0;
        if (dayIndex == 6) {
          dayIndex = 0;
        }
        else {
          dayIndex ++;
        }
        if (day == getMaxDay(month)) {
          day = 1;
          if (month == 12) {
            month = 1;
            year ++;
          }
          else {
            month ++;
          }
        }
        else {
          day ++;
        }
      }
      else {
        hour ++;
      }
    }
    else {
      minute ++;
    }
    if ((activeMenu == 0) & (~lcdRestart)) {
      tft.setFont(&FreeSerif12pt7b);
      tft.setTextSize(1);
      tft.setTextColor(COL);
      tft.fillRect(390,30,60,25,WHITE);
      tft.setCursor(390,50);
      tft.print(hour);
      tft.print(":");
      if (String(minute).length() < 2) {
        tft.print("0");
      }
      tft.print(minute);
    }
    minuteDead = minuteDead + minuteRef;
  }
}
