#include <Arduino.h>
#include <analogReadAsync.h>
#include "ao_analogread.h"

bool ExclusiveAnalogReader::read(byte pin, Listener& listener, int& value) {
  if(lock(listener)) {
    if(start()) {
      analogReadAsync(pin);
      setBusy(true);
    } else {
      if(!isBusy()) {
        value = getAnalogReadValue();
        unlock();
        return true;
      }
    }
  }
  return false;
}

void ExclusiveAnalogReader::loop() {
  if(isBusy() && getAnalogReadComplete()) {
    setBusy(false);
  }
}

ExclusiveAnalogReader AnalogReader;
