#ifndef _Classes_H_
#define _Classes_H_

#include <Arduino.h>

class UpDownValueStep
{ public:
    UpDownValueStep(int value, int step, int minValue, int maxValue);
    int up();
    int down();
    operator int();

    int value, step, minValue, maxValue;
};

// ---------------------------------------------------------------------------

class UpDownValueTable
{ public:
    UpDownValueTable(unsigned valuesPtr, int *values, unsigned size, int weldTimeWarning=-1);
    int up();
    int down();
    operator int();

    int *values, weldTimeWarning;
    unsigned valuesPtr, size;
};

// ---------------------------------------------------------------------------

class MenuItem
{ public:
    MenuItem(String name, UpDownValueTable upDownValueTable);

    String name;
    UpDownValueTable upDownValueTable;
};

// ---------------------------------------------------------------------------

class Menu
{ public:
    void displayStart();
    void control();
    void displayDot(bool on);

  private:
    void displayName(int itemNr, String &name);
    void displayValue(int itemNr, int value, bool focus, bool edit);
    void displayContinuously();
    void introtext();

    bool editValueMode=0;
    const int textLeft = 20;
};

#endif
