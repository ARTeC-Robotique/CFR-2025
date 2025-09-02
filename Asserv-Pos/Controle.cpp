#include <Arduino.h>
#include "Controle.h"
#include "Parametres.h"

// === Contrôleur PID générique ===
float PID(float erreur, float& erreurPrec, float& erreurInt, float Kp, float Ki, float Kd, float Te) {
  erreurInt += erreur * Te;
  float deriv = (erreur - erreurPrec) / Te;
  float out = Kp * erreur + Ki * erreurInt + Kd * deriv;
  erreurPrec = erreur;
  return out;
}

// === Profil en trapèze pour variation progressive ===
float Trapeze(float consigne, float& consignePrec, float accMax, float maxSpeed, float minSpeed, float Te) {
  float diff = consigne - consignePrec;
  float direction = (diff > 0) ? 1 : -1;

  if (abs(diff) > accMax){
    consigne = consignePrec + direction * accMax * Te;
  }
  if (consigne > maxSpeed) consigne = maxSpeed;
  if (consigne < -maxSpeed) consigne = -maxSpeed;
  
  consignePrec = consigne;
  return consigne;
}