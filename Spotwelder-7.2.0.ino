bool test_without_transformer = 0; // 0 for production firmware, 1 for test without transformer

/*
Attention: verify code after upload: ON / OFF

Installation libraries:
Extract libraries\libraries_Spotwelder.zip and copy all to
C:\Program Files (x86)\Arduino\libraries

Todo
Change maximum values, just as selectTFTorientation()

Note: use the latest Arduino software and install the libraries.

Arduino spot welder controller
http://www.avdweb.nl/arduino/hardware-interfacing/spot-welder-controller.html

Copyright (C) 2012  Albert van Dalen
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License at http://www.gnu.org/licenses .

TFT 240x320 library:
https://www.instructables.com/id/Arduino-TFT-display-and-font-library/
https://github.com/Bodmer/TFT_ILI9341 m

Version 5.2 1-02-2015
Version 6.0 2-01-2017 added menu with TFT display
Version 6.1 22-05-2017 added set display orientation
Version 6.2 26-8-2017 only supports PCB with LCD now, weldContinuously with sinusMax, displayDot improved
Version 7.0 2-10-2017 only for smd board and ILI9341 240x320 TFT display
Version 7.1
Version 7.1.1 24-4-2018 improvement commits by hlovdal
Version 7.1.2 27-8-2018 changed the library name to avoid conflicts with other libraries TFT_ILI9341_Bodmer.h
Version 7.2.0 27-8-2018 added UpDownValueTable, changed introtext, selectContinuously, blinkLed without delay

Program with FTDI programmer, Extra > Board > Arduino Uno

              <   20ms   >< >sinusMax_us
               _____       _____       _____
zeroCross   __|     |_____|     |_____|     |__
                              _____________
gate       __________________|             |____

T   = 1/(4*50Hz) = 5000us
T   = 1/(4*60Hz) = 4167us
Average time: sinusMax_us = (5000us + 4167us)/2 = 4583us, is not critical, use for 50 and 60Hz
*/

#include <Arduino.h>
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/
#include <SPI.h>
#include <TFT_ILI9341_Bodmer.h> // (changed the name) https://github.com/Bodmer/TFT_ILI9341 disadvantage: settings in library

#include <Albert.h> // https://github.com/avandalen/Albert-Arduino-library
#include <avdweb_Switch.h> // https://github.com/avandalen/avdweb_Switch
#include <EEPROMex.h>
#include "Definitions.h"
#include "Classes.h"
#include "Eeprom.h"
#include "Functions.h"

String version = "Version 7.2.0";

Switch weldButton(weldButtonPin);
Switch upButton(upButtonPin);
Switch downButton(downButtonPin);
Switch selectButton(selectButtonPin);
Switch footSwitch(footSwitchPin);

TFT_ILI9341 tft = TFT_ILI9341();  // pins are defined in User_Setup.h in library TFT_ILI9341_Bodmer
UpDownValueStep WeldItemNr = UpDownValueStep(0, 1, 0, 2); // value, step, minValue, maxValue

Menu menu;
Eeprom eeprom;

int preweldTimeValues[]={10, 20, 30, 40, 50, 60, 80, 100, 120, 150, 200, 250, 300, 400, 500, 600, 800, 1000};
MenuItem preweldTimeItem = MenuItem("Preweld, ms", UpDownValueTable(4, preweldTimeValues, arrayLenght(preweldTimeValues))); // valuesPtr, table, arrayLenght

int pauseTimeValues[]={10, 20, 30, 40, 50, 60, 80, 100, 120, 150, 200, 250, 300, 400, 500, 600, 800, 1000};
MenuItem pauseTimeItem = MenuItem("Pause, ms", UpDownValueTable(14, pauseTimeValues, arrayLenght(pauseTimeValues)));

int weldTimeValues[]={10, 20, 30, 40, 50, 60, 80, 100, 120, 150, 200, 250, 300, 400, 500, 600, 800, 1000};
MenuItem weldTimeItem = MenuItem("Weld time, ms", UpDownValueTable(11, weldTimeValues, arrayLenght(weldTimeValues)));

MenuItem menuItems[] = {preweldTimeItem, pauseTimeItem, weldTimeItem};
bool continuously;
unsigned orientation_addr, orientation=3;

void setup()
{ Serial.begin(9600);
  setpinModes();
  pollAll();
  pollAll(); // do after the blinkLed() delay
  eeprom.init();
  eeprom.read(); // set the menu with the stored EEPROM values
  printValuesToSerial();
  TFTinit();
  menu.displayStart();
}

void loop()
{ pollAll();
  menu.control();
  weldControl();
  blinkLed1(250);
  //test();
}
