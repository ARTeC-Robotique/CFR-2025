#include <Arduino.h>
#include "Commande.h"
#include "Odometrie.h"
#include "Controle.h"
#include "Moteur.h"
#include "encodeur.h"
#include "Parametres.h"

// === Variables de communication ===
char streamChar[32];
int i = 0;
extern char cmd;

// === Cibles de position / orientation ===
extern float Xc, Yc, Zc;

// === États internes ===
extern bool Arret, Marche, rotation, translation;

// === Variables PID & trapèze externes ===
extern float errPrecRot, errIntRot, consignePrecRot;
extern float errPrecTr, errIntTr, consignePrecTr;
extern float errPrecCap, errIntCap;

// === Encodeurs ===
extern volatile long countR, countL;
extern long prevCountR, prevCountL;

// === Décryptage des commandes série ===
void decryptIncom() {
  bool neg = false;
  int index = 2; // position de lecture après la commande
  cmd = streamChar[0];

  // Réinitialisation systématique des états internes
  errPrecRot = errIntRot = consignePrecRot = 0;
  errPrecTr = errIntTr = consignePrecTr = 0;
  errPrecCap = errIntCap = 0;

  // Commande : arrêt immédiat
  if (cmd == ' ') {
    stopMoteurs();
    Arret = true;
    Marche = false;
    rotation = false;
    translation = false;

    Serial.println("Commande reçue : arrêt immédiat.");
    return;
  }

  // Commande : aller à un point Go x y z
  if (cmd == 'G') {
    Xc = 0; Yc = 0; Zc = 0;
    
    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Xc = Xc * 10 + (streamChar[index++] - '0');
    if (neg) Xc = -Xc;
    index++; neg = false;
    
    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Yc = Yc * 10 + (streamChar[index++] - '0');
    if (neg) Yc = -Yc;
    index++; neg = false;

    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Zc = Zc * 10 + (streamChar[index++] - '0');
    if (neg) Zc = -Zc;

    Zc *= PI / 180.0;  // Conversion en radians
    
    Arret = false;
    Marche = true;
    rotation = false;
    translation = false;

    Serial.print("Commande reçue : aller à (");
    Serial.print(Xc);
    Serial.print(" , ");
    Serial.print(Yc);
    Serial.print(" , ");
    Serial.print(Zc * 180.0 / PI);
    Serial.println(")");
  }

  // Commande : aller à un point G x y
  if (cmd == 'T') {
    Xc = 0; Yc = 0;
    
    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Xc = Xc * 10 + (streamChar[index++] - '0');
    if (neg) Xc = -Xc;
    index++; neg = false;
    
    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Yc = Yc * 10 + (streamChar[index++] - '0');
    if (neg) Yc = -Yc;
    

    Arret = false;
    Marche = true;
    rotation = false;
    translation = false;

    Serial.print("Commande reçue : aller à (");
    Serial.print(Xc);
    Serial.print(" , ");
    Serial.print(Yc);
    Serial.println(")");
  }

  // Commande : rotation absolue R angle_deg
  if (cmd == 'R') {
    Zc = 0;

    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) Zc = Zc * 10 + (streamChar[index++] - '0');
    if (neg) Zc = -Zc;
    Zc *= PI / 180.0; // conversion en radians

    Arret = false;
    Marche = true;
    rotation = false;
    translation = false;

    Serial.print("Commande reçue : rotation à ");
    Serial.print(Zc * 180 / PI);
    Serial.println(" degrés");
  }

  // Commande : définition position actuelle S x y z
  if (cmd == 'S') {
    float x = 0, y = 0, z = 0;

    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) x = x * 10 + (streamChar[index++] - '0');
    if (neg) x = -x;
    index++; neg = false;

    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) y = y * 10 + (streamChar[index++] - '0');
    if (neg) y = -y;
    index++; neg = false;

    if (streamChar[index] == '-') { neg = true; index++; }
    while (isDigit(streamChar[index])) z = z * 10 + (streamChar[index++] - '0');
    if (neg) z = -z;

    setPosition(x, y, z * PI / 180.0);
    Xc = x;
    Yc = y;
    Zc = z * PI / 180.0;

    countR = countL = 0;
    prevCountR = prevCountL = 0;

    Serial.print("Position réinitialisée à : (");
    Serial.print(x);
    Serial.print(" , ");
    Serial.print(y);
    Serial.print(" , ");
    Serial.print(z);
    Serial.println(" degrés)");
  }

  if (cmd == 'K') {
    extern float Kp_rot_s, Ki_rot_s, Kd_rot_s;
    extern float Kp_tr_s,  Ki_tr_s,  Kd_tr_s;
    extern float Kp_cap_s, Ki_cap_s, Kd_cap_s;

    float* pid_target[3] = {nullptr, nullptr, nullptr}; // pointeurs vers Kp, Ki, Kd
    const char* label = nullptr;

    if (streamChar[1] == 'R') {
      pid_target[0] = &Kp_rot_s; pid_target[1] = &Ki_rot_s; pid_target[2] = &Kd_rot_s;
      label = "Rotation";
    } else if (streamChar[1] == 'T') {
      pid_target[0] = &Kp_tr_s;  pid_target[1] = &Ki_tr_s;  pid_target[2] = &Kd_tr_s;
      label = "Translation";
    } else if (streamChar[1] == 'C') {
      pid_target[0] = &Kp_cap_s; pid_target[1] = &Ki_cap_s; pid_target[2] = &Kd_cap_s;
      label = "Correction de cap";
    } else {
      Serial.println("❌ Erreur : commande K inconnue (utilise KR / KT / KC)");
      return;
    }

    // Lecture des valeurs float (ex. KT1.2;0.01;0.1)
    int index = 2;
    
    while (streamChar[index] == ' ' || streamChar[index] == ';' || streamChar[index] == '\t') {
      index++;
    }

    String current = "";
    int i = 0;

    while (streamChar[index] != '\0' && i < 3) {
      char c = streamChar[index];
      if (c == ' ' || c == ';' || c == '\n' || c == '\r') {
        *pid_target[i] = current.toFloat();
        current = "";
        i++;
      } else {
        current += c;
      }
      index++;
    }

    // Dernière valeur
    if (i < 3 && current.length() > 0) {
      *pid_target[i] = current.toFloat();
    }

    Serial.print("✅ PID ");
    Serial.print(label);
    Serial.println(" mis à jour :");
    Serial.print("  Kp = "); Serial.println(*pid_target[0]);
    Serial.print("  Ki = "); Serial.println(*pid_target[1]);
    Serial.print("  Kd = "); Serial.println(*pid_target[2]);
  }
}


// === Lecture série non bloquante ===
void checkSerial() {
  while (Serial.available() > 0) {
    char incoming = Serial.read();
    if (incoming == '\n') {
      streamChar[i] = '\0';
      
      // Vérifie les messages spéciaux : obstacle / fin_obstacle
      if (strcmp(streamChar, "CMD:STOP") == 0) {
        pauseObstacle = true;
        stopMoteurs();
        Serial.println("Obstacle détecté. Pause du robot.");
        i = 0;
        return;
      }

      if (strcmp(streamChar, "CMD:GO") == 0) {
        pauseObstacle = false;
        Serial.println("Fin d’obstacle. Reprise du mouvement.");
        i = 0;
        return;
      }
      
      decryptIncom();
      i = 0;
    } else if (i < 31) {
      streamChar[i++] = incoming;
    }
  }
}