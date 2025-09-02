#include <Arduino.h>
#include "encodeur.h"
#include "Parametres.h"

// === Pins des encodeurs ===
#define interruptPinRA 2
#define interruptPinLA 3
#define interruptPinRB 4
#define interruptPinLB 5

// === Coefficients de conversion ===
// Ticks par cm (en ligne droite)
#define TICKS_CM_R 127.277
#define TICKS_CM_L 126.238

// Ticks par radian pour rotation horaire et trigonométrique
#define TICKS_RAD_HR 2225.145
#define TICKS_RAD_HL 2331.779
#define TICKS_RAD_TR 2373.319
#define TICKS_RAD_TL 2340.214

// === Compteurs d'impulsions ===
volatile long countR = 0;
volatile long countL = 0;
long prevCountR = 0;
long prevCountL = 0;

// === Différences entre deux lectures ===
float dR = 0;
float dL = 0;

// === Interruptions des encodeurs ===
// Sens configurable en inversant la logique
void interruptR() {
  countR += (digitalRead(interruptPinRA) == digitalRead(interruptPinRB)) ? 1 : -1;
}

void interruptL() {
  countL += (digitalRead(interruptPinLA) == digitalRead(interruptPinLB)) ? -1 : 1;
}

// === Initialisation des encodeurs ===
void initEncodeurs() {
  pinMode(interruptPinRA, INPUT_PULLUP);
  pinMode(interruptPinRB, INPUT_PULLUP);
  pinMode(interruptPinLA, INPUT_PULLUP);
  pinMode(interruptPinLB, INPUT_PULLUP);

  // Attache les interruptions sur RA et LA
  attachInterrupt(digitalPinToInterrupt(interruptPinRA), interruptR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPinLA), interruptL, CHANGE);
}

// === Calcul des incréments depuis la dernière lecture ===
void impEncodeurs() {
  noInterrupts(); // section critique
  long currentR = countR;
  long currentL = countL;
  interrupts();

  dR = currentR - prevCountR;
  dL = currentL - prevCountL;

  prevCountR = currentR;
  prevCountL = currentL;
}

// === Distance parcourue en cm (moyenne des 2 roues) ===
float distEncodeurs() {
  return (dR / TICKS_CM_R + dL / TICKS_CM_L) * 0.5;
}

// === Angle parcouru en rad (différence des 2 roues) ===
float angleEncodeurs() {
  float dRrad = (dR > 0) ? dR / TICKS_RAD_HR : dR / TICKS_RAD_TR;
  float dLrad = (dL > 0) ? dL / TICKS_RAD_HL : dL / TICKS_RAD_TL;
  return (dRrad - dLrad) * 0.5;
}

// === Accès direct aux ticks (debug) ===
long getTicksR() {
  noInterrupts();
  long temp = countR;
  interrupts();
  return temp;
}

long getTicksL() {
  noInterrupts();
  long temp = countL;
  interrupts();
  return temp;
}