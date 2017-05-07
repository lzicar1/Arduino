int leva_dopredu = 5; 
int leva_dozadu = 6; 
int prava_dozadu = 10; 
int prava_dopredu = 9; 
 const int SensorLeft = A3;        // Left sensor input pin 
 const int SensorMiddle = A2;      // The sensor input pin 
 const int SensorRight = A1;       // Right sensor input pin 
 int SL;      
 int SM;       
 int SR;      

int rychlostMin=50;
int rychlostMax=255;
int sensorMin = 0;
int sensorMax = 1024;

 void setup () 
 {   
   Serial.begin (9600); 
   pinMode (leva_dopredu, OUTPUT);    // Pin 6 (PWM) 
   pinMode (leva_dozadu, OUTPUT);    // Pin 7 (PWM) 
   pinMode (prava_dozadu,    OUTPUT);    // Pin 8 (PWM) 
   pinMode (prava_dopredu,    OUTPUT);    // Pin 9 (PWM) 
   pinMode (SensorLeft, INPUT); // define left Sensors 
   pinMode (SensorMiddle, INPUT) ;// definition sensors 
   pinMode (SensorRight, INPUT); // definition of the right sensor 
 } 

int mez = 65;

void dopredu(int rychlost)
{
  rychlost = map(rychlost, rychlostMin, rychlostMax, 0, 100); 
  analogWrite(prava_dopredu, rychlost);
  analogWrite(leva_dopredu, rychlost); 
}

void doleva(int rychlost)
{
  rychlost = map(rychlost, rychlostMin, rychlostMax, 0, 100); 
  analogWrite(prava_dopredu, rychlost);
  analogWrite(leva_dopredu, 0); 
}

void doprava(int rychlost)
{
  rychlost = map(rychlost, rychlostMin, rychlostMax, 0, 100); 
  analogWrite(prava_dopredu, 0);
  analogWrite(leva_dopredu, rychlost); 
}

int pomala_zatacka = 1;

void kontroluj_hranu(int rychlost_zataceni, int rychlost_narovine)
{
  if(SL > mez)
    {
      doprava(rychlost_zataceni);
    } else if (SR > mez) {
      doleva(rychlost_zataceni);
    } else {
      dopredu(rychlost_narovine);
    }
}

void loop () 
 {
  SL = map(analogRead (SensorLeft), sensorMin, sensorMax, 100, 0); 
  SM = map(analogRead (SensorMiddle), sensorMin, sensorMax, 100, 0); 
  SR = map(analogRead (SensorRight), sensorMin, sensorMax, 100, 0);

  Serial.println(SM);
  if(SM < mez)
  {
    kontroluj_hranu(5, 100);
  }else{
    kontroluj_hranu(60, 100);
  }
  
  
 // Serial.print(SL); Serial.print(" "); Serial.println(SR);
  delay(5);
 }
