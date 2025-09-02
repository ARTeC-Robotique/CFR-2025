#ifndef MOUVEMENT_H
#define MOUVEMENT_H

// Fonction qui gère la rotation du robot autour de lui-même
// Retourne true quand la rotation est terminée
bool Rotation(float Zcible, float Te, char mode);

// Fonction qui gère la translation du robot vers un point après rotation
// Retourne true quand la translation est terminée
bool Translation(float D, float A, float Te, char mode);

bool GoTo(float Xcible, float Ycible, float Zfinal, float Te, char mode);

#endif