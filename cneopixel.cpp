#include "cneopixel.h"

// Constructeur
CNeoPixel::CNeoPixel(int num_leds, int pin, int delay_val)
  : _pixels(num_leds, pin, NEO_GRB + NEO_KHZ800),
    _numLeds(num_leds),
    _delayVal(delay_val)
{}

// Initialisation
void CNeoPixel::begin() {
  _pixels.begin();
  _pixels.clear();
  _pixels.show();
}

// Éteindre toutes les LEDs
void CNeoPixel::clear() {
  _pixels.clear();
  _pixels.show();
}

// Allumer toutes les LEDs avec une couleur
void CNeoPixel::setAll(uint8_t r, uint8_t g, uint8_t b) {
  // TODO BATTERIE FAIBLE
  for (int i = 0; i < _numLeds; i++) {
    _pixels.setPixelColor(i, _pixels.Color(r, g, b));
  } // for
  _pixels.show();
}

int CNeoPixel::progression() {
  return _nbAllumed;
}

void CNeoPixel::setProgression(int coul, int lum, int nb) {
  // TODO BATTERIE FAIBLE
  if (nb > _numLeds)
    nb = _numLeds;
  clear();
  _nbAllumed = nb;
  for (int i = 0; i < nb; i++) {
    _pixels.setPixelColor(i, _pixels.Color((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256));
  } // for
  _pixels.show();
}
// Effet chenillard
/*
void CNeoPixel::chenillard(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < _numLeds; i++) {
    _pixels.clear();
    _pixels.setPixelColor(i, _pixels.Color(r, g, b));
    _pixels.show();
    delay(_delayVal);
  } // for
}*/

void CNeoPixel::off() {
  clear();
}

void CNeoPixel::on(uint8_t coul, uint8_t lum) {
  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;
  setAll((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256);
}

void CNeoPixel::clignote(uint8_t coul, uint8_t lum, uint8_t c) {
  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;
  if (c)
    setAll((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256);
  else
    clear();
}
