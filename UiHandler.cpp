#include "UiHandler.h"
#include "StringFormatter.h"

UiHandler *instance;
long int sampleTime;
#define CURRENT_SAMPLE_MAX 2000
volatile byte interrup;

void btnInterrupHandler(){
    if(interrup==0) interrup=UI_BTN_CICLES;
}

UiHandler::UiHandler(byte btnPin, byte runPin, byte errPin, byte ws2812Pin){
    this->btnPin=btnPin;
    this->runPin=runPin;
    this->errPin=errPin;
    this->ws2812Pin=ws2812Pin;

    pinMode(btnPin, INPUT);
    pinMode(runPin, OUTPUT);
    pinMode(errPin, OUTPUT);
    pinMode(ws2812Pin, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(btnPin),btnInterrupHandler,FALLING);
    interrup=0;

    this->pixel= new Adafruit_NeoPixel(NEO_LENGHT, ws2812Pin, NEO_GRB + NEO_KHZ800);
}



void UiHandler::begin(byte btnPin, byte runPin, byte errPin, byte ws2812Pin){
    instance = new UiHandler(btnPin, runPin, errPin, ws2812Pin);
    sampleTime=0;
}



void UiHandler::doLoop(){
    if(interrup==UI_BTN_CICLES){
        btnTriggered();
    }
    if(interrup>0)interrup--;


    pixel->clear();
    pixel->setPixelColor(1, pixel->Color(0, 0, 0));
    pixel->setPixelColor(2, pixel->Color(0, 0, 0));

    POWERMODE runMain, runProg;
    runMain=DCCWaveform::mainTrack.getPowerMode();
    runProg=DCCWaveform::progTrack.getPowerMode();

    if(runMain == POWERMODE::OFF && runProg== POWERMODE::OFF){
       digitalWrite(runPin,0);
       pixel->setPixelColor(0, pixel->Color(15, 15, 0));
    }else{
        pixel->setPixelColor(0, pixel->Color(0, 15, 0));
       digitalWrite(runPin,1); 
       setCurrentColor(1,&DCCWaveform::mainTrack);
       setCurrentColor(2,&DCCWaveform::progTrack);
    }
    if(runMain == POWERMODE::OVERLOAD || runProg== POWERMODE::OVERLOAD){
        digitalWrite(errPin,1);
        pixel->setPixelColor(0, pixel->Color(150, 0, 0));
    }else{
       digitalWrite(errPin,0); 
    }


    pixel->show();
}

void UiHandler::setCurrentColor(int i, DCCWaveform* track){
    int current=  track->getLastCurrent();
    if (track->getPowerMode()==POWERMODE::OFF) return;
    if (track->getPowerMode()==POWERMODE::OVERLOAD){
        pixel->setPixelColor(i, pixel->Color(200, 0, 0));
        return;
    };
  if(current<CURRENT_SAMPLE_MAX/2){
    int val = map(current,0,CURRENT_SAMPLE_MAX/2,10,150);
    pixel->setPixelColor(i, pixel->Color(0, val, 0));
  }else if(current<CURRENT_SAMPLE_MAX){
    int val = map(current,CURRENT_SAMPLE_MAX/2,CURRENT_SAMPLE_MAX,75,200);
    pixel->setPixelColor(i, pixel->Color(val, val, 0));
  }else{
    int val = map(current,CURRENT_SAMPLE_MAX,CURRENT_SAMPLE_MAX*2,75,200);
    pixel->setPixelColor(i, pixel->Color(val, 0, 0));
  }
}

void UiHandler::loop(){
    if(millis() - sampleTime < UI_PERIOD ) return;
    sampleTime=millis();
    instance->doLoop();
}

void UiHandler::btnTriggered(){
    POWERMODE runMain=DCCWaveform::mainTrack.getPowerMode();
    POWERMODE runProg=DCCWaveform::progTrack.getPowerMode();

    if(runMain == POWERMODE::OFF && runProg== POWERMODE::OFF){
        DCCWaveform::mainTrack.setPowerMode(POWERMODE::ON);
        DCCWaveform::progTrack.setPowerMode(POWERMODE::ON);
        StringFormatter::send(Serial,F("<p1>"));
    }else{
        DCCWaveform::mainTrack.setPowerMode(POWERMODE::OFF);
        DCCWaveform::progTrack.setPowerMode(POWERMODE::OFF);
        StringFormatter::send(Serial,F("<p0>"));
    }


              
}
