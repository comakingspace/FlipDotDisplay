#include <Arduino.h>


// Size of the display
const int nDotsX = 24;
const int nDotsY = 16;
// size of one screen refresh
const int DATA_SIZE = nDotsX * nDotsY;

byte dataDisplayed[DATA_SIZE/8];
byte dataStored[DATA_SIZE/8];
byte optionByte = 0;

// length of current pulse in microseconds
const int PULSE_LENGTH = 1000;

// Declaration of pins
const int X_SHIFT_DATA = 2;
const int X_SHIFT_CLOCK = 3;
const int X_SHIFT_LATCH = 4;

const int Y_SHIFT_DATA = 5;
const int Y_SHIFT_CLOCK = 6;
const int Y_SHIFT_LATCH = 7;

const int X_DATA = 8;
const int Y_DATA = 9;

const int OUTPUT_ENABLE = 10;

//
unsigned long serialTimer = 0;
int serialMaxTime = 5; // in millis.


void setup()
{
    pinMode(X_SHIFT_DATA, OUTPUT);
    pinMode(X_SHIFT_CLOCK, OUTPUT);
    pinMode(X_SHIFT_LATCH, OUTPUT);

    pinMode(Y_SHIFT_DATA, OUTPUT);
    pinMode(Y_SHIFT_CLOCK, OUTPUT);
    pinMode(Y_SHIFT_LATCH, OUTPUT);

    pinMode(X_DATA, OUTPUT);
    pinMode(Y_DATA, OUTPUT);

    pinMode(OUTPUT_ENABLE, OUTPUT);
    // HIGH -> no output
    digitalWrite(OUTPUT_ENABLE, HIGH);

    //init serial (UART) communication
    Serial.begin(115200);
}


// pushes the next dot selection in x-axis into storage
// the latch function activates it
// counting from 0
void shiftSetX(int position)
{
    // invert position count as the shift register
    // pushes from 1st to last position
    // aka.: to write the 2nd dot the bit is send
    // second to last
    position = nDotsX - position;

    for(int i = 0; i < nDotsX; ++i)
    {
        // once the loop reaches the right position
        // i == position => TRUE/HIGH
        digitalWrite(X_SHIFT_DATA,
                     i == position);
        // advance clock
        digitalWrite(X_SHIFT_CLOCK, HIGH);
        digitalWrite(X_SHIFT_CLOCK, LOW);

        // reset data line (not strictly necessary)
        digitalWrite(X_SHIFT_DATA, LOW);
    }
}

void shiftSetY(int position)
{
    // invert position count as the shift register
    // pushes from 1st to last position
    // aka.: to write the 2nd dot the bit is send
    // second to last
    position = nDotsY - position;

    for(int i = 0; i < nDotsY; ++i)
    {
        // once the loop reaches the right position
        // i == position => TRUE/HIGH
        digitalWrite(Y_SHIFT_DATA,
                     i == position);
        // advance clock
        digitalWrite(Y_SHIFT_CLOCK, HIGH);
        digitalWrite(Y_SHIFT_CLOCK, LOW);

        // reset data line (not strictly necessary)
        digitalWrite(Y_SHIFT_DATA, LOW);
    }
}

// enables the stored position in registers and activates the
// H-bridges for the given interval (microseconds)
void latchXY()
{
    // write data from storage to outputs
    digitalWrite(X_SHIFT_LATCH, HIGH);
    digitalWrite(Y_SHIFT_LATCH, HIGH);

    digitalWrite(X_SHIFT_LATCH, LOW);
    digitalWrite(Y_SHIFT_LATCH, LOW);

}

void flipDot(int x, int y, bool state)
{
    shiftSetX(x);
    shiftSetY(y);

    // data lanes need to be in opposing states for current to flow
    digitalWrite(X_DATA, state);
    digitalWrite(Y_DATA, !state);

    // latch storage -> output
    latchXY();

    // start pulse
    // enabling output for given pulse length; HIGH = OFF
    digitalWrite(OUTPUT_ENABLE, LOW);
    delayMicroseconds(PULSE_LENGTH);
    digitalWrite(OUTPUT_ENABLE, HIGH);

    // reset data lines
    digitalWrite(X_DATA, LOW);
    digitalWrite(Y_DATA, LOW);
}



bool checkSerial(int expDataSize)
{
    // start timer so see if data arrives in time
    if(Serial.available() == 0)
    {
       serialTimer = millis(); 
    }

    if(Serial.available() == expDataSize)
    {
        return true;
    }
    else if(Serial.available() > expDataSize)
    {
        Serial.println("Error: Data overflow!");
        // Flush Serial data
        while(Serial.available() > 0)
        {
            char t = Serial.read();
        }
        
        return false;
    }
    else
    {
        if(serialTimer + serialMaxTime < millis())
        {
            Serial.println("Error: Data packet too short");
            // Flush Serial data
            while(Serial.available() > 0)
            {
                char t = Serial.read();
            }     
        }
        
        return false;
    }
    return false;
}


void readSerial()
{
    optionByte = Serial.read();
    for(int i = 0; Serial.available() > 0; i++)
    {
        dataStored[i] = Serial.read();
    }
}


void flipOut(byte dots[])
{
    for(int y = 0; y < nDotsY; y++)
    {
        for(int x = 0; x < nDotsX; x++)
        {
            flipDot(x, y, getBit(
                x + y*nDotsX, dots));
        }
    }
    Serial.println("Dots Flipped!");
}

// Nth bit of array
bool getBit(int n, byte array[])
{
    // bitwise operation to check if bit is set
    return ( array[n/8] & (1 << (n%8)) );
}


bool comp_arr(byte * a, const byte * b, int len)
{
    for(int i = 0; i < len; i++)
    {
        if( *(a+i) != *(b+i))
        {
            return false;
        }
    }
    return true;
}


void loop()
{
    // add option byte size
    if(checkSerial(DATA_SIZE/8 + 1))
    {
        readSerial();
    }

    if(!comp_arr(dataDisplayed, dataStored, DATA_SIZE/8))
    {
        flipOut(dataStored);
        // copy arrays
        memcpy(dataDisplayed, dataStored, sizeof(dataDisplayed));
    }
}
