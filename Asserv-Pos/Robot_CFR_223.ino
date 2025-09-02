#include "Parametres.h"
#include "Moteur.h"
#include "encodeur.h"
#include "Odometrie.h"
#include "Mouvement.h"
#include "Commande.h"

// Temps d’échantillonnage pour l’asservissement (en secondes)
const float Te = 0.1;
unsigned long previousMillis = 0;
unsigned long chrono;
unsigned long end;

// === SETUP ===
void setup() {
  Serial.begin(115200);
  initMoteurs();
  initEncodeurs();
  previousMillis = millis();
}

// === LOOP PRINCIPAL ===
void loop() {
  checkSerial();  // Lecture des commandes série

  static int phase = 0;  // Étape actuelle
  static bool enCours = false;

  if ((millis() - previousMillis) >= Te * 1000) {
    previousMillis = millis();
 
    updateOdometrie();  // Mise à jour X, Y, Z

    // === DEBUG (position absolue) ===
    /*Serial.print("X: ");
    Serial.print(getX());
    Serial.print(" ; Y: ");
    Serial.print(getY());
    Serial.print(" ; Z: ");
    Serial.println(getZ() * 180 / PI);*/
    
    // === Debug encodeurs ===
    /*Serial.print("TicksR: ");
    Serial.print(getTicksR());
    Serial.print(" ; TicksL: ");
    Serial.println(getTicksL());*/

    // === Fonctions Goto ===
    if (cmd == 'G'){
      if (GoTo(Xc, Yc, Zc, Te, 's')){
        Serial.println("Déplacement finie");
        cmd = ' ';
      }
    }

    // === Commandes rectangulaires ===
    /*switch (phase) {
      case 0:
        enCours = GoTo(0.0, 0.0, 0.0, Te, 's');  // point de départ
        if (enCours) phase++;
        break;

      case 1:
        enCours = GoTo(150.0, 0.0, 0.0, Te, 's');  // avancer de 150 cm
        if (enCours) phase++;
        break;

      case 2:
        enCours = GoTo(150.0, 50.0, PI/2, Te, 's');  // tourner vers le haut
        if (enCours) phase++;
        break;

      case 3:
        enCours = GoTo(0.0, 50.0, PI, Te, 's');  // revenir à gauche
        if (enCours) phase++;
        break;

      case 4:
        enCours = GoTo(0.0, 0.0, -PI/2, Te, 's');  // redescendre
        if (enCours) {
          Serial.println("Rectangle terminé !");
          phase++;  // boucle terminée
        }
        break;

      case 5:
        // Fin de la séquence : rester en IDLE
        break;
    }*/
    
    // === Commandes unitaire ===
    if (cmd == 'R'){
      if (Rotation(Zc, Te, 's')){
        Serial.println("Rotation finie");
        cmd = ' ';
      }
    }

    if (cmd == 'T'){
      float Xerr = Xc - getX();
      float Yerr = Yc - getY();
      Terr = sqrt(Xerr * Xerr + Yerr * Yerr);
      Zerr = atan2(Yerr, Xerr) - getZ();
      if (Zerr > PI) Zerr -= 2 * PI;
      if (Zerr < -PI) Zerr += 2 * PI;
      
      if (Translation(Terr, Zerr, Te, 's')){
        Serial.println("translation finie");
        cmd = ' ';
      }
    }
  }
}
