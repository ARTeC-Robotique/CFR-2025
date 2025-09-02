#include "Parametres.h"

// === Detection d'obstacle ===
bool pauseObstacle = false;

// === Trapèze (valeurs par défaut) ===
extern float maxAcc_s = 7.0;
extern float maxSpeed_s = 80.0;

extern float maxAcc_f = 14.0;
extern float maxSpeed_f = 150.0;

float minSpeed = 30.0;

// === Cibles à atteindre ===
float Xc = 0;
float Yc = 0;
float Zc = 0;

// === États du robot ===
bool Arret = false;
bool Marche = false;
bool rotation = false;
bool translation = false;

// === Commande reçue (par défaut vide) ===
char cmd = ' ';

// === Erreurs actuelles ===
float Xerr = 0;
float Yerr = 0;
float Zerr = 0;
float Terr = 0;