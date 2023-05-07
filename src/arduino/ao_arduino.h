#ifndef ao_arduino_h
#define ao_arduino_h

byte BC_START_DELAY = 1;
byte BC_EVENT_PERIOD = 3;

class Button {
public:
  Button(byte pin) {
    this->pin = pin;
  }
  byte pin;
  byte pressed; // pressing time in periods
};

class ButtonControl : public AObject {
  Button* buttons;  
  int count;
  ao_time next;	
  byte checkPeriod;
public:
  Signaler buttonEvent;

  ButtonControl(byte count, Button* buttons, byte checkPeriod, boolean pullUp=true) {
    this->buttons = buttons;
    this->count = count;
    this->checkPeriod = checkPeriod;
    this->init(pullUp);
  }

  void init(boolean pullUp) {
    for(int i=0; i<count; i++) {
      if(pullUp)
        pinMode(buttons[i].pin, INPUT_PULLUP);
      else
        pinMode(buttons[i].pin, INPUT);
    } 
  }

  bool isPressed(byte pin) {
    return digitalRead(pin) == LOW;
  }

	void process() {
    if(reached(next)) {     
      addDelay(next, checkPeriod);
      bool event = false;
      bool event2 = false;
      Button* btn = buttons;
      for(int i=0; i<count; i++) {        
        if(isPressed(btn->pin)) { 
          switch(btn->pressed) {
            case 0xFF:
              break;
            default:
              if(btn->pressed >= BC_START_DELAY) {
                if(btn->pressed == BC_START_DELAY || !(btn->pressed % BC_EVENT_PERIOD)) {
                  event = true;
                }
              }      
              btn->pressed++;
          }
        } else {
          // кнопка не нажата
          switch(btn->pressed) {
            case 0x00:
              break;
            default:
              btn->pressed = 0;
              event = true;
          }
        }
        if(event) buttonEvent.signalAll();
        btn++;
      }
    }
	}

};

class LEDSignalerTask {
public:
  byte type; // 0 - nothing
  byte count;
  unsigned long data;

  void clear() {
    type = 0;
    count = 0;
    data = 0;
  }

};

typedef bool (*GetTaskCallback)(byte index, LEDSignalerTask& task, Listener& listener);

typedef void (*SetPinSignal)(byte pin, bool on);

class PinSignaler : public AObject {
private:
  byte pin;
  SetPinSignal setPinSignal;
  byte state;
  ao_time next;
  byte index;
  LEDSignalerTask task;
  AOListener listener;
  GetTaskCallback taskCallback;  
public:

  PinSignaler(byte pin, SetPinSignal setPinSignal) : listener(this) {
    this->pin = pin;
    this->setPinSignal = setPinSignal;
    //pinMode(pin, OUTPUT);
  }

  void setOn(bool on) {
    setPinSignal(pin, on);
/*    if(on)
      digitalWrite(pin, LOW);
    else
      digitalWrite(pin, HIGH);*/
  }

  void restart(GetTaskCallback taskCallback, unsigned long pause) {
    this->taskCallback = taskCallback;
    state = 0;
    setOn(false);
    delay(next, pause);
  }

  void process() {
    switch(state) {
      case 0: // off - pause
        setOn(false);
        if(reached(next)) {          
          task.clear();
          index = 0;
          state = 10;
        } else
          break;
      case 10: // getting task
        if(taskCallback && taskCallback(index, task, listener)) {
          switch(task.type) {
            case 0: // restart with pause
              delay(next, task.data);
              break;
            case 1: // pause with LED on or off
              setOn(task.count);
              delay(next, task.data);              
              state = 20;
              signal();
              break;
            case 2: // send bits
              state = 30;
              signal();
              break;     
            default:
              index++;
              signal();
          }
        }
        break;
      case 20: // wait pause
        if(reached(next)) {
          index++;
          state = 10;          
          signal();
        }
        break;
      case 30: // start next bit
        if(!task.count) {     
          index++;
          state = 10;
          signal();
        } else {
          setOn(true);
          task.count--;
          if(task.data & (1 << task.count)) {
            delay(next, 500);
          } else {
            delay(next, 250);
          }          
          state = 40;
        }
        break;
      case 40: // wait end bit
        if(reached(next)) {
          setOn(false);
          delay(next, 250);
          state = 50;
        }
        break;
      case 50: // bit pause
        if(reached(next)) {
          state = 30;
          signal();
        }
        break;
    }
  }

};

#endif

