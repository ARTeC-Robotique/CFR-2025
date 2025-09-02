#pragma once
#include <Arduino.h>

// === Detection d'obstacle ===
extern bool pauseObstacle;

// === Constantes globales (connues à la compilation) ===
constexpr float seuilZerr = 10.0 * PI / 180.0;  // 10°
constexpr float seuilTerr = 2.0;              // 5 cm

// === Variables de trapèze (modifiable dynamiquement) ===
extern float maxAcc_s;
extern float maxSpeed_s;

extern float maxAcc_f;
extern float maxSpeed_f;

extern float minSpeed;

// === Cibles de position et orientation ===
extern float Xc;
extern float Yc;
extern float Zc;

// === États du robot ===
extern bool Arret;
extern bool Marche;
extern bool rotation;
extern bool translation;

// === Commande série actuelle ===
extern char cmd;

// === Erreurs (calculées à chaque boucle) ===
extern float Xerr;
extern float Yerr;
extern float Zerr;
extern float Terr;