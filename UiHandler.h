#ifndef UiHandler_h
#define UiHandler_h

#include "DCC.h"
#include <Adafruit_NeoPixel.h>
#include "DCCWaveform.h"

#define RUN_PIN 6
#define ERR_PIN 7
#define BTN_PIN 2
#define NEO_PIN 4
#define NEO_LENGHT 3

#define UI_PERIOD 100
#define UI_BTN_CICLES 5

#define JUMPED_MOTOR_SHIELD     \
   new MotorDriver(3 , 10, UNUSED_PIN, UNUSED_PIN, A0, 2.99, 2000, UNUSED_PIN),    \
   new MotorDriver(11, 5, UNUSED_PIN, UNUSED_PIN, A1, 2.99, 250 , UNUSED_PIN) 

class UiHandler {
  public:
    static void begin(byte btnPin, byte runPin, byte errPin, byte ws2812Pin);
    static void loop();
  
  private:
    UiHandler (byte btnPin, byte runPin, byte errPin, byte ws2812Pin);
    byte btnPin, runPin, errPin, ws2812Pin;
    void doLoop();
    void btnTriggered();
    Adafruit_NeoPixel *pixel;
    void setCurrentColor(int i, DCCWaveform* track);
};

#endif