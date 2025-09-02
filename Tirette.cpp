#include <Arduino.h>
#include "Tirette.h"

#define PIN_fc 6

void initTirette(){
  pinMode(PIN_fc, INPUT_PULLUP);
}

bool activation(){
  int etat = digitalRead(PIN_fc);

  if(etat == LOW){
    return true;
  } else {
    return false;
  }
}