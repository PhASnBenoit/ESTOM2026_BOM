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
void CNeoPixel::setAll(uint8_t r, uint8_t g, uint8_t b, bool bf) {

  for (int i = 0; i < _numLeds; i++) {
    if ( (i==_numLeds-1) && bf) // LED du milieu
      _pixels.setPixelColor(i, _pixels.Color(orange[0], orange[1], orange[2]));
    else 
      _pixels.setPixelColor(i, _pixels.Color(r, g, b));
  } // for
  _pixels.show();
}

int CNeoPixel::progression() {
  return _nbAllumed;
}

void CNeoPixel::setProgression(int coul, int lum, int nb, bool bf) {
  int i;
  clear();
  if (nb > _numLeds)
    nb = _numLeds;
  _nbAllumed = nb;
  for (i = 0; i < _nbAllumed; i++) {
    _pixels.setPixelColor(i, _pixels.Color((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256));
  } // for
  for (i = nb; i < _numLeds; i++) {  // si batterie faible LED centrale orange
    if ( (i==_numLeds-1) && bf) // LED du milieu
      _pixels.setPixelColor(i, _pixels.Color(orange[0], orange[1], orange[2]));
    else 
      _pixels.setPixelColor(i, _pixels.Color(0, 0, 0));
  } // for
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

void CNeoPixel::clignote(uint8_t coul, uint8_t lum, uint8_t c, bool bf) {
  if (lum > 3) lum = 3;
  if (lum < 1) lum = 1;
  if (c)
    setAll((couleurs[coul][0]*lum)%256, (couleurs[coul][1]*lum)%256, (couleurs[coul][2]*lum)%256, bf);
  else
    clear();
}
