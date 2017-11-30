#include <Servo.h>                           // Use the Servo library (included with Arduino IDE)  
int hash=0;
Servo servoL;                                // Define the left and right servos
Servo servoR;
double thres = .1;
float tm;
int incoming, hashNum;
bool onLight = false;
int goSignal = 103;
#define Rx 17 // DOUT to pin 10
#define Tx 16 // DIN to pin 11
int buttonState=0;
bool sent = false;            // For demo to send once when bot reaches the end
bool finished = false;
int count = 0, readyCars = 0;

int A = 51, B = 2, C = 3, D = 4, E = 5, F = 6, G = 7;
int disp[7][7] = {{1, 1, 1, 1, 1, 1, 0},
               {0, 1, 1, 0, 0, 0, 0},
               {1, 1, 0, 1, 1, 0, 1},
               {1, 1, 1, 1, 0, 0, 1},
               {0, 1, 1, 0, 0, 1, 1},
               {1, 0, 1, 1, 0, 1, 1}};
               
int numLights = 0;

void setup()
{
  Serial.begin(9600);                        // Set up Arduino Serial Monitor at 9600 baud
  Serial2.begin(9600);
  servoL.attach(13);                         // Attach (programmatically connect) servos to pins on Arduino
  servoR.attach(12);

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  hashNum = 6;
}

void loop()
{
  LineFollowing(hashNum);
  SenseLight();
  
  delay(10);
}


void LineFollowing(int maxHash) {
  pinMode(9, OUTPUT);   
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(53, OUTPUT);
                        // Set level of Arduino pins to HIGH
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(53, HIGH);
  delayMicroseconds(230);                    // Short delay to allow capacitor charge in QTI module
                          // Set direction of pins as INPUT
  pinMode(9, INPUT);   
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(53, INPUT);
                         // Set level of pins to LOW
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(53, LOW);
  delayMicroseconds(230);                    // Short delay to allow capacitor charge in QTI module
                          // Set direction of pins as INPUT
  int leftSide = digitalRead(9);
  int leftMid = digitalRead(10);
  int rightMid = digitalRead(11);
  int rightSide = digitalRead(53);
  int vL, vR;

  if (rightSide == 0 && rightMid == 1 && leftMid == 1 && leftSide == 1)
  {
    vL = -25;                             
    vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 1 && leftSide == 1)
  {
    vL = -50;
    vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 0 && leftSide == 1)
  {
    vL = -100;
      vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 1 && leftSide == 0)
  {
    vL = -100;
      vR = 100;
  }

  if (rightSide == 0 && rightMid == 1 && leftMid == 1 && leftSide == 0)
  {
    vL = 50;
      vR = 50;
  }
  
  
  if (rightSide == 1 && rightMid == 0 && leftMid == 0 && leftSide == 0)
  {
    vL = 100;
      vR = -100;
  }

  if (rightSide == 1 && rightMid == 1 && leftMid == 0 && leftSide == 0)
  {
    vL = 100;
    vR = -50;
  }
  
  if (rightSide == 1 && rightMid == 1 && leftMid == 1 && leftSide == 0)
  {
    vL = 100;
    vR = -25;
  }
  
if (rightSide == 0 && rightMid == 1 && leftMid == 0 && leftSide == 0)
  {
    vL = 100;                             
    vR = -100;
  }
  
  if (rightSide == 0 && rightMid == 0 && leftMid == 0 && leftSide == 0)
  {
    vL = -100;
    vR = -100;
    delay(20);
  }
  
  if (rightSide == 1 && rightMid == 1 && leftMid == 1 && leftSide == 1)
  {
    hash++;
    if(hash == maxHash){
      servoL.writeMicroseconds(1500);
      servoR.writeMicroseconds(1500);
      Communicate();
      if (maxHash == numLights) {
        Finished();
      }
      if (sent)
        return;
    }
    else {
      vL = 0;
      vR = 0;
      servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
      servoR.writeMicroseconds(1500 - vR);
      delay(500);
      vL = 100;
      vR = 100;
      servoL.writeMicroseconds(1500 + vL);      // Move forward - delay is so it doesnt immediately stop again
      servoR.writeMicroseconds(1500 - vR);
      delay(500);
    }
  }
  
  servoL.writeMicroseconds(1500 + vL);      // Steer robot to recenter it over the line
  servoR.writeMicroseconds(1500 - vR);
  
  delay(10);                                // Delay for 10 milliseconds (1/100 second)
  Serial.println(numLights);
}


void SenseLight() {
  Serial.print(" ");
  Serial.print(volts(A2));
  Serial.print(",");
  if (volts(A2)>thres) {              // get the voltage coming into analog read 2 and see if it is above the threshold
   if (!onLight && (numLights == 0 || (millis() - tm > 2500 ))) {
      numLights++;                    // increment the number of lights read
      tm = millis();            
   }
    onLight = true;
  } 
  else {
    onLight = false;
  }
  Serial.print(numLights);
}


float volts(int adPin)
{ 
  return float(analogRead(adPin)) * 5.0 / 1024.0;       // Calculate the voltaage input in A2
} 

void Communicate() {
  for (int x = 2; x <= 7; x++)
    if (numLights != 0) {                               // for loop to turn on segment seven display
      digitalWrite(x, disp[numLights][x-1]);
      if (numLights==2 || numLights==3 || numLights==5)
        digitalWrite(51, HIGH);
    }
   delay(2000);                                        // display numLights for 4 seconds
   for (int x = 2; x <= 7; x++)                         // turn the seven segment display off
    if (numLights != 0) {
      digitalWrite(x, LOW);
      if (numLights==2 || numLights==3 || numLights==5)
        digitalWrite(51, LOW);
  }
  if (!sent) {                                          // Send the number of lights read to the main computer
    Serial.print("NumLights: ");
    Serial.println(numLights);
    Serial2.print(numLights);
    sent = true;
  }
  Listen();
}

void Listen() {
  if (count == 1)
    Finished();
    char a = 0;
    int count = 0;
    while (numLights != 1 && Serial2.available() > 0 && a == 'z') {
      Serial2.print('z');
      delay(100);
      a = Serial2.read();
      int t = millis();
      Serial2.flush();
      while ( millis() - t < 3000)
        a = Serial2.read();
    }
    
  if(Serial2.available()) { // Is data available from XBee?
   incoming = Serial2.read();

    if (incoming > 64 && incoming < 124 && incoming == 103)
      readyCars++;
    Serial.println(readyCars);
    if (readyCars == numLights-1 || numLights == 1) {
      Serial.print(incoming);
      trashShoot();
      count++;
      return;
    }
    else {
    if (incoming != NULL && incoming != numLights) {
      for (int x = 2; x <= 7; x++) {
        digitalWrite(x, HIGH);
        digitalWrite(51, HIGH);
      }
      delay(1000);
      for (int x = 2; x <= 7; x++) {
        digitalWrite(x, LOW);
        digitalWrite(51, LOW);
      }
    }
    incoming = NULL;
  }
  delay(1000);
  
  }
  Listen();
}

void wait(){
  if (numLights == 1) {
    int atEnd=0;
    char teams[] = {'q', 'w', 'e', 't', 'u'}; 
    char ourTeamColor= 'q';                     // change to a letter not used by anyone else. Yellow team is q, Green is t, Red is w, Blue is e, Orange is u
    for (int x = 0; x < sizeof(teams); x++)     // goes through and removes your team's color     
      if (teams[x] == ourTeamColor)
        teams[x] = 'n';                         // sets index to "n" for null
    while (atEnd<4){     
      if(Serial2.available()>0 || incoming!=ourTeamColor){
        char incoming = Serial2.read();         // read incoming signal
        for (int x = 0; x < sizeof(teams); x++) 
            if (incoming == teams[x]) {         // if signal has not been read already, mark bot as read
              teams[x] = 'n';
              atEnd+=1;
            }
      }
    
    }
    int t = millis();
    while (millis() - t < 4000)
      Serial2.print('a');
    
  }
}

void trashShoot() {
  wait();
  Serial.println("In trashshoot");
  int vL = 100, vR = 30;
  servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
  servoR.writeMicroseconds(1500 - vR);
  delay(1500);
  servoL.writeMicroseconds(1500);      // Stops for half a second
  servoR.writeMicroseconds(1500);
  hash = 0;
  hashNum = 6-numLights;
}

void Finished() {
  delay(100);
  char g = 'g';
  Serial2.flush();
  Serial2.print(g);
  Serial2.flush();
  //delay(100);
  for (int x = 1; x <= 7; x++) {
    if (x != 5)
      digitalWrite(x, HIGH);
    digitalWrite(51, HIGH);
  }
  delay(50000000000);
}

