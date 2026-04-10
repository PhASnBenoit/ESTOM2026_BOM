#ifndef __CDIPSWITCH_H
#define __CDIPSWITCH_H

#include <Arduino.h>

// DIP SW
#define GPIO1 1  // sw1
#define GPIO2 2  // sw2
#define GPIO3 3  // sw3

class CDipSwitch {
public:
  CDipSwitch();
  ~CDipSwitch();

  void setup();
  int getDsCouleur(); 
  int getType();  // 0:PAV, 1:Abri

private:

}; // class



#endif