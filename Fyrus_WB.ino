/* FYRUS WB v1.0 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// DISPLAY
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


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


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 


  display.clearDisplay();


  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(18, 5);
  display.println("FYRUS WB");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(9, 29);
  display.println("White Balance Meter");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(22, 52);
  display.println("( Version 1.0 )");
  display.display();


  display.display();
  delay(5000);


//     rgb_sensor.getColorTemp();
}



void loop () {

   Read();

  // refresh Metering when button is pressed.
  if (MeteringState == 0) {
  

    rgb_sensor.getColorTemp();

    delay(200);
  }


   display.clearDisplay();

// WHITE BALANCE DISPLAY
   display.drawRect(0, 1, 128, 15, WHITE);
   display.setCursor(27, 5);
   display.setTextSize(1);
   display.print(F("WHITE BALANCE"));

   display.drawRect(0, 18, 128, 46, WHITE);
   display.setTextSize(3);
    if (rgb_sensor.ct > 9999) {
   display.setCursor(38, 30);
   display.print(rgb_sensor.ct / 1000.0, 0);
  } else {
   display.setCursor(31, 30);
   display.print(rgb_sensor.ct / 1000.0, 1);
  }
   display.print(F("K"));

   display.display();
   delay(200);
  }


void Read() {

    MeteringState = digitalRead(Metering);
}
