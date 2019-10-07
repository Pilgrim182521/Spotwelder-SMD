bool selectTFTorientation() // because LCD orientation is unknown
{ if(upButton.on() && downButton.on()) // change between 1<->3 when up and down are pressed during power up
  { if(orientation==1) orientation=3; // orientation can only be either 1 or 3
    else orientation=1; // if 0, 2, 3...
    eeprom.write();
    TFTinit();
    return 1;
  }
  return 0;
}

bool selectContinuously()
{ if(downButton.on() && selectButton.on()) 
  { continuously = 1;  
    return 1;
  }
  return 0;
}

void weldControl() // runs in a loop
{ if(continuously) weldContinuously();
  else if(weldButton.pushed() || footSwitch.pushed()) weldCyclus();
}

void weldCyclus()
{ pollAll(); pollAll(); // reset all buttons because polling was interrupted during delay functions
  weldPulse(menuItems[0].upDownValueTable);
  pollAll(); pollAll();
  delay(menuItems[1].upDownValueTable);
  pollAll(); pollAll();
  weldPulse(menuItems[2].upDownValueTable);
  pollAll(); pollAll();
}

void weldContinuously() // runs in a loop
{ static bool zeroCrossDetected=0; // detected only once at weldButton.pushed()
  if(weldButton.pushed()) zeroCrossDetected = waitForSinusmax(); // use pushed() because do only once 
  weld(weldButton.on() && zeroCrossDetected);
}

void weldPulse(int ms)
{ bool zeroCrossDetected = waitForSinusmax();
  if(ms) weld(zeroCrossDetected);// do not trigger for 0 ms
  delay(ms); // weldPin is continuous high now
  weld(0);
  Serial << ms << endl;
}

void weld(bool b)
{ digitalWrite(weldPin, b);
  menu.displayDot(b);
  if(b) Serial << "weld ";
}

void setpinModes()
{ pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(weldButtonPin, INPUT_PULLUP);
  pinMode(weldPin, OUTPUT);
  pinMode(zeroCrossPin, INPUT);
  pinMode(footSwitchPin, INPUT_PULLUP);
}

void pollAll()
{ weldButton.poll();
  upButton.poll();
  downButton.poll();
  selectButton.poll();
  footSwitch.poll();
  zeroCrossCounter.poll();
}

void printValuesToSerial()
{  Serial << "\npreweld " << menuItems[0].upDownValueTable << "ms, pause " << menuItems[1].upDownValueTable << "ms, weldTime "
   << menuItems[2].upDownValueTable << "ms, continuously " << continuously << ", Orientation " << orientation << endl;
}

void drawColorTextLine(int line, int left, String str, uint16_t textColor, uint16_t backgroundColor) 
{ const int fontSize=4, fontHeight=26;
  const int vertOffset=3; // 240-9*26)/2
  tft.setTextColor(textColor, backgroundColor);
  tft.drawString(str.c_str(), left, fontHeight*line+vertOffset, fontSize);
}

void TFTinit()
{ tft.init();
  tft.setRotation(orientation);
}

bool waitForZeroCross() 
{ bool level, lastLevel;
  lastLevel = digitalRead(zeroCrossPin);
  unsigned long timeOut_ms = millis() + 15; // 1 period = 0.5/50Hz=10ms, take 15ms
  while(millis() < timeOut_ms) // works also without AC mains
  { level = digitalRead(zeroCrossPin);
    if(level != lastLevel) return 1; // transient detected
    lastLevel = level;  
  }  
  return 0; // no transient detected after 15ms
}

bool waitForSinusmax()
{ bool zeroCrossDetected = waitForZeroCross(); 
  if(test_without_transformer) zeroCrossDetected=1; 
  if(!zeroCrossDetected) 
  { menu.displayNoZeroCross(); 
    delay(2000); // display for 2s
    menu.displayItems();
  }   
  delayMicroseconds(sinusMax_us); // prevent high inrush current, turn-on at the sinus max
  return zeroCrossDetected;
}
