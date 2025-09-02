/*#include "FSM.h"
#include "Controle.h"     // pour PID, trapèze, etc.
#include "Mouvement.h"    // pour Rotation(), Translation()
#include <Arduino.h>

State currentState = IDLE;

// Variables internes
static float targetAngle = 0.0;
static float targetDistance = 0.0;
static unsigned long waitStart = 0;
static unsigned long waitDuration = 0;

void setNextRotation(float angle) {
  targetAngle = angle;
  currentState = ROTATE;
}

void setNextTranslation(float distance) {
  targetDistance = distance;
  currentState = TRANSLATE;
}

void setWait(unsigned long duration) {
  waitDuration = duration;
  waitStart = millis();
  currentState = WAIT;
}

void updateFSM(float Te) {
  switch (currentState) {
    case IDLE:
      // Rien à faire
      break;

    case ROTATE:
      if (Rotation(Te, targetAngle)) {
        currentState = IDLE;
        Serial.println("Rotation terminée.");
      }
      break;

    case TRANSLATE:
      if (Translation(Te, targetDistance)) {
        currentState = IDLE;
        Serial.println("Translation terminée.");
      }
      break;

    case ALIGN:
      if (AlignementSurMur(Te)) {
        currentState = IDLE;
        Serial.println("Alignement terminé.");
      }
      break;

    case WAIT:
      if (millis() - waitStart >= waitDuration) {
        currentState = IDLE;
        Serial.println("Fin du WAIT.");
      }
      break;

    case STOP:
      stopMotors();
      // On reste dans STOP jusqu’à réinitialisation manuelle
      break;
  }
}*/