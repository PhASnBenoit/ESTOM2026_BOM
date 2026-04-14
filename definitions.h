#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

//////////////////////////////////////////
// DEFINITIONS
//////////////////////////////////////////
#define COULEUR 0x06  // couleur du PAV
#define TYPE 0x01     // type d'émetteur IR (PAV ou Arrêt bus)
#define ADRIP 0xF8    // 5 derniers bits de l'adresse IP du PAV

typedef enum e_etatsBOM {
  S_INIT,       // La partie n'a pas commencé ou est terminée
  S_JEUENCOURS, // Partie en cours
  S_TRANSFERT  // En cours de transfert (pour les durées de transfert)
} T_ETATSBOM;

// --- VALEURS FIXÉES SELON LE CAHIER DES CHARGES ---
typedef enum e_typesTrameRecu {
  R_INIT = 0,   
  R_DEBUT = 1,  
  R_FIN = 2     
} T_TYPESTRAMERECU;

typedef enum e_typesTrameSend {
  E_BONJOUR = 0,              
  E_DEB_TRANSFERT = 1,        
  E_FIN_TRANSFERT = 2,        
  E_ANNULATION_TRANSFERT = 3, 
  E_CHOC = 4                  
} T_TYPESTRAMESEND;

#endif