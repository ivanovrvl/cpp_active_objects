#include <Arduino.h>
#include "ao_arduino.h"

void GPIOtone::setTone(int val) {
  if(tone <= 0)
    noTone(pin);
  else
    tone(pin, val);
}