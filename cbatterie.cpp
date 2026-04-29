#include "esp32-hal-adc.h"
#include "cbatterie.h"

CBatterie::CBatterie() {
}

CBatterie::~CBatterie() {
}

#define NB_READ 5
float CBatterie::getValue() {
  uint32_t valeur;
  for (int i=0 ; i<NB_READ ; i++) {
    valeur = analogReadMilliVolts(GPIO_ANA);
  } // for
  valeur /= NB_READ;
  float tension = valeur*4.64+0.2;  // tension image après diode 
  return tension;
}
