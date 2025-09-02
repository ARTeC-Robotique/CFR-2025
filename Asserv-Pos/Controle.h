#ifndef CONTROLE_H
#define CONTROLE_H

// Calcul d'une sortie PID
float PID(float erreur, float& erreurPrec, float& erreurInt, float Kp, float Ki, float Kd, float Te);

// Rampe trapézoïdale sur une consigne
float Trapeze(float consigne, float& consignePrec, float accMax, float maxSpeed, float minSpeed, float Te);

#endif