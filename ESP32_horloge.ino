// ESP32_horloge
// ESP32_horloge.ino
// (c) B. Froger 2020

#include <arduino.h>
#include <WiFi.h>

#include "afficheur.hpp"
#include "bouton.hpp"
#include "config.hpp"
#include "chronometre.hpp"
#include "wifiTools.hpp"
#include "reveil.hpp"
#include "ntp.hpp"
#include "heure.hpp"

// set the LCD parameters

// time varaibles
unsigned long nbMillis;
int displayMode, configHeureMode;

#define NBDISPLAYMODE   5
char tblModeDisplay[NBDISPLAYMODE][15] = {"heure", "alarme", "chronometre", "Wifi", "NTP"};

//=========================================
//
//          setup
//
//=========================================
void setup() {
    delay(1000);
    // initialize serial communication
    Serial.begin(115200);
    int timeoutInitSerial = 100;
    while (timeoutInitSerial-- > 0)
    {
        if (Serial)
        break;
        delay(10);
    }
    delay(1000);
    if (timeoutInitSerial != 0)
    {
        Serial.println("Serial initialized");
    } else
    {
        Serial.println("Serial not initialized");
        exit(0);
    }

    initAfficheur();
    delay(500);
    afficheTexte(lcd,0,0,"afficheur OK");
    Serial.println("afficheur OK");
    delay(1000);
    lcd.clear();

    nbMillis = millis();
    configInit();

    displayMode = 0;

    resetChrono();
    resetHeure();

    Serial.println("try wifi ...");
    afficheTexte(lcd,0,0,"try wifi ...");
    initWifi();
    afficheTexte(lcd,0,1,getSsid());
    delay(1000);
    initNtp();
    refreshNtpNow();
    initReveil();
    lcd.clear();

    Serial.println("Fin de setup");
}

//=========================================
//
//          loop
//
//=========================================
void loop() {

   updateHeure();

    if ((millis() - nbMillis) >= 100){
        nbMillis = millis();
        switch (displayMode){
            case 0 : // mode affichage de l'heure
                if (!configMode){
                    // affichege de l'ecran d'heure
                    printHeure();
                } else {
                    // affichage des ecrans de config de l'heure
                    configHeure(cptConfigMode);
                }
                break;
            case 1 : // mode affichage heure infos aloarme
                if (!configMode){
                    // affichege de l'ecran d'alarme
                    printAlarme();
                } else {
                    // affichage des ecrans de config de l'alarme
                    configReveil(cptConfigMode);
                }
                break;
            case 2 : // mode affichage chronometre
                if (!configMode){
                    printChronometre();
                } else {
                    chronometre(cptConfigMode);
                }
                break;
            case 3 : // mode wifi
                if (!configMode){
                    printInfoWifi();
                } else {
                    lcd.clear();
                    afficheTexte(lcd,0,0,"rescan Wifi");
                    initWifi();
                    refreshNtpNow();
                    configMode = false;
                }
                break;
            case 4 : // mode NTP
                if (!configMode){
                    printNtp();
                } else {
                    refreshNtpNow();
                    configMode = false;
                }
                break;
        }
    }
    if (isShortClic()){
        if (!configMode) {
            // mode affichage incremente le menu principal
            displayMode++;
        } else {
            // mode config on incremente le sous menu
            cptConfigMode++;
        }
        displayMode = displayMode % NBDISPLAYMODE;
        Serial.print("display mode = ");
        Serial.println(displayMode);
        Serial.print("config mode = ");
        Serial.println(cptConfigMode);
        lcd.clear();
    }
    if (!configMode){
        if (isLongClic()){
            configMode = true;
            cptConfigMode = 0;
        }
    }

    readBouton();
    refreshNtp();
    testReveil();
    delay (1);
}
