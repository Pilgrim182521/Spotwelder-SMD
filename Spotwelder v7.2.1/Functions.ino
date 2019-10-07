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

void weldControl()
{ if(continuously) weldContinuously();
  else if(weldButton.pushed() || footSwitch.pushed()) weldCyclus(menuItems[2].upDownValueTable);
}

void weldCyclus(int weldTime_ms)
{ pulseWeld(menuItems[0].upDownValueTable);
  delay(menuItems[1].upDownValueTable);
  pulseWeld(weldTime_ms);
}

void weldContinuously()
{ if(weldButton.pushed()) sinusMax(); // do only once
  weld(weldButton.on());
}

void pulseWeld(int ms)
{ sinusMax();
  if(ms) weld(1); // avoid short displayDot flash
  delay(ms); // gate signal is not pulsed but continuous because the current zero crossing moment is unknown
  weld(0);
  Serial << ms << endl;
}

void weld(bool b)
{ menu.displayDot(b);
  digitalWrite(weldPin, b);
  if(b) Serial << "w ";
}

void sinusMax()
{ Serial << "sinusMax ";
  if(test_without_transformer) return;
  while(digitalRead(zeroCrossPin));
  while(!digitalRead(zeroCrossPin));
  delayMicroseconds(sinusMax_us); // prevent high inrush current, turn-on at the sinus max
}

void setpinModes()
{ pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(weldButtonPin, INPUT_PULLUP);
  pinMode(weldPin, OUTPUT);
  pinMode(ledPin, OUTPUT); // is also done in blinkLed
  pinMode(zeroCrossPin, INPUT);
  pinMode(footSwitchPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); 
}

void pollAll()
{ weldButton.poll();
  upButton.poll();
  downButton.poll();
  selectButton.poll();
  footSwitch.poll();
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

void blinkLed1(unsigned long onTime_ms)
{ static bool ledOn = 0;
  static unsigned long last_ms = millis();
  digitalWrite(ledPin, ledOn);    
  if(millis() > last_ms + onTime_ms)
  { ledOn = ! ledOn;
    last_ms = millis();
  }
}




