#include "cbatterie.h"

CBatterie::CBatterie() {
}

CBatterie::~CBatterie() {
}

float CBatterie::getValue() {
  int valeur = analogRead(GPIO_ANA);
  float tension = valeur * (3.3 / 4095.0); // 12 bits par défaut, tension max 3,3V
  return tension;
}
