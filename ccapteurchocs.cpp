#include "ccapteurchocs.h"

CCapteurChocs::CCapteurChocs() {
  _setup = 0;
  _cpt = 0;
  _nbChocs = 0;
} // method

CCapteurChocs::~CCapteurChocs() {
    _setup = 0;
} // method

//int CCapteurChocs::get_cpt() {
//  return _cpt;
//}

void CCapteurChocs::setup() {
  _nbChocs = 0;
  _lu = false;
  _lastChocTime = 0; // Initialisation du chrono

  if (_setup == 0) {
    _setup = 1;
    // TRES IMPORTANT : On configure la broche ici pour rendre la classe autonome
    pinMode(GPIOCHOC_INT, INPUT_PULLUP);
    attachInterruptArg(
      digitalPinToInterrupt(GPIOCHOC_INT),  
      onGpioChocInterrupt,  
      this,  // On passe l'objet actuel à l'interruption
      RISING     // FALLING  // RISING  // CHANGE
    );
    _cpt = 1;  // setup effectuée pour éliminer l'IT parasite du début
  } // if setup
} // method

/* ISR (Interruption) 
void IRAM_ATTR CCapteurChocs::onGpioChocInterrupt(void *arg) {
  // On récupère notre objet
  CCapteurChocs* instance = static_cast<CCapteurChocs*>(arg);
  // Sécurité : si l'objet n'existe pas, on quitte
  if (instance == nullptr) return; 
  instance->_cpt++;

    int gpio = digitalRead(GPIOCHOC_INT);
    if (gpio == 1) { // doit revenir à l'état haut pour compter un choc
      instance->_nbChocs++;
      instance->_lu = true; // CORRECTION DE L'ERREUR ICI (ajout de instance->)
    } // if gpio
    // On met à jour le chrono

} // method
*/

void IRAM_ATTR CCapteurChocs::onGpioChocInterrupt(void *arg) {
  // On récupère notre objet
  CCapteurChocs* instance = static_cast<CCapteurChocs*>(arg);
  // Sécurité : si l'objet n'existe pas, on quitte
  if (instance == nullptr) return; 

  unsigned long currentTime = millis();
  // ANTI-REBOND NON BLOQUANT
  // On ne rentre ici que si ANTIREBOND (ms) se sont écoulées depuis le dernier changement d'état
  if (currentTime - instance->_lastChocTime > ANTIREBOND) {  // ms
    if (instance->_cpt == 1) { // si le setupt vient d'avoir lieu
      instance->_cpt = 0;
    } else {
      instance->_nbChocs++;
      instance->_lu = true; // CORRECTION DE L'ERREUR ICI (ajout de instance->)
      // On met à jour le chrono
      instance->_lastChocTime = currentTime;
    }
  } // if 150
} // method

void CCapteurChocs::setNbChocs(int nbChocs) {
  _cpt = 0;
  _nbChocs = nbChocs;
} // method

int CCapteurChocs::getNbChocs() {
  _lu = false; // On baisse le drapeau immédiatement après l'avoir lu
  return _nbChocs;
} // method

bool CCapteurChocs::isChocs() {
  return _lu;
} // method
