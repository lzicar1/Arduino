int leva_dopredu = 5; 
int leva_dozadu = 6; 
int prava_dozadu = 10; 
int prava_dopredu = 9; 
const int SensorLeft = A3;        // Left sensor input pin 
const int SensorMiddle = A2;      // The sensor input pin 
const int SensorRight = A1;       // Right sensor input pin 

int SL, SM, SR;     
float soucet, chyba, posledni_cteni;

int rychlostMin=50;
int rychlostMax=255;
float stred = 1000;

float KP = 0.1;
float rychlost = 30;

float K_AVG = 20;
float magicka_hodnota = -16423;

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

void sleduj_caru(int rychlost, float regulace)
{
  rychlost = map(rychlost, 0, 100, rychlostMin, rychlostMax);
  regulace = map(rychlost, 0, 100, rychlostMin, rychlostMax); 
  analogWrite(prava_dopredu, rychlost + regulace);
  analogWrite(leva_dopredu, rychlost - regulace); 
}

void zastav()
{
  digitalWrite(prava_dopredu, LOW);
  digitalWrite(leva_dopredu, LOW); 
}

//VAZENY PRUMER S EXPONENCIALNIM ZAPOMINANIM - K_AVG 10 ZNAMENA ZE SOUCASNA HODNOTA MA VAHU 1/10
float vystup_avg = 1;
float avg_exp(float K_AVG, float vstup)
{
   vystup_avg = vystup_avg + 1/K_AVG * (vstup - vystup_avg);
return vystup_avg;
}

float pozice()
{
   soucet=SL+SM+SR; //to bude v prumeru kolem 1500
   float chytravec = (SL*0+SM*1000+SR*2000)/(soucet); //stred = 1000

   if(chytravec > 1900) posledni_cteni = 2000;
   if(chytravec < 100) posledni_cteni = 0;

    if(soucet < 3*20) //(kdyz vsechny sensory videj bilou)
      return posledni_cteni;

    if(soucet > 2900) //kdyz vsechny senzory videj cernou
      return magicka_hodnota;

   float kde_jsi = avg_exp(K_AVG,chytravec); //s touhle hodnotou bude chtít experimentovat
   return kde_jsi;
}

void vypis()
{
   Serial.print("POZICE: "); Serial.print(pozice()); 
   Serial.print(" AVG: "); Serial.print(avg_exp(20,pozice()));
   Serial.print(" CHYBA: "); Serial.println(chyba * KP);
}


void loop () 
 {
  SL = analogRead (SensorLeft); 
  SM = analogRead (SensorMiddle); 
  SR = analogRead (SensorRight);

  if(pozice() == magicka_hodnota)
  {
    zastav();
  }
  else
  {
    chyba = stred - pozice();
    sleduj_caru(rychlost, chyba * KP);
  }  
  
  delay(5);
 }
