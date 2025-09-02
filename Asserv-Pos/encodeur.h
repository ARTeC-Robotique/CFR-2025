#ifndef ENCODEUR_H
#define ENCODEUR_H

// Pour initialiser les PINs
void initEncodeurs();

// Pour obtenir les ticks gauche et droite tout le temps
long getTicksR();  // ticks droite
long getTicksL();  // ticks gauche

// Pour calculer la distance et l'angle parcourue par l'ensemble des deux encodeurs
void impEncodeurs();
float distEncodeurs();
float angleEncodeurs();

#endif