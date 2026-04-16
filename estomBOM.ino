////////////////////////////////
//
//  PROGRAMME BOM (ESTOM 2026)
//  V5 - NeoPixel Adafruit & Protocole JSON v1.6
//  v2.0 Réseau OK, IR ok Choc à tester
//  v2.1 Version avec affichage debug 
//  v2.2 Modif champs JSON et INIT
//  v2.3 Amélioration CCapteurChocs (gestion des rebonds)
//  v2.4 Amélioration durée transfert
//  v2.5 Intégration classe CBatterie
//  v2.6 Corrections bug transferts
//  v2.7 Ajout JSON de debug
////////////////////////////////
#define VER "2.7"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "definitions.h"
#include "ccapteurchocs.h"
#include "cdipswitch.h"
#include "cneopixel.h"
#include "cbatterie.h"

/////////////////////////////////////////
// VARIABLES ET INSTANCIATION GLOBALES
/////////////////////////////////////////
CDipSwitch ds;   
CCapteurChocs cc;
CBatterie batt;  // pas de setup
CNeoPixel afficheur(NUM_LEDS, LED_PIN, DELAYVAL);
WiFiClient clientTcp;  

// PARAMS WiFi
//const char *ssid = "STS_C12";
//const char *password = "PervasioN";
const char *ssid = "AP-ESTOM";
const char *password = "ESTOM2025";
// Adresse IP et port du serveur TCP
const char *tcpAddress = "192.168.0.10";
const uint16_t tcpPort = 5005;

// etats et valeurs de l'application
int g_dsCouleur=0;  
int g_type=0;
int g_adrIpPav;
int g_luminosite=1;
int g_dep_time=0;
int g_dep_dureeEntreDeuxBytes=0; 
int g_dep_pausePourReprise=0;
int g_deltaT=0;
bool g_batterie_faible=false;
int g_c=0;
String g_message="";

T_ETATSBOM _etatBOM = S_INIT;

////////////////////////////////////////////////////////////////////////////
void connectToWiFi() {
  Serial.print("Connexion au WiFi");
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(1000);
    Serial.print(".");
  } // wh
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnecté au WiFi");
    Serial.print("Adresse IP locale : ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nÉchec de connexion WiFi. RESTART !");
    delay(2000);
    ESP.restart();
  } // else 
} 

///////////////////////////////////////////////////////////////////////////////
bool connectToServer() {
  Serial.println("Connexion au serveur TCP...");
  if (clientTcp.connect(tcpAddress, tcpPort)) {
    Serial.println("Connexion TCP réussie.");
    return true;
  } else {
    Serial.println("Échec de connexion TCP.");
    return false;
  } // else
} 

///////////////////////////////////////////////////////////////////////////////
void sendMessageToServer(T_TYPESTRAMESEND typeTrame) {
  if (!clientTcp.connected()) {
    Serial.println("Connexion perdue pour envoi, tentative de reconnexion...");
    if (!connectToServer()) {
      Serial.println("Échec de reconnexion pour envoi.");
      return;
    } // if connect
  } // if connect
  
  DynamicJsonDocument doc(256);
  // TOUTES LES VALEURS DOIVENT ETRE DES STRING (v1.6)
  doc["status"] = String(typeTrame); 

  switch(typeTrame) {
    case E_BONJOUR: 
      doc["type"] = "BOM"; 
      doc["couleur"] = String(g_dsCouleur); 
    break;
    
    case E_DEB_TRANSFERT:  
      doc["ipPAV"] = String(g_adrIpPav); break;
    
    case E_FIN_TRANSFERT:  
      doc["leds"] = String(afficheur.progression());
      doc["ipPAV"] = String(g_adrIpPav);
    break;
    
    case E_ANNULATION_TRANSFERT: 
      doc["leds"] = String(afficheur.progression());
      doc["ipPAV"] = String(g_adrIpPav);
    break;
    
    case E_CHOC:  
      doc["collisions"] = String(cc.getNbChocs()); break;

    case E_DIVERS: // Pour des messages de debug car pas de voie série sur le BOM
      doc["texte"] = g_message; break;
  } // sw
  
  serializeJson(doc, clientTcp); // envoi TCP vers serveur
  clientTcp.println(); 
  
  Serial.print("Infos envoyées vers le serveur : ");
  serializeJson(doc, Serial); 
  Serial.println();
} 

/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);  // vitesse obligatoire pour recevoir le caractère du uC
  Serial.print("estomBOM v");
  Serial.println(VER);

  // connexion WIFI
  connectToWiFi();
  // connexion au serveur TCP
  while (!connectToServer()) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\nÉchec de connexion WiFi. RESTART !");
      delay(2000);
      ESP.restart();
    } // if Wifi
  } // wh

    // lecture des sw au démarrage seulement
  ds.setup();  
  g_dsCouleur = ds.getDsCouleur();
  g_type = ds.getType();
  Serial.print("Couleur détectée : ");
  Serial.println(g_dsCouleur);
  Serial.print("Type détectée : ");
  Serial.println(g_type);

  // batterie
  float nivBatt = batt.getValue();
  g_batterie_faible = (nivBatt<=SEUIL_BATTERIE_FAIBLE?true:false);
  Serial.printf("Batterie : %f\n", nivBatt);

  // init couleur et bandeau LED (NeoPixel)
  Serial.print("Luminosité de départ : ");
  Serial.println(g_luminosite);
  afficheur.begin(); // require to intialize object
  afficheur.on(g_dsCouleur, g_luminosite, g_batterie_faible);
  delay(500);
  afficheur.off();

  sendMessageToServer(E_BONJOUR);
  Serial.println("Fin du SETUP");
  delay(500);
} 

//////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  if (!clientTcp.connected()) {
    Serial.println("\nÉchec de connexion au serveur. RESTART !");
    ESP.restart();
  } // if tcp connected

  // test niveau batterie
  float nivBatt = batt.getValue();
  g_batterie_faible = (nivBatt<=SEUIL_BATTERIE_FAIBLE?true:false);

  // ==========================================
  // LECTURE TRAME TCP (SUPERVISION)
  // ==========================================
  if (clientTcp.available()) {
    String trameRec = clientTcp.readStringUntil('\n');
    Serial.println(trameRec);
    StaticJsonDocument<512> data;
    DeserializationError error = deserializeJson(data, trameRec);
    if (error) {
      Serial.print("Erreur JSON : ");
      Serial.println(error.f_str());
      return;
    } // if error
    if (!data.containsKey("ordre")) {
      Serial.println("Trame invalide (pas de clé 'ordre').");
      return;
    } // if data

    int ordre = data["ordre"].as<String>().toInt();
    Serial.print("Ordre reçu : "); Serial.println(ordre);

    int etatJeu;
    switch (ordre) {  
      case R_INIT: 
        Serial.println("Trame INIT reçue");
        etatJeu = (data["etat"].as<String>().toInt());
        _etatBOM = (etatJeu==S_JEUENCOURS?S_JEUENCOURS:S_INIT);
        if (_etatBOM != S_INIT) {
           cc.setup(); 
        } // if _etatBOM
        cc.setNbChocs(data["collisions"].as<String>().toInt()); 
        afficheur.setProgression(g_dsCouleur, g_luminosite, data["leds"].as<String>().toInt(), g_batterie_faible);
      break;
        
      case R_DEBUT: 
        Serial.println("Trame DEBUT PARTIE reçue");
        _etatBOM = S_JEUENCOURS;
        if (data.containsKey("luminosite")) {
          g_luminosite = data["luminosite"].as<String>().toInt();
        } // if lum
        cc.setup();
        afficheur.off();
        cc.setNbChocs(); // RAZ du nombre de chocs
      break;

      case R_FIN: 
        Serial.println("Trame FIN PARTIE reçue");
        _etatBOM = S_INIT;
        // TODO vérifier que tout est réinitialisé
      break;
        
      default:
        Serial.println("Ordre inconnu."); 
      break;
    } // sw 
  } 
  
  // ==========================================
  // GESTION DES CHOCS
  // ==========================================
  if (_etatBOM != S_INIT) {
    if (cc.isChocs()) {
      Serial.print("Chocs = ");
      Serial.println(cc.getNbChocs());
      sendMessageToServer(E_CHOC);
    } // if cc
  } 

  // ==========================================
  // RÉCEPTION DU CARACTERE VIA INFRAROUGE (PAV)
  // ==========================================   
  int coulPAV;
  int typePAV;
  if (Serial.available() > 0) {
    char car = Serial.read();
    Serial.print("Caractère reçu : ");
    Serial.println(car, HEX);
    if ( (_etatBOM!=S_INIT) && (_etatBOM!=S_FIN_TRANSFERT) ) {
      // décodage
      if (car != 0) {  // parfois lecture de parasite valeur 0
        coulPAV = (car&COULEUR)>>1;
        typePAV = car&TYPE;
        if ( (coulPAV==g_dsCouleur) && (typePAV==g_type) ) { // Si compatible
          if (_etatBOM == S_JEUENCOURS) {
            g_dep_time = millis();
            _etatBOM = S_TRANSFERT;
            g_adrIpPav = (car & ADRIP) >> 3;
            sendMessageToServer(E_DEB_TRANSFERT);
            Serial.println("Debut de transfert...");
          } // _etatBOM
          g_dep_dureeEntreDeuxBytes = millis();
        } // coul
      } // if car
    } // if  _etatBOM
  } // if available

  // ==========================================
  // GESTION DU TRANSFERT (5 SECONDES)
  // ==========================================
  if (_etatBOM == S_TRANSFERT) {
    g_deltaT = millis() - g_dep_time;
    if (g_deltaT > DUREE_TRANSFERT) {   
      g_message = "\n-------------\nTemps atteint fin transfert\n-----------------\n";
      sendMessageToServer(E_DIVERS);
      _etatBOM = S_FIN_TRANSFERT;     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      g_dep_pausePourReprise = millis();
      afficheur.setProgression(g_dsCouleur, g_luminosite, afficheur.progression()+NBLEDS_PAR_TRANSFERT, g_batterie_faible);
      sendMessageToServer(E_FIN_TRANSFERT);
    } // if DUREEE_TRANSFERT  

    g_deltaT = millis() - g_dep_dureeEntreDeuxBytes;
    Serial.printf("Début entre 2 octets : %d\n", g_dep_dureeEntreDeuxBytes);
    if (g_deltaT > DUREE_ENTRE_2BYTES) {  
      _etatBOM = S_FIN_TRANSFERT;
      g_message = "\n-------------\nTemps dépassé entre deux\n-----------------\n";
      sendMessageToServer(E_DIVERS);
      Serial.printf("deltaT = %d\n", g_deltaT);
      g_dep_pausePourReprise = millis();
      sendMessageToServer(E_ANNULATION_TRANSFERT);
    } // if DUREE_ENTRE_2BYTES
  } // if _etatBOM

  if (_etatBOM == S_FIN_TRANSFERT) {
    if ( (millis()-g_dep_pausePourReprise) > 1000) {
      g_message = "\n-------------\njeuencours\n-----------------\n";
      sendMessageToServer(E_DIVERS);
      _etatBOM = S_JEUENCOURS;
      Serial.println("JEU EN COURS");
    } // if millis
  } // if fin_transfert


  // ==========================================
  // AFFICHAGE LED (NeoPixel)
  // ==========================================
  switch(_etatBOM) {
    case S_INIT: 
      afficheur.off();
      break;
    case S_JEUENCOURS: 
      afficheur.setProgression(g_dsCouleur, g_luminosite, afficheur.progression(), g_batterie_faible); 
      break;
    case S_TRANSFERT: 
      if (g_c==0) g_c=1; else g_c=0;
      afficheur.clignote(g_dsCouleur, 0, g_c, false);
      break;
    default: 
      break;
  } // sw
} // loop