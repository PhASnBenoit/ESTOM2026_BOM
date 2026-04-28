#ifndef __CNEOPIXEL
#define __CNEOPIXEL

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 9  // Bandeau LED
#define NUM_LEDS 7
#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

//
// A AJUSTER EN FAISANT DES ESSAIS
//
const uint8_t orange[3] = {20, 10, 0};
const uint8_t couleurs[4][3] = {
  {10, 10, 0},   // jaune RAL1018
  {0, 10, 10},   // vert RAL6032
  {0, 10, 10},   // Bleu RAL5015
  {10, 10, 10}   // Gris RAL7011
};  

// RGB théoriques
//    CRGB(255,205,0); // jaune RAL1018
//    CRGB(0,124,89);  // vert RAL6032  
//    CRGB(0,83,135);  // Bleu RAL5015  
//    CRGB(85,93,80);  // Gris RAL7011 

class CNeoPixel {
  private:
    Adafruit_NeoPixel _pixels;
    int _numLeds;
    int _delayVal;
    int _nbAllumed;

    void clear();
    void setAll(uint8_t r, uint8_t g, uint8_t b, bool bf);

  public:
    // Constructeur
    CNeoPixel(int num_leds, int pin, int delay_val);

    void begin();
    void on(uint8_t coul, uint8_t lum, bool bf);
    void off();
    int progression();
    void setProgression(int coul, int lum, int nb, bool bf);
    void clignote(uint8_t coul, uint8_t lum, bool bf);
};

#endif
