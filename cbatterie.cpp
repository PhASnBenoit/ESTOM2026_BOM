#include "esp32-hal-adc.h"
#include "cbatterie.h"

CBatterie::CBatterie() {
}

CBatterie::~CBatterie() {
}

float CBatterie::getValue() {
  uint32_t valeur = analogReadMilliVolts(GPIO_ANA);
  float tension = valeur*4.64+0.2;  // tension image après diode 
  return tension;
}
