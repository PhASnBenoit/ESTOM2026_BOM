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
void CNeoPixel::begin(bool type) {
  _type = type;
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
    _pixels.setPixelColor(0, _pixels.Color(rouge[0], rouge[1], rouge[2]));
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
  int r,v,b;
  if (nb > _numLeds) nb = _numLeds;
  _nbAllumed = nb;
  if (lum < 1) lum = 1;
  if (lum > 3) lum = 3;

  if (_type == 1) {  // BUS
    r = (couleurs[4][0]*lum)%256;
    v = (couleurs[4][1]*lum)%256;
    b = (couleurs[4][2]*lum)%256;
  } else {   // BOM
    r = (couleurs[coul][0]*lum)%256;
    v = (couleurs[coul][1]*lum)%256;
    b = (couleurs[coul][2]*lum)%256;
  } // else

  if (bf==true) // LED du milieu indice 0
    _pixels.setPixelColor(0, _pixels.Color(rouge[0], rouge[1], rouge[2]));
  else 
    _pixels.setPixelColor(0, r, v, b);

  if (nb > 0) {
    for (i=1 ; i<_nbAllumed+1 ; i++)
      _pixels.setPixelColor(i, r, v, b);
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
  int r,v,b;

  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;

  if (_type == 1) {  // BUS
    r = (couleurs[4][0]*lum)%256;
    v = (couleurs[4][1]*lum)%256;
    b = (couleurs[4][2]*lum)%256;
  } else {   // BOM
    r = (couleurs[coul][0]*lum)%256;
    v = (couleurs[coul][1]*lum)%256;
    b = (couleurs[coul][2]*lum)%256;
  } // else

  setAll(r, v, b, bf);
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
