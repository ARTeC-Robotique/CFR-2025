#ifndef FSM_H
#define FSM_H

// Les différents états du robot
enum State {
  IDLE,
  ROTATE,
  TRANSLATE,
  ALIGN,
  WAIT,
  STOP
};

extern State currentState;

void updateFSM(float Te);
void setNextRotation(float angle);
void setNextTranslation(float distance);
void setWait(unsigned long duration);

#endif