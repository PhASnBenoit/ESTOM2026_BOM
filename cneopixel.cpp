#include "cneopixel.h"

// Constructeur
CNeoPixel::CNeoPixel(int num_leds, int pin, int delay_val)
  : _pixels(num_leds, pin, NEO_GRB + NEO_KHZ800),
    _numLeds(num_leds),
    _delayVal(delay_val)
{
  _nbAllumed = 0;
}

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
void CNeoPixel::setAll(uint8_t r, uint8_t g, uint8_t b, bool bf) {
  if (bf==true) // si batterie faible
    _pixels.setPixelColor(0, _pixels.Color(orange[0], orange[1], orange[2]));
  else 
    _pixels.setPixelColor(0, _pixels.Color(r, g, b));
  for (int i = 1; i < _numLeds; i++) {
    _pixels.setPixelColor(i, _pixels.Color(r, g, b));
  } // for
  _pixels.show();
}

int CNeoPixel::progression() {
  return _nbAllumed;
}

void CNeoPixel::setProgression(int coul, int lum, int nb, bool bf) {
  int i;
  if (nb > _numLeds) nb = _numLeds;
  _nbAllumed = nb;
  if (lum < 1) lum = 1;
  if (lum > 3) lum = 3;

  if (bf==true) // LED du milieu indice 0
    _pixels.setPixelColor(0, _pixels.Color(orange[0], orange[1], orange[2]));
  else 
    _pixels.setPixelColor(0, _pixels.Color((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256));

  if (nb > 0) {
    for (i=1 ; i<_nbAllumed+1 ; i++)
      _pixels.setPixelColor(i, _pixels.Color((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256));
    for (i = _nbAllumed+1 ; i<_numLeds ; i++) 
      _pixels.setPixelColor(i, _pixels.Color(0, 0, 0));
  } else {
    for (i = 1 ; i<_numLeds ; i++)
      _pixels.setPixelColor(i, _pixels.Color(0, 0, 0));
  } // else
  _pixels.show();
}

void CNeoPixel::off() {
  clear();
}

void CNeoPixel::on(uint8_t coul, uint8_t lum, bool bf) {
  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;
  setAll((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256, bf);
}

void CNeoPixel::clignote(uint8_t coul, uint8_t lum, bool bf) {
  static int c=0;
  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;
  if (c==0) c=1; else c=0;
  if (c)
    on(coul, lum, bf);
  else
    clear();
}
