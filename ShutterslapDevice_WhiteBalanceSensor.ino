/*
 * Shutterslap Device | White Balance Sensor | Version 1.0
 * 
 * white balance meter
 * 
 * 1 Button operation
 * 
 * Component:
 * 1. Arduino Nano v3.
 * 2. Oled I2C Display ( 0.96" SSD1306 128x64 ).
 * 3. TCS34725 RGB Sensor.
 * 4. Tactile Button.
 * 5. LiPo 3.7v Battery.
 * 6. 5V Step-Up Boost Converter (optional).
 * 
 * 
 * Wiring:
 * 
 * > Button
 *   pin D2 + GND
 *   * press once or press and hold.
 *
 * > Display & Sensors
 *   VCC -> 3.3V
 *   GND -> GND
 *   SDA -> pin A4
 *   SCL -> pin A5
 *   
 * > Display 7 pins
 *   VCC -> 5V
 *   GND -> GND
 *   D0  -> D8
 *   D1  -> D9
 *   RST -> D10
 *   DC  -> D11
 *   CS  -> D12
 *
 *
 * > Step-Up Boost Converter wiring (optional)
 * 
 *                                           +------+
 *                                           |      |
 *                  +-------+                |      |
 *   BAT+ ----- IN+ | 5V    | OUT+ -----  5V | Nano |
 *                  | BOOST |                |   v3 |
 *   BAT- ----- IN- |       | OUT- ----- GND |      |
 *                  +-------+                +------+
 * 
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// DISPLAY
/* 4 Pin Oled Display */
// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);

/* 7 Pin Oled Display
 *  
 *  VCC -> 5V
 *  GND -> GND
 *  the rest of the pin connect as below
*/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_CLK              8   // PIN D8
#define OLED_MOSI             9   // PIN D9
#define OLED_RESET            10  // PIN D10
#define OLED_DC               11  // PIN D11
#define OLED_CS               12  // PIN D12
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


// BUTTONS
#define Metering              2    // Tactile button attach to D2 + GND

boolean MeteringState;


// SENSOR CALIBRATION
#define TCS34725_R_Coef       0.136
#define TCS34725_G_Coef       1.000
#define TCS34725_B_Coef       -0.444
#define TCS34725_GA           1.0
#define TCS34725_DF           310.0
#define TCS34725_CT_Coef      3810.0
#define TCS34725_CT_Offset    1391.0


float ct;
float lux;


class ColorSensor {
  
    public:
  
    void getColorTemp();
    uint16_t r, g, b, c, ir;
    uint16_t ct, lux, r_comp, g_comp, b_comp;
    float cpl;
} rgb_sensor;

void ColorSensor::getColorTemp() {

    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);
  
    tcs.getRawData(&r, &g, &b, &c);

    // DN40 calculations
    ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
    r_comp = r - ir;
    g_comp = g - ir;
    b_comp = b - ir;
  
    cpl = (700) / (TCS34725_GA * TCS34725_DF);
    lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
    ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;

}


void setup () {

  pinMode(Metering, INPUT_PULLUP);

  // DISPLAY STARTS
  // display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // 4 pins oled
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // 7 pins oled

  display.clearDisplay();

  display.drawRect(0, 1, 128, 15, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(11, 5);
  display.println("SHUTTERSLAP DEVICE");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27, 24);
  display.println("WHITE BALANCE");
  display.setCursor(47, 34);
  display.println("SENSOR");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 52);
  display.println("[ Version 1.0 ]");
  display.display();

  display.display();
  delay(5000);


/* rgb_sensor.getColorTemp(); */

}


void loop () {

   Read();

  // refresh Metering when button is pressed.
  if (MeteringState == 0) {

    rgb_sensor.getColorTemp();

    /* delay(200); */
  }


   display.clearDisplay();

// WHITE BALANCE DISPLAY
   display.drawRect(0, 1, 128, 15, WHITE);
   display.setTextSize(1);
//   display.setCursor(27, 5);
//   display.print(F("WHITE BALANCE"));
   display.setCursor(11, 5);
  display.println("SHUTTERSLAP DEVICE");

   display.drawRect(0, 18, 128, 46, WHITE);
   display.setTextSize(3);
    if (rgb_sensor.ct > 9999) {
   display.setCursor(27, 25);
   display.print(rgb_sensor.ct / 100, 0);
  // display.setCursor(38, 25);
  // display.print(rgb_sensor.ct / 1000.0, 0);
  } else {
   display.setCursor(20, 25);
   display.print(rgb_sensor.ct / 100, 1);
  // display.setCursor(31, 25);
  // display.print(rgb_sensor.ct / 1000.0, 1);
  }
   display.print(F("00K"));


   display.setTextSize(1);
   display.setCursor(4, 53);
   if (rgb_sensor.ct > 9001 && rgb_sensor.ct < 9999) {
     display.setCursor(52, 51);
   display.print(F("BLUE"));
   } else if (rgb_sensor.ct > 8000 && rgb_sensor.ct < 9000) {
     display.setCursor(46, 51);
   display.print(F("CLOUDY"));
   } else if (rgb_sensor.ct > 7000 && rgb_sensor.ct < 7999) {
     display.setCursor(49, 51);
   display.print(F("SHADE"));
   } else if (rgb_sensor.ct > 6000 && rgb_sensor.ct < 6999) {
     display.setCursor(40, 51);
   display.print(F("OVERCAST"));
   } else if (rgb_sensor.ct > 5700 && rgb_sensor.ct < 5999) {
    display.setCursor(25, 51);
   display.print(F("DAYLIGHT COOL"));
   } else if (rgb_sensor.ct > 5500 && rgb_sensor.ct < 5699) {
     display.setCursor(40, 51);
   display.print(F("DAYLIGHT"));
   } else if (rgb_sensor.ct > 5000 && rgb_sensor.ct < 5599) {
    display.setCursor(25, 51);
   display.print(F("DAYLIGHT WARM"));
   } else if (rgb_sensor.ct > 4000 && rgb_sensor.ct < 4999) {
    display.setCursor(52, 51);
   display.print(F("WARM"));
   } else if (rgb_sensor.ct > 3001 && rgb_sensor.ct < 3999) {
    display.setCursor(40, 51);
   display.print(F("TUNGSTEN"));
   } else if (rgb_sensor.ct > 2000 && rgb_sensor.ct < 3000) {
    display.setCursor(22, 51);
   display.print(F("SUNRISE/SUNSET"));
   } else {
    display.setCursor(22, 51);
   display.print(F("PRESS TO START"));
   }
   
/*
   display.setTextSize(1);
   display.setCursor(4, 53);
    if (rgb_sensor.lux > 9999) {
   display.print(rgb_sensor.lux, 0);
    } else {
   display.print(rgb_sensor.lux, 1);
    }
    display.println(F("lx"));
*/

    
   display.display();
   /* delay(200); */
  }


void Read() {

    MeteringState = digitalRead(Metering);
}
