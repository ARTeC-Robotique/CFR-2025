#ifndef ODOMETRIE_H
#define ODOMETRIE_H

//Pour donner la position de départ
void setPosition(float x, float y, float z);

// Pour calculer et mettre à jour la position
void updateOdometrie();

// Pour obtenir les positions du robot (pour les print) 
float getX();
float getY();
float getZ(); // orientation en radians

#endif