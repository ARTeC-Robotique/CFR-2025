#ifndef MOTEUR_H
#define MOTEUR_H

// Initialisation des moteurs
void initMoteurs();

// Commande individuelle d'un moteur (1 = gauche, 2 = droite)
void setMotor(int mot, float vitesse);

// Commande simultanée des deux moteurs
void setMoteurs(float vitesseGauche, float vitesseDroite);

// Arrêt complet
void stopMoteurs();

#endif