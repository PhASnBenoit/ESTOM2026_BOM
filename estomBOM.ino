////////////////////////////////
//
//  PROGRAMME BOM (ESTOM 2026)
//  V5 - NeoPixel Adafruit & Protocole JSON v1.6
//  v2.0 Réseau OK, IR ok Choc à tester
//  v2.1 Version avec affichage debug 
//  v2.2 Modif champs JSON et INIT
//
////////////////////////////////
#define VER "2.2"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "definitions.h"
#include "ccapteurchocs.h"
#include "cdipswitch.h"
#include "cneopixel.h"

/////////////////////////////////////////
// VARIABLES ET INSTANCIATION GLOBALES
/////////////////////////////////////////
CDipSwitch ds;   
CCapteurChocs cc;
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
const int g_dureeTransfert = 5000;  // 5s
const int g_dureeEntreDeuxBytes = 1000; 

int g_dsCouleur=0;  
int g_type=0;
int g_adrIpPav;
int g_luminosite=1;
int g_dep_time=0;
int g_dep_dureeEntreDeuxBytes=0; 
int g_deltaT=0;

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
    if (!clientTcp.connect(tcpAddress, tcpPort)) {
      Serial.println("Échec de reconnexion pour envoi.");
      return;
    } // if connect
  } // if connect
  
  DynamicJsonDocument doc(256);
  // TOUTES LES VALEURS DOIVENT ETRE DES STRING (v1.6)
  doc["status"] = String(typeTrame); 

  switch(typeTrame) {
    case E_BONJOUR: 
//      doc["type"] = (g_type == 1) ? "BUS" : "BOM"; // si on veut distinguer les deux
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
  } // sw
  
  serializeJson(doc, clientTcp);
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
  
  // init couleur et bandeau LED (NeoPixel)
  afficheur.begin(); // require to intialize object
  afficheur.on(g_dsCouleur, g_luminosite);
  delay(500);
  afficheur.off();

  sendMessageToServer(E_BONJOUR);
  delay(500);
  Serial.println("Fin du SETUP");
} 

//////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  if (!clientTcp.connected()) {
    Serial.println("\nÉchec de connexion au serveur. RESTART !");
    ESP.restart();
  } // if tcp connected

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
        afficheur.setProgression(g_dsCouleur, g_luminosite, data["leds"].as<String>().toInt());
      break;
        
      case R_DEBUT: 
        Serial.println("Trame DEBUT PARTIE reçue");
        _etatBOM = S_JEUENCOURS;
        if (data.containsKey("luminosite")) {
          g_luminosite = data["luminosite"].as<String>().toInt();
        } // if lum
        cc.setup();
        afficheur.off();
        cc.setNbChocs();
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
      sendMessageToServer(E_CHOC);
    } // if cc
  } 

  // ==========================================
  // RÉCEPTION DU CARACTERE VIA INFRAROUGE (PAV)
  // ==========================================   
  if (_etatBOM != S_INIT) {
      if (Serial.available()>0) {
        char c = Serial.read();
        Serial.print("Je viens de lire : ");
        Serial.println(c, HEX);
        int coul = (c&COULEUR)>>1;
        int typ = c&TYPE;
          
        if ( (coul==g_dsCouleur) && (typ==g_type) ) {
          if (_etatBOM == S_TRANSFERT) {
            g_dep_time = millis();
            _etatBOM = S_TRANSFERT;   // plus nécessaire
            g_adrIpPav = (c & ADRIP) >> 3;
            sendMessageToServer(E_DEB_TRANSFERT);
            Serial.println("Debut de transfert...");
          } // _etatBOM
          g_dep_dureeEntreDeuxBytes = millis();
        } // coul
      } // if available
  } // _etatBOM 
  
  // ==========================================
  // GESTION DU TRANSFERT (5 SECONDES)
  // ==========================================
  if (_etatBOM == S_JEUENCOURS) {
    g_deltaT = millis() - g_dep_time;
    
    if (g_deltaT >= g_dureeTransfert) {   
      afficheur.setProgression(g_dsCouleur, g_luminosite, afficheur.progression()+2); // Avance de 2 LEDs par transfert
      sendMessageToServer(E_FIN_TRANSFERT);
      _etatBOM = S_JEUENCOURS;
      delay(500);  // attendre que le PAV reçoive l'ordre d'arrêter d'émettre
    } else { 
      g_deltaT = millis() - g_dep_dureeEntreDeuxBytes;
      if (g_deltaT >= g_dureeEntreDeuxBytes) {  
        sendMessageToServer(E_ANNULATION_TRANSFERT);
        _etatBOM = S_JEUENCOURS;
        delay(500); // attendre que le PAV reçoive l'ordre d'arrêter d'émettre
      } // if g_deltaT
    } // else
  } // if _etatBOM

  // ==========================================
  // AFFICHAGE LED (NeoPixel)
  // ==========================================
  switch(_etatBOM) {
    case S_INIT: 
      afficheur.off();
      break;
    case S_JEUENCOURS: 
    case S_TRANSFERT: 
      afficheur.setProgression(g_dsCouleur, g_luminosite, afficheur.progression()); // TODO pas obligé !
      break;
    default: 
      break;
  } // sw
} // loop