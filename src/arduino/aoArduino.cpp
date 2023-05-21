#include <Arduino.h>
#include "aoArduino.h"

void GPIOtone::setTone(int val) {
  if(tone <= 0)
    noTone(pin);
  else
    tone(pin, val);
}