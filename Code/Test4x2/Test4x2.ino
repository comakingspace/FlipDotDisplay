int Data_X = 2;
int Data_Y = 3;

int EN_X[] = {4,5,6,7};

int EN_Y[] = {8,9};

int pulse = 1000;
bool dot = HIGH;
  
void setup() {
  pinMode(Data_X, OUTPUT);
  pinMode(Data_Y, OUTPUT);
  for(int i = 0; i < 4; ++i)
  {
    pinMode(EN_X[i],OUTPUT);
  }
  for(int i = 0; i < 2; ++i)
  {
    pinMode(EN_Y[i],OUTPUT);
  }
}

void flipDot(int x, int y, bool state)
{
  digitalWrite(Data_X, state);
  digitalWrite(Data_Y, !state);

  digitalWrite(EN_X[x], HIGH);
  digitalWrite(EN_Y[y], HIGH);

  delayMicroseconds(pulse);

  digitalWrite(EN_X[x], LOW);
  digitalWrite(EN_Y[y], LOW);

  digitalWrite(Data_X, LOW);
  digitalWrite(Data_Y, LOW);
}

void loop() {
 
  for(int iy = 0; iy < 2; ++iy)
  {
    for(int ix = 0; ix < 4; ++ix)
    {
      flipDot(ix, iy, dot);
      delay(200);
    }
  }

  dot = !dot;

}

