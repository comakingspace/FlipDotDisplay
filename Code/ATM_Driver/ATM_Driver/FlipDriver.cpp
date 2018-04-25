/*
  FlipDriver.h – Library for Driving Flip Dot Displays
  Created by Patrick Kübler, sometime in 2017
  Release under Creative Commons 4.0
*/

#include Arduino.h
#include FlipDriver.h

FlipDriver::FlipDotDisplay(
    int nDotsX,
    int nDotsY,
    int X_SHIFT_DATA,
    int X_SHIFT_CLOCK,
    int X_SHIFT_LATCH,
    int Y_SHIFT_DATA,
    int Y_SHIFT_CLOCK,
    int Y_SHIFT_LATCH,
    int X_DATA,
    int Y_DATA)
{
    pinMode(X_SHIFT_DATA, OUTPUT);
    pinMode(X_SHIFT_CLOCK, OUTPUT);
    pinMode(X_SHIFT_LATCH, OUTPUT);
    pinMode(Y_SHIFT_DATA, OUTPUT);
    pinMode(Y_SHIFT_CLOCK, OUTPUT);
    pinMode(Y_SHIFT_LATCH, OUTPUT);
    pinMode(X_DATA, OUTPUT);
    pinMode(X_DATA, OUTPUT);

    _nDotsX = nDotsX;
    _nDotsY = nDotsY;
    _X_SHIFT_DATA = X_SHIFT_DATA;
    _X_SHIFT_CLOCK = X_SHIFT_CLOCK;
    _X_SHIFT_LATCH = X_SHIFT_LATCH;
    _Y_SHIFT_DATA = Y_SHIFT_DATA;
    _Y_SHIFT_CLOCK = Y_SHIFT_CLOCK;
    _Y_SHIFT_LATCH = Y_SHIFT_LATCH;
    _X_DATA = X_DATA;
    _Y_DATA = Y_DATA;
}
