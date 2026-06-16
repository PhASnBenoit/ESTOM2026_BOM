#ifndef __CCAPTEURCHOCS_H
#define __CCAPTEURCHOCS_H

#include <Arduino.h>

// GPIO des capteurs de choc
#define GPIOCHOC_INT 6 
#define ANTIREBOND 150  // ms

class CCapteurChocs {

public:
  CCapteurChocs();
  ~CCapteurChocs();

  void setup();
  bool isChocs();
  int getNbChocs();
//  int get_cpt();
  void setNbChocs(int nbChocs=0);

private:
  volatile int _nbChocs;
  volatile int _cpt;
  volatile bool _lu;
  volatile int _setup;
  volatile unsigned long _lastChocTime; // NOUVEAU : Chrono pour l'anti-rebond

  static void IRAM_ATTR onGpioChocInterrupt(void *arg);
  
}; // class

#endif