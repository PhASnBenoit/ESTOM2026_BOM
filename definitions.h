#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

//////////////////////////////////////////
// DEFINITIONS
//////////////////////////////////////////
#define COULEUR 0x06  // couleur du PAV
#define TYPE 0x01     // type d'émetteur IR (PAV ou Arrêt bus)
#define ADRIP 0xF8    // 5 derniers bits de l'adresse IP du PAV
//////// A DEFINIR ///////////////
#define DUREE_TRANSFERT 5000    // ms
#define DUREE_ENTRE_2BYTES 2000  // ms
#define NBLEDS_PAR_TRANSFERT 2  // nbre de LED à allumer pour chaque transfert
#define SEUIL_BATTERIE_FAIBLE 6.8   // tension seuil batterie faible (V)
//////////////////////////////////

typedef enum e_etatsBOM {
  S_INIT,       // La partie n'a pas commencé ou est terminée
  S_JEUENCOURS, // Partie en cours
  S_TRANSFERT,  // En cours de transfert (pour les durées de transfert)
  S_FIN_TRANSFERT
} T_ETATSBOM;

// --- VALEURS FIXÉES SELON LE CAHIER DES CHARGES ---
typedef enum e_typesTrameRecu {
  R_INIT,
  R_DEBUT,
  R_FIN
} T_TYPESTRAMERECU;

typedef enum e_typesTrameSend {
  E_BONJOUR,              
  E_DEB_TRANSFERT,        
  E_FIN_TRANSFERT,        
  E_ANNULATION_TRANSFERT, 
  E_CHOC,
  E_DIVERS=99             
} T_TYPESTRAMESEND;

#endif