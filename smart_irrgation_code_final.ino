// Automated Outdoor Residential Irrigation System Code 
// Author : Jack Hayes 
// Last update : April 13 , 7:14 pm 

#include <Adafruit_Sensor.h>

#include <LiquidCrystal_I2C.h>

#include <Servo.h> // servo library
#include <DHT.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2); // A5 to SCL , A4 to SDA

#define LED1 2// 
#define LED2 3 //
#define LED3 4 // MOTOR LED 
#define LED4 5
#define RELAY 6 // motor relay high = off , low = on 
#define DHTPIN1 7 
#define BUZZER 8 
#define SERVOMOTOR 9
#define TRIG 10 
#define ECHO 11 
#define SOILMOIST1 A0  
#define RAINFALL A1  

DHT HT (DHTPIN1, DHT11); // creating the object 

Servo myservo;  

// define gloabl varibales for system parameters 
int count ;
float sum; 
float tankLvlCM;
long duration_us; 
float soilMos1;  
float tempF; 
float tempC; 
float rainfallVal; 

int motorpos = 50; 
int minwatertankLvl= 16; // THE HEIGHT OF THE TANK IS 20 CM SO and dont want water to get below 4 cm so when the sensor see that is more tha n 16 cm measure water level to low 
int rainfallThreashhold= 50; 
int soilMoisturemin = 50; // when measured mositure is above this value then action , the more dry the soil the larger the value 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Initialise Serial debug output
  // put your setup code here, to run once: 
  HT.begin(); //Initialise the DHT sensor , starting the object 
  //delay(500);

  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  pinMode(LED3, OUTPUT); 
  pinMode(LED4, OUTPUT);
  pinMode(RELAY,OUTPUT); 
  pinMode(BUZZER,OUTPUT);  
  pinMode(RAINFALL,INPUT);      
  pinMode(TRIG, OUTPUT);                  // Relay Module PIN D8
  pinMode(ECHO, INPUT);
                             
  myservo.attach(SERVOMOTOR);                                   //  Servo PIN D9

  digitalWrite(RELAY, LOW);                           // Relay Normally High for OFF condition
  digitalWrite(TRIG, LOW);                          // trig normally low 
  digitalWrite (BUZZER, HIGH);      // buzzer pin normally low 
             

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("IRRAGATION SYS");
  lcd.setCursor(0,1);
  lcd.print("IS ACTIVATED!");
  delay(2000);
  lcd.clear();

}

void loop() {
  //put your main code here, to run repeatedly:  
  // lcd printing 
  lcd.setCursor(0,0);
  lcd.print("Obtaining System"); 
  lcd.setCursor(0,1); 
  lcd.print("Measurments");
  delay(2000); 
  lcd.clear(); 
  // now we are hoing to get the enviromental measurments using call functions 
  measureDistance(); 
  indiLED1();
  measureTemperature(); 
  indiLED2();
  measureSoilmositure(); 
  indiLED3();
  measureRainfall (); 
  indiLED4(); 
  // now we are using the measured values in conditional statments to perform output functions 
  if ( tankLvlCM >= minwatertankLvl ) { 
    lowWaterlevelnotify() ;  
  } 

  else if (!(measureRainfall()) && (tankLvlCM < minwatertankLvl ) && (soilMos1 >= soilMoisturemin) &&  (tempC >= 4.4) && (tempC < 21) ) { 
    waterpumplowtime();  
  } 

  else if (!(measureRainfall()) && (tankLvlCM < minwatertankLvl ) && (soilMos1 >= soilMoisturemin) && (tempC > 21) ) { 
    waterpumphightime(); 
  } 

}

//declaring indicating led functions 
void indiLED1(){ 
  digitalWrite (LED1,HIGH ); 
  Serial.print("distance: ");
  Serial.print(tankLvlCM);
  Serial.println("cm"); 
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Current Water"); 
  lcd.setCursor(0,1); 
  lcd.print("Level:");  
  lcd.print(tankLvlCM); 
  delay(2000);
  lcd.clear();
  digitalWrite (LED1,LOW );

} 
void indiLED2(){ 
  digitalWrite (LED2,HIGH ); // led is turned on 
  lcd.setCursor(0,0); 
  lcd.print("Air Temperature:"); 
  lcd.setCursor(0,1);
  lcd.print(tempC);
  lcd.print((char)223);
  lcd.print("C"); 
  Serial.println("temperature: ");
  Serial.println(tempC); 
  delay(2000); // here i want to delay the system so that the lcd stays on for 5 seconds and the text printed on the lcd stays on for 5 seconds ,
  lcd.clear(); 
  digitalWrite (LED2,LOW ); 
} 
void indiLED3(){ 
  digitalWrite (LED3,HIGH );
  lcd.setCursor(0,0); 
  lcd.print("Soil Moisture:"); 
  lcd.setCursor(0,1); 
  lcd.print(soilMos1); 
  //lcd.print("%"); 
  Serial.println ("soil mositure: "); 
  Serial.println(soilMos1); 
  delay(2000); // here i want to delay the system so that the lcd stays on for 5 seconds and the text printed on the lcd stays on for 5 seconds ,
  lcd.clear(); 
  digitalWrite (LED3,LOW); 
} 
void indiLED4(){ 
  digitalWrite (LED4,HIGH);
  lcd.setCursor(0,0); 
  lcd.print("Rainfall Level: "); 
  lcd.setCursor(0,1);
  lcd.print(rainfallVal); 
  Serial.println ("rainfall value: ");
  Serial.println(rainfallVal);
  delay(2000); // here i want to delay the system so that the lcd stays on for 5 seconds and the text printed on the lcd stays on for 5 seconds ,
  lcd.clear();
  digitalWrite (LED4,LOW);
} 

void measureDistance() { 
  
  sum = 0 ; 

  count = 14; 

  for (int i=0; i<count; i++){ 
  // this first measurment is getting distance of the water level in the water tank
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    duration_us = pulseIn(ECHO, HIGH);
    sum += 0.017 * duration_us; 
    //Serial.println(tankLvlCM); 
    delay(200);
  } 
  tankLvlCM = sum/count ;  
  
  }
//declaring 
void measureTemperature() {
  tempF= HT.readTemperature(DHTPIN1); 
  tempC = ((tempF-32)/ 1.8);
} 

void measureSoilmositure () {
  soilMos1= analogRead(SOILMOIST1); 
  soilMos1 = map(soilMos1,0,1024,0,100);
} 

bool measureRainfall () { 
  rainfallVal = analogRead(RAINFALL); 
  rainfallVal = map(rainfallVal, 0, 1023, 0, 100); 
  if (rainfallVal <= 50){ 
    return true ; 
  } 
  else { 
    return false ; 
  } 
} 

void lowWaterlevelnotify(){ 
  do {
    digitalWrite (BUZZER, LOW); 
    lcd.setCursor(0,0);
    lcd.print("Water Tank Level:"); 
    lcd.setCursor(0,1);    
    lcd.print("LOW!");   
    measureDistance(); 
    Serial.print(tankLvlCM); 
    Serial.print ('\n');     
  } while (tankLvlCM >= minwatertankLvl); 
  digitalWrite (BUZZER, HIGH);
} 

void waterpumplowtime(){ // water pump should be on for 15 seconds  

  Serial.println ("water pump on 15 sec");

  digitalWrite(RELAY, HIGH); //relay goes high so that the water pump turns on 
          lcd.setCursor(0,0); 
          lcd.print("MOTOR STATUS:ON"); 
          lcd.setCursor(0,1); 
          lcd.print("TIME ON:15");
          for (int i = 0 ; i < 8 ; i++) {
  for (motorpos = 75; motorpos <= 118; motorpos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(motorpos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (motorpos = 118; motorpos >= 75; motorpos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(motorpos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
         
          digitalWrite(RELAY, LOW); 
          lcd.clear(); 
}

void waterpumphightime(){ // water pump on for 30 sec

    Serial.println ("water pump on 30 sec");

    digitalWrite(RELAY, HIGH); //relay goes high so that the water pump turns on 
          lcd.setCursor(0,0); 
          lcd.print("MOTOR STATUS:ON"); 
          lcd.setCursor(0,1); 
          lcd.print("TIME ON:30");
          for (int i = 0 ; i < 16 ; i++)  {
  for (motorpos = 75; motorpos <= 118; motorpos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(motorpos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (motorpos = 118; motorpos >= 75; motorpos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(motorpos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
          
            digitalWrite(RELAY, LOW); 
            lcd.clear(); 
}


