#ifndef __CCAPTEURCHOCS_H
#define __CCAPTEURCHOCS_H

#include <Arduino.h>

// GPIO des capteurs de choc
#define GPIOCHOC_INT 5  // A VERIFIER avec ton montage

class CCapteurChocs {

public:
  CCapteurChocs();
  ~CCapteurChocs();

  void setup();
  bool isChocs();
  int getNbChocs();
  void setNbChocs(int nbChocs=0);

private:
  volatile int _nbChocs;
  volatile bool _lu;
  volatile unsigned long _lastChocTime; // NOUVEAU : Chrono pour l'anti-rebond

  static void IRAM_ATTR onGpioChocInterrupt(void *arg);
  
}; // class

#endif