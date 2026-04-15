#ifndef __CBATTERIE_H
#define __CBATTERIE_H

#include <Arduino.h>

// DIP SW
#define GPIO_ANA A8  // tension batterie

class CBatterie {
public:
  CBatterie();
  ~CBatterie();
  float getValue(); 
private:
}; // class

#endif