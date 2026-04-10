#include "cdipswitch.h"

CDipSwitch::CDipSwitch() {
}

CDipSwitch::~CDipSwitch() {
}

void CDipSwitch::setup() {
  // initialisation des E/S des SW
  pinMode(GPIO1, INPUT_PULLUP);
  pinMode(GPIO2, INPUT_PULLUP);
  pinMode(GPIO3, INPUT_PULLUP);
}

int CDipSwitch::getDsCouleur() {
  int sw = digitalRead(GPIO2);
  sw |= (digitalRead(GPIO3)<<1);
  return sw;
}

int CDipSwitch::getType() {
  return digitalRead(GPIO1);
}