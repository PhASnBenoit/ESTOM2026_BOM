#include "ccapteurchocs.h"

CCapteurChocs::CCapteurChocs() {
  _setup = 0;
} // method

CCapteurChocs::~CCapteurChocs() {
    _setup = 0;
} // method

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
      FALLING  
    );
  } // if setup
} // method

/* ISR (Interruption) */
void IRAM_ATTR CCapteurChocs::onGpioChocInterrupt(void *arg) {
  // On récupère notre objet
  CCapteurChocs* instance = static_cast<CCapteurChocs*>(arg);
  
  // Sécurité : si l'objet n'existe pas, on quitte
  if (instance == nullptr) return; 

  unsigned long currentTime = millis();

  // ANTI-REBOND NON BLOQUANT (Exemple : 250 ms)
  // On ne rentre ici que si 250ms se sont écoulées depuis le dernier vrai choc
  // TODO A perfectionner
  if (currentTime - instance->_lastChocTime > 250) {
    instance->_nbChocs++;
    instance->_lu = true; // CORRECTION DE L'ERREUR ICI (ajout de instance->)
    // On met à jour le chrono
    instance->_lastChocTime = currentTime;
  } // if 250
} // method

void CCapteurChocs::setNbChocs(int nbChocs) {
  _nbChocs = nbChocs;
} // method

int CCapteurChocs::getNbChocs() {
  return _nbChocs;
} // method

// FONCTION AMÉLIORÉE
bool CCapteurChocs::isChocs() {
  bool etatActuel = _lu;
  _lu = false; // On baisse le drapeau immédiatement après l'avoir lu
  return etatActuel;
}