#ifndef COMMANDE_H
#define COMMANDE_H

// Fonction principale de traitement des commandes série
void decryptIncom();

// Lecture du buffer série et appel à decryptIncom si commande complète
void checkSerial();

void logRotation(float Zc, float Te, float pwm, float pwmFiltre); 

#endif