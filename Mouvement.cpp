#include <Arduino.h>
#include "Commande.h"
#include "Mouvement.h"
#include "Controle.h"
#include "Moteur.h"
#include "Odometrie.h"
#include "Parametres.h"

// === Paramètres PID rotation lente ===
float Kp_rot_s = 20.0; //0.65 //65
float Ki_rot_s = 1.0; //0.25
float Kd_rot_s = 2.0; //5.0

// === Paramètres PID rotation rapide ===
const float Kp_rot_f = 1.0;
const float Ki_rot_f = 0.0; 
const float Kd_rot_f = 0.0;

// === Paramètres PID translation lente ===
float Kp_tr_s = 1.0;
float Ki_tr_s = 0.0;
float Kd_tr_s = 0.0;

// === Paramètres PID translation rapide ===
const float Kp_tr_f = 1.0;
const float Ki_tr_f = 0.0;
const float Kd_tr_f = 0.0;

// === Correction de cap (pendant translation lente) ===
float Kp_cap_s = 1.0;
float Ki_cap_s = 0.0;
float Kd_cap_s = 0.06;

// === Correction de cap (pendant translation rapide) ===
const float Kp_cap_f = 2.5;
const float Ki_cap_f = 0.0;
const float Kd_cap_f = 0.0;

// === Variables internes Rotation ===
float errPrecRot = 0, errIntRot = 0, consignePrecRot = 0;

// === Variables internes Translation ===
float errPrecTr = 0, errIntTr = 0, consignePrecTr = 0;

// === Variables correction de cap (pendant translation) ===
float errPrecCap = 0, errIntCap = 0;

bool Rotation(float Zcible, float Te, char mode) {
  float pwm, maxAcc, maxSpeed;

  // --- Calcul de l'erreur --- 
  Zerr = Zcible - getZ();
  if (Zerr > PI) Zerr -= 2 * PI;
  if (Zerr < -PI) Zerr += 2 * PI;
  
  // --- PID sur Zerr selon la vitesse ---
  if (mode == 's'){
    pwm = PID(Zerr, errPrecRot, errIntRot, Kp_rot_s, Ki_rot_s, Kd_rot_s, Te);
    maxAcc = maxAcc_s;
    maxSpeed = maxSpeed_s;
  } else if (mode == 'f'){
    maxAcc = maxAcc_f;
    maxSpeed = maxSpeed_f;;
    pwm = PID(Zerr, errPrecRot, errIntRot, Kp_rot_f, Ki_rot_f, Kd_rot_f, Te);
  }

  // --- Trapèze sur la sortie ---
  float PWMR = Trapeze(pwm, consignePrecRot, maxAcc, maxSpeed, minSpeed, Te);

  // --- Vitesse minimale pour éviter de brouter ---
  if (abs(PWMR) < minSpeed) PWMR = minSpeed * (PWMR > 0 ? 1 : -1);

  float PWML = -PWMR;

  Serial.print("Z: ");
  Serial.print((Zcible - Zerr) * 180 / PI);
  Serial.print("| pwm: ");
  Serial.print(pwm);
  Serial.print("| PWMR: ");
  Serial.println(PWMR);
  
  // --- Appliquer la rotation si l’erreur est significative ---
  if (abs(Zerr) > seuilZerr) {
    setMotor(1, PWMR);
    setMotor(2, PWML);
    return false; // rotation encore en cours
  }

  // --- Rotation terminée ---
  stopMoteurs();
  errPrecRot = errIntRot = consignePrecRot = 0;
  Zerr = 0;
  rotation = true;
  return true;
}

bool Translation(float D, float A, float Te, char mode) {
  float pwm_dist, cap, maxAcc, maxSpeed;
  
  // --- Détecter si la cible est derrière : marche arrière automatique ---
  bool recul = false;
  if (cos(A) < 0) {
  recul = true;

  if (A < 0) A += PI;
  else A -= PI;
  }

  // --- PID sur la distance et le Cap ---
  if (mode == 's'){
    pwm_dist = PID(D, errPrecTr, errIntTr, Kp_tr_s, Ki_tr_s, Kd_tr_s, Te);
    cap = PID(A, errPrecCap, errIntCap, Kp_cap_s, Ki_cap_s, Kd_cap_s, Te);
    maxAcc = maxAcc_s;
    maxSpeed = maxSpeed_s;
  } else if (mode == 'f'){
    maxAcc = maxAcc_f;
    maxSpeed = maxSpeed_f;
    pwm_dist = PID(D, errPrecTr, errIntTr, Kp_tr_f, Ki_tr_f, Kd_tr_f, Te);
    cap = PID(A, errPrecCap, errIntCap, Kp_cap_f, Ki_cap_f, Kd_cap_f, Te);
  }

  // Calcul de vitesse max admissible pour pouvoir s'arrêter à temps
  float vmax_theorique = sqrt(2 * maxAcc * abs(D));  
  float vmax = min(maxSpeed, vmax_theorique);

  pwm_dist = constrain(pwm_dist, -vmax, vmax);
  
  pwm_dist = Trapeze(pwm_dist, consignePrecTr, maxAcc, maxSpeed, minSpeed, Te);

  float PWMR = recul ? -pwm_dist : pwm_dist;
  float PWML = PWMR;

  // --- Correction de cap en translation ---
  //cap = constrain(cap, -0.6, 0.6);
  PWMR += cap * abs(PWMR);
  PWML -= cap * abs(PWML);

  Serial.print("X= ");
  Serial.print(getX());
  Serial.print(", Y= ");
  Serial.print(getY());
  Serial.print(" | pwm: ");
  Serial.print(pwm_dist);
  Serial.print(cap * 180.0 / PI);
  Serial.print("| PWMR: ");
  Serial.println(PWMR);
  
  // --- Appliquer vitesse minimale ---
  if (abs(PWMR) < minSpeed) PWMR = minSpeed * (PWMR > 0 ? 1 : -1);
  if (abs(PWML) < minSpeed) PWML = minSpeed * (PWML > 0 ? 1 : -1);

  // --- Avancer si distance restante significative ---
  if (abs(D) > seuilTerr) {
    setMotor(1, PWMR);
    setMotor(2, PWML);
    return false;
  }

  // --- Translation terminée ---
  stopMoteurs();
  errPrecTr = errIntTr = consignePrecTr = 0;
  errPrecCap = errIntCap = 0;
  translation = true;
  return true;
}

bool GoTo(float Xcible, float Ycible, float Zfinal, float Te, char mode) {
  static enum { GO_ROT_INIT, GO_TRANSLATE, GO_ROT_FINAL, PAUSE_OBSTACLE } phase = GO_ROT_INIT;
  static float angleInit, distance;

  float Xerr = Xcible - getX();
  float Yerr = Ycible - getY();

  // Vérifie si on doit mettre en pause
  if (pauseObstacle) {
    stopMoteurs();
    phase = PAUSE_OBSTACLE;
    return false;
  }

  // Si on était en pause et que l’obstacle est levé, on reprend la phase précédente
  if (phase == PAUSE_OBSTACLE && !pauseObstacle) {
    Serial.println("Reprise après obstacle.");
    phase = GO_ROT_INIT;  // ou GO_TRANSLATE pour reprendre au milieu
  }

  switch (phase) {
    case GO_ROT_INIT:
      angleInit = atan2(Yerr, Xerr);
      if (Rotation(angleInit, Te, mode)) {
        phase = GO_TRANSLATE;
      }
      break;

    case GO_TRANSLATE:
      Terr = sqrt(Xerr * Xerr + Yerr * Yerr);
      Zerr = atan2(Yerr, Xerr) - getZ();
      if (Zerr > PI) Zerr -= 2 * PI;
      if (Zerr < -PI) Zerr += 2 * PI;

      if (Translation(Terr, Zerr, Te, mode)) {
        phase = GO_ROT_FINAL;
      }
      break;

    case GO_ROT_FINAL:
      if (Rotation(Zfinal, Te, mode)) {
        phase = GO_ROT_INIT;  // Réinitialise pour l’appel suivant
        return true;
      }
      break;

    case PAUSE_OBSTACLE:
      stopMoteurs();
      // Attendre fin d'obstacle pour relancer dans le bloc de contrôle au-dessus
      break;
  }

  return false;
}