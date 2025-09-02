#include <Arduino.h>
#include "Odometrie.h"
#include "Parametres.h"
#include "encodeur.h" // utilise dR et dL

// === Position absolue du robot (cm et rad) ===
static float X = 0.0;
static float Y = 0.0;
static float Z = 0.0;

// === Définir manuellement la position du robot ===
// x et y en cm, z en rad
void setPosition(float x, float y, float z) {
  X = x;
  Y = y;
  Z = z;
}

// === Mise à jour de la position par odométrie ===
// Utilise les incréments dR et dL (en ticks) pour calculer dX, dY, dZ
void updateOdometrie() {
  impEncodeurs(); // met à jour dR et dL (globaux de encodeur.cpp)
  
  float dD = distEncodeurs();   // distance parcourue (cm)
  float dZ = angleEncodeurs();  // angle parcouru (rad)
  
  // Mise à jour de la position absolue
  X += dD * cos(Z + dZ);
  Y += dD * sin(Z + dZ);
  Z += dZ;

  // Normaliser Z entre -π et π pour éviter les débordements
  if (Z > PI) Z -= 2 * PI;
  if (Z < -PI) Z += 2 * PI;
}

// === Accesseurs pour les coordonnées ===
float getX() { return X; }
float getY() { return Y; }
float getZ() { return Z; }