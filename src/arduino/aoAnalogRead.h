#ifndef aoAnalogRead_h
#define aoAnalogRead_h

#include "aobject.h"
#include <Arduino.h>

class ExclusiveAnalogReader : private ExclusiveResource {
private:
  using  ExclusiveResource::setBusy; 
  using ExclusiveResource::start;
  using ExclusiveResource::lock;
  using  ExclusiveResource::isBusy;
public: 

  bool read(byte pin, Listener& listener, int& value);

  void loop();
  
};

extern ExclusiveAnalogReader AnalogReader;

#endif

