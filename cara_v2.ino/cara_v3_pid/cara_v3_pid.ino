int leva_dopredu = 5; 
int leva_dozadu = 6; 
int prava_dozadu = 10; 
int prava_dopredu = 9; 
const int SensorLeft = A3;        // Left sensor input pin 
const int SensorMiddle = A2;      // The sensor input pin 
const int SensorRight = A1;       // Right sensor input pin 

float SL, SM, SR;     
float soucet, posledni_cteni, odchylka;

float rychlostMin=0; //50
float rychlostMax=255;
float stred = 1000;

float zaklad = 60; //od -100 do 100
bool pid_adaptivni = true;
float zakladni_pid[] = {0.5, 0, 0.2};
float agresivni_pid[] = {0.2, 0.02, 0.2}; //=zakladni_pid[] -> v setupu se hodnoty srovnaji s zakladni_pid[]
float prah_prechodu = 200; //soucet hodnot senzoru (tohle je neco ve smyslu, že jsou všechny na bile)

float magicka_hodnota = -16423;

void reset_pid(){
 for(int i=0; i<3; i++){
  agresivni_pid[i] = zakladni_pid[i];
 }
}

void setup () 
 {   
 if(pid_adaptivni == false) reset_pid();
  
   Serial.begin (9600); 
   pinMode (leva_dopredu, OUTPUT);    // Pin 6 (PWM) 
   pinMode (leva_dozadu, OUTPUT);    // Pin 7 (PWM) 
   pinMode (prava_dozadu,    OUTPUT);    // Pin 8 (PWM) 
   pinMode (prava_dopredu,    OUTPUT);    // Pin 9 (PWM) 
   pinMode (SensorLeft, INPUT); // define left Sensors 
   pinMode (SensorMiddle, INPUT) ;// definition sensors 
   pinMode (SensorRight, INPUT); // definition of the right sensor 
 } 

float odchylka_pid = 0;
void loop () 
 {
  odchylka = avg_exp(3,chyba(pozice(), 1000));
  
  //Adaptivni PID regulator
  if(soucet > prah_prechodu){
    odchylka_pid = pid(odchylka, zakladni_pid);
  }else{
    odchylka_pid = pid(odchylka, agresivni_pid);
  }
  
  sleduj_caru(zaklad, odchylka_pid);
  vypis();
  delay(5);
 }
  
float leva_cast, prava_cast;
void sleduj_caru(float rychlost, float regulace)
{ 
  leva_cast = rychlost + regulace;
  prava_cast = rychlost - regulace;
  float rozpeti_min = -100; float rozpeti_max = 100;
  leva_cast = map(leva_cast, rozpeti_min, rozpeti_max, -rychlostMax, rychlostMax);
  prava_cast = map(prava_cast, rozpeti_min, rozpeti_max, -rychlostMax, rychlostMax);

  leva_cast = constrain(leva_cast, -255, 255);
  prava_cast = constrain(prava_cast, -255, 255);

  if(prava_cast > 0){
    analogWrite(prava_dozadu, 0);
    analogWrite(prava_dopredu, prava_cast);
  }
  if(prava_cast < 0){
    analogWrite(prava_dopredu, 0);
    analogWrite(prava_dozadu, fabs(prava_cast));
  } 
  if(leva_cast > 0){
    analogWrite(leva_dozadu, 0);
    analogWrite(leva_dopredu, leva_cast);
  }
  if(leva_cast < 0){
    analogWrite(leva_dopredu, 0);
    analogWrite(leva_dozadu, fabs(leva_cast));
  }
}

void zastav()
{
  digitalWrite(prava_dopredu, LOW);
  digitalWrite(leva_dopredu, LOW); 
  digitalWrite(prava_dozadu, LOW);
  digitalWrite(leva_dozadu, LOW);
}

//VAZENY PRUMER S EXPONENCIALNIM ZAPOMINANIM - K_AVG 10 ZNAMENA ZE SOUCASNA HODNOTA MA VAHU 1/10
float vystup_avg = 1;
float avg_exp(float K_AVG, float vstup)
{
   vystup_avg = vystup_avg + 1/K_AVG * (vstup - vystup_avg);
return vystup_avg;
}

float pamatuj_si_posledni_cteni(float vstup)
{
     posledni_cteni = vstup > 1850 ? 2000 : vstup < 150 ? 0 : posledni_cteni;
     return posledni_cteni;
}

float pozice()
{
  SL = analogRead (SensorLeft); 
  SM = analogRead (SensorMiddle); 
  SR = analogRead (SensorRight);

   soucet=SL+SM+SR; //to bude v prumeru kolem 1500
   float pos = (SL*0+SM*1000+SR*2000)/(soucet); //stred = 1000
   float tmp = pamatuj_si_posledni_cteni(pos);
   if(soucet < 200) pos = tmp;
   return pos;
}

float chyba(float promenna, float stala_hodnota)
{
  float rozdil = promenna - stala_hodnota;
    rozdil = map(rozdil, -1000, 1000, -100, 100);
  return rozdil;
}

void vypis()
{  Serial.print("LMR "); 
  Serial.print((int)SL); Serial.print("|"); 
  Serial.print((int)SM); Serial.print("|"); 
  Serial.print((int)SR); 
  Serial.print(" POZICE "); Serial.print(pozice()); 
   Serial.print(" SOUCET "); Serial.print(soucet); 
   Serial.print(" CHYBA "); Serial.print(odchylka); 
   
   Serial.print(" PID ");  
   Serial.print(odchylka_pid);
   
   Serial.print(" PL "); 
   Serial.print((int)prava_cast); Serial.print("|");
   Serial.print((int)leva_cast); 
  Serial.println(" ");
}

float integral = 0; float last_error = 0;
float pid(float error, float pid_konstanty[3]){
  float derivativ = error - last_error;
  float compute_pid = error * pid_konstanty[0] + integral * pid_konstanty[1] + derivativ * pid_konstanty[2]; 
  integral += error;
  float last_error = error;
  return compute_pid;
}
