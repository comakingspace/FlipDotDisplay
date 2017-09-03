/*
  FlipDriver.h – Library for Driving Flip Dot Displays
  Created by Patrick Kübler, sometime in 2017
  Release under Creative Commons 4.0
*/

#ifndef FlipDriver_h
#define FlipDriver_h

#include Arduino.h

class FlipDotDriver
{
public:
    void FlipDotDisplay(
        int nDotsX,
        int nDotsY,
        int X_SHIFT_DATA,
        int X_SHIFT_CLOCK,
        int X_SHIFT_LATCH,
        int Y_SHIFT_DATA,
        int Y_SHIFT_CLOCK,
        int Y_SHIFT_LATCH,
        int X_DATA,
        int Y_DATA
    );
    void flipOut(byte data[]);
    byte storeData[];
    byte dispData[];

private:
    int _nDotsX;
    int _nDotsY;
    int _X_SHIFT_DATA;
    int _X_SHIFT_CLOCK;
    int _X_SHIFT_LATCH;
    int _Y_SHIFT_DATA;
    int _Y_SHIFT_CLOCK;
    int _Y_SHIFT_LATCH;
    int _X_DATA;
    int _Y_DATA;

    int _pulseLength;

    void _shiftSetX(int pos);
    void _shiftSetY(int pos);
    void _latchXY();
    void _flipDot(int x, int y, bool state);
};


#endif
