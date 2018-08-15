/*
 * FlipDotDisplay Atmel328 Firmware
 * 
 * This is the software for the display controller of the FlipDotDisplay. It handles the 
 * management of the shift registers and H-bridge drivers. The communication between this 
 * controller and the ESP8266 is done via serial communication with an 8bit header and a
 * checksum byte.
 * 
 * 
 * by Patrick Kübler (2018)
 * License: Creative Commons 4.0
 */



//number of dots in X and Y 
//(For multidisplay: enter size of total display)
const int sizeX = 24;
const int sizeY = 48;

//multidisplay position
const int multi = 2; // first  =^ 0
const int multiSize = 3;// displays in Y

// Shift register pins
const int pin_Shift_Data_X = 2;   
const int pin_Shift_Clock_X = 3;
const int pin_Shift_Latch_X = 4;

const int pin_Shift_Data_Y = 5;
const int pin_Shift_Clock_Y = 6;
const int pin_Shift_Latch_Y = 7;

// enables output of shift registers, active = LOW
const int OE = 10;

// sets output of the half-briges 
// LOW = GND
// HIGH = VIN
const int pin_Data_X = 8;
const int pin_Data_Y = 9;

// pulse length of a bit flip
// in µSec
const int time_Flip = 800;


// Serial protocol
// ===============
const uint8_t serial_header = 0xaa;
const uint8_t dataSize = (sizeX * sizeY/multiSize )/8;
//header+payload+checksum
const int bufferSize = (dataSize*multiSize) + 2;

uint8_t dataBuffer[bufferSize];
uint8_t readCounter = 0;
bool isHeader = false;
bool firstTimeHeader = false;

// array for storing the current display 
byte currentData[dataSize] = { 0 };

void setup() {
  Serial.begin(115200);
  
  pinMode(OE, OUTPUT);
  digitalWrite(OE, LOW);
  
  pinMode(pin_Shift_Data_X, OUTPUT);
  pinMode(pin_Shift_Clock_X, OUTPUT);
  pinMode(pin_Shift_Latch_X, OUTPUT);

  pinMode(pin_Shift_Data_Y, OUTPUT);
  pinMode(pin_Shift_Clock_Y, OUTPUT);
  pinMode(pin_Shift_Latch_Y, OUTPUT);

  pinMode(pin_Data_X, OUTPUT);
  pinMode(pin_Data_Y, OUTPUT);

  // wipe display
  clearAll();
}

void loop() {

  if( Serial.available() ){
    //Serial.println("reciving Data");
    readData();
  }
  //delay(1000);
  //Serial.print("dataSize= ");
  //Serial.println(dataSize);
}
    

void clearAll(){
  for( int i = 0; i<(sizeX*sizeY); i++ ){
    writeFlip(i%sizeX,
              i/sizeX,
              LOW);
  }
  for( int i = 0; i < dataSize; i++ ){
    currentData[i] = B00000000;
  }
}

void readData() {
  // get first byte from Serial buffer
  uint8_t c = Serial.read();
  
  if ( c == serial_header ) {
    if( !firstTimeHeader ) {
      isHeader = true;
      readCounter = 0;
      firstTimeHeader = true;
    }
  }
  
  dataBuffer[readCounter] = c;
  readCounter++;
  
  if( readCounter >= (bufferSize) ) {

    readCounter = 0;
    
    if( isHeader ) {
      uint8_t checksumByte = dataBuffer[bufferSize-1];
    
      if( checksum(checksumByte) ) {
        
        // Transfer buffer into display data
        byte data[dataSize];

        for( int i = 0; i < dataSize; i++ ){
          data[i] = dataBuffer[i+1 + (dataSize*multi)];
        }

        writeData(data);
      }
      else{
        Serial.println("Error");
      }
      
      // restart header flag
      isHeader = false;
      firstTimeHeader = false;
    }
  }
}

bool checksum(byte checksumValue) {
  //return true;
  byte result = 0;
  uint16_t sum = 0;
  
  for( int i = 0; i < (bufferSize-1); i++ ) {
    sum += dataBuffer[i];
  }
  result = sum & 0xFF;
  
  if(checksumValue == result ) {
     return true;
  }
  else {
     return false;
  }
}

void writeData(byte data[]) {

  Serial.println("Writing Display");
  for( int i=0; i<dataSize; i++ ){ 
    for( int j=0; j<8; j++ ){
      // compare current dot with new one
      if( bitRead(data[i], 7-j) != bitRead(currentData[i], 7-j) ){
        writeFlip( ( (i*8) + j) % sizeX,
                   ( (i*8) + j) / sizeX,
                   bitRead(data[i], 7-j)
                  );
      }
    }
  }
  // save new dislay data
  memcpy(currentData, 
         data,
         sizeof(currentData)*sizeof(byte));
}

void writeFlip(uint8_t cooX, uint8_t cooY, bool flip) {
  
  pushCoordinate(cooX,cooY);

  if( flip ){
    digitalWrite(pin_Data_Y, HIGH);
    delayMicroseconds(time_Flip);
    digitalWrite(pin_Data_Y, LOW);
  }
  else {
    digitalWrite(pin_Data_X, HIGH);
    delayMicroseconds(time_Flip);
    digitalWrite(pin_Data_X, LOW);
  }
}

void pushCoordinate(uint8_t cooX, uint8_t cooY) {
  /*
      0 1 2 3 4 5 6 7 …
    0
    1
    2   X
    3
    4
    5
    6
    7
    …
  */
  //coordianates start at 0

  // push Y coordinates in reverse order
  // so if dot 0x0 is flipped the high bit
  // is pushed in last

  // Y-Axis
  for( int i = 0; i < sizeY; i++ ){
    
    if ( i == ( (sizeY-1) - cooY) ){
      pushDataY(HIGH);
    }
    else {
      pushDataY(LOW);
    }
  }

  // X-Axis
  for( int i = 0; i < sizeX; i++ ){
    if(i == ( (sizeX-1) - cooX ) ){
      pushDataX(HIGH);
    }
    else {
      pushDataX(LOW);
    }
  }

  //move the data to the output registers
  digitalWrite(pin_Shift_Latch_X, HIGH);
  digitalWrite(pin_Shift_Latch_X, LOW);

  digitalWrite(pin_Shift_Latch_Y, HIGH);
  digitalWrite(pin_Shift_Latch_Y, LOW);
}

void pushDataX(bool state) {
  // pushes a new bit into the shift registers
  // digitalWrite() delay is long enough to trigger the clock
  
  digitalWrite(pin_Shift_Data_X, state);
  digitalWrite(pin_Shift_Clock_X, HIGH);
  digitalWrite(pin_Shift_Clock_X, LOW);
  // latching needs to be done in the pushCoordinates function
}

void pushDataY(bool state) {
  // pushes a new bit into the shift registers
  // digitalWrite() delay is long enough to trigger the clock
  
  digitalWrite(pin_Shift_Data_Y, state);
  digitalWrite(pin_Shift_Clock_Y, HIGH);
  digitalWrite(pin_Shift_Clock_Y, LOW);
  // latching needs to be done in the pushCoordinates function
}
