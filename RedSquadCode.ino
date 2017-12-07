#include <Servo.h>                           // Use the Servo library (included with Arduino IDE)  
int hash=0;
Servo servoL;                                // Define the left and right servos
Servo servoR;
double thres = .1;
float tm;
int incoming, hashNum;
char theG;
bool onLight = false;
char goSignal = 'g';
int black=0;
int white=0;
const int buttonPin = 2; //pushbutton
const int rledPin = 6; //red led
const int gledPin = 7; //green led
const int qti= 51; //sensing qti
const int rrledPin = 11; //receiving led
const int syledPin = 10; //sending led
#define Rx 17 // DOUT to pin 10
#define Tx 16 // DIN to pin 11
int buttonState=0;
bool sent = false;            // For demo to send once when bot reaches the end
bool finished = false;
int count = 0, readyCars = 0;

int A = 39, B = 37, C = 35, D = 47, E = 45, F = 41, G = 43;
int disp[7][7] = {{1, 1, 1, 1, 1, 1, 0},
               {0, 1, 1, 0, 0, 0, 0},
               {1, 1, 0, 1, 1, 0, 1},
               {1, 1, 1, 1, 0, 0, 1},
               {0, 1, 1, 0, 0, 1, 1},
               {1, 0, 1, 1, 0, 1, 1}};
               
int blackminuswhite = 0; //black blocks minus white blocks

void setup()
{
  Serial.begin(9600);                        // Set up Arduino Serial Monitor at 9600 baud
  Serial2.begin(9600);
  servoL.attach(13);                         // Attach (programmatically connect) servos to pins on Arduino
  servoR.attach(12);
  pinMode(rledPin, OUTPUT);
  pinMode(gledPin, OUTPUT);
  pinMode(rrledPin, OUTPUT);
  pinMode(syledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  pinMode(A, OUTPUT); //setup 7 segment display
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
  //SenseBlocks();
  
  delay(10);
}



void LineFollowing(int maxHash) { //Line Following
  pinMode(3, OUTPUT);   
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
                        // Set level of Arduino pins to HIGH
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(9, HIGH);
  delayMicroseconds(230);                    // Short delay to allow capacitor charge in QTI module
                          // Set direction of pins as INPUT
  pinMode(3, INPUT);   
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(9, INPUT);
                         // Set level of pins to LOW
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(9, LOW);
  delayMicroseconds(230);                    // Short delay to allow capacitor charge in QTI module
                          // Set direction of pins as INPUT
  int leftSide = digitalRead(3);
  int leftMid = digitalRead(4);
  int rightMid = digitalRead(5);
  int rightSide = digitalRead(9);
  int vL, vR;
  
  Serial.print(leftSide);                 // Display result of D4-D7 pins in Serial Monitor
  Serial.print(rightMid); 
  Serial.print(leftMid); 
  Serial.print(rightSide); 
  Serial.println(" ");

 /* int myPins[]= {37, 35, 47, 45, 41, 43};
  for (int x = 0; x < 6; x++){
    if (blackminuswhite != 0) {                         // for loop to turn on seven segment display
      digitalWrite(myPins[x], disp[blackminuswhite][x+1]);
      if (blackminuswhite==2 || blackminuswhite==3 || blackminuswhite==5)
        digitalWrite(A, HIGH);
    }
  }*/

  if (rightSide == 0 && rightMid == 1 && leftMid == 1 && leftSide == 1) //turn based on qti sensors
  {
    vL = -100;                             
    vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 1 && leftSide == 1)
  {
    vL = -100;
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
    vL = 130;
      vR = 130;
  }
  
  
  if (rightSide == 1 && rightMid == 0 && leftMid == 0 && leftSide == 0)
  {
    vL = 100;
      vR = -100;
  }

  if (rightSide == 1 && rightMid == 1 && leftMid == 0 && leftSide == 0)
  {
    vL = 100;
    vR = -100;
  }
  
  if (rightSide == 1 && rightMid == 1 && leftMid == 1 && leftSide == 0)
  {
    vL = 100;
    vR = -100;
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
    if(blackminuswhite>0 && hash==1){ //print a g at first hashmark in trashshoot
      Serial2.print(goSignal);
    }
    if(hash == maxHash){ //when bot gets to end of the line
      servoL.writeMicroseconds(1500);
      servoR.writeMicroseconds(1500);
      blackminuswhite=black-white; //calculate black blocks minus white blocks
      if(blackminuswhite<0){
        blackminuswhite=blackminuswhite*-1;
      }
      Communicate(); //communicate with other bots
      Serial.print(maxHash); //
      Serial.print("   ");
      Serial.println(blackminuswhite); //print black blocks minus white blocks
      if (maxHash == blackminuswhite) {
        Finished(); //stop at the end of the trashshoot
      }
      if (sent) //if bot communicated, return
        return;
    }
    else { //stop at each hashmark and sense
      vL = 0;
      vR = 0;
      servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
      servoR.writeMicroseconds(1500 - vR);
      SenseBlocks(); //sense block
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
}

//below is to make qti sensitivity adjustable
long rcTime(int pin)                         // ..returns decay time
{
  pinMode(pin, OUTPUT);                      // Charge capacitor
  digitalWrite(pin, HIGH);                   // ..by setting pin ouput-high
  delay(1);                                  // ..for 5 ms
  pinMode(pin, INPUT);                       // Set pin to input
  digitalWrite(pin, LOW);                    // ..with no pullup
  long time  = micros();                     // Mark the time
  while (digitalRead(pin));                  // Wait for voltage < threshold
  time = micros() - time;                    // Calculate decay time
  return time;                               // Return decay time
}

void SenseBlocks(){ //block sensing
  long qtiSense=rcTime(qti);
  if(qtiSense<=7000){ //block is white
    digitalWrite(gledPin, HIGH);
    delay(500);
    digitalWrite(gledPin, LOW);
    white+=1;
    //Serial.print(white);
  }
  if(qtiSense>7000){ //block is black
    digitalWrite(rledPin, HIGH);
    delay(500);
    digitalWrite(rledPin, LOW);
    black+=1;
    //Serial.print(black);
  }
}

/* void SenseBlocks() {
     pinMode(qti, OUTPUT); 
digitalWrite(qti, HIGH);
delayMicroseconds(230);
pinMode(qti, INPUT);
digitalWrite(qti, LOW);
delayMicroseconds(230);
int qtiSense=digitalRead(qti);
//Serial.print(qtiSense);
//Serial.println(" ");
if(qtiSense == 1){
  digitalWrite(rledPin, HIGH);
  delay(500);
  digitalWrite(rledPin, LOW);
  black+=1;
  Serial.println("black");
}

if(qtiSense == 0){
  digitalWrite(gledPin, HIGH);
  delay(500);
  digitalWrite(gledPin, LOW);
  white+=1;
  Serial.println("white");
}
delay(400);
} */

void Communicate() { //communicate at end of line
  if(finished==true|| count==1){
    Finished();
  }
  Serial.print("started comm");
 int myPins[]= {37, 35, 47, 45, 41, 43};
  for (int x = 0; x < 6; x++){ //displays blackminuswhite
    if (blackminuswhite != 0) {                // for loop to turn on seven segment display
      digitalWrite(myPins[x], disp[blackminuswhite][x+1]);
      if (blackminuswhite==2 || blackminuswhite==3 || blackminuswhite==5)
        digitalWrite(A, HIGH);
    }
  }
   //delay(2000);                                        // display numLights for 4 seconds
   //for (int x = 0; x < 6; x++){                       // turn the seven segment display off
    //if (blackminuswhite != 0) {
      //digitalWrite(myPins[x], LOW);
      //if (blackminuswhite==2 || blackminuswhite==3 || blackminuswhite==5)
        //digitalWrite(A, LOW);
    //}
  // }
  if (!sent) {                                          // Send the number of lights read to the main computer
    //Serial.print("Blackminuswhite: ");
    //Serial.println(blackminuswhite);
    Serial2.print(blackminuswhite);
    sent = true;
  }
  Listen();
}

void Listen() { 
  //int myPins[]= {37, 35, 47, 45, 41, 43};
  if (count == 1) //when you get to the end of the trashshoot, count=1
    Finished(); //stop
     //char a = 0;
    //int count = 0;
    /*while (blackminuswhite != 1 && a != 97) {
      Serial2.print('m');
      delay(100);
      a = Serial2.read();
      int t = millis();
      Serial2.flush();
      while ( millis() - t < 3000){
      a = Serial2.read();
      if (a=='a'){
        break;
      }
    }
    }*/
  //Serial.println("in listening");
  if(Serial2.available()) { // Is data available from XBee?
   incoming = Serial2.read(); //read a g
  }
   //theG=Serial2.read();

    if (incoming > 64 && incoming < 124 && incoming == 103 ){ //if bot gets a g, add 1 to readyCars
    //if(theG=='g'){
      readyCars++;
      Serial.print(readyCars);
    }
      
    if (readyCars == blackminuswhite-1 || blackminuswhite == 1) { //if it's your turn next or you're first, go
      //Serial.print(incoming);
      trashShoot();
      //count++;
      return;
    }
    else {
    //if (incoming != NULL && incoming != blackminuswhite) {
      //for (int x = 0; x < 6; x++) {
        //digitalWrite(myPins[x], HIGH);
        //digitalWrite(A, HIGH);
     // }
      //delay(1000);
      //for (int x = 0; x < 6; x++) {
        //digitalWrite(myPins[x], LOW);
        //digitalWrite(A, LOW);
      //}
    //}
    incoming = NULL;
  }
  delay(1000);
  
  Listen(); //listen again if it's not your turn yet
}

/*void wait(){
  int atEnd=0;
  char teams[] = {'q', 'w', 'e', 't', 'u'}; 
  char ourTeamColor= 'w';                     // change to a letter not used by anyone else. Yellow team is q, Green is t, Red is w, Blue is e, Orange is u
  for (int x = 0; x < sizeof(teams); x++)     // goes through and removes your team's color     
    if (teams[x] == ourTeamColor)
      teams[x] = 'n';                         // sets index to "n" for null
  while (atEnd<4){
    Serial2.print(ourTeamColor);              // send out your team color        
    if(Serial2.available()>0 || incoming!=ourTeamColor){
      char incoming = Serial2.read();         // read incoming signal
      for (int x = 0; x < sizeof(teams); x++) 
          if (incoming == teams[x]) {         // if signal has not been read already, mark bot as read
            teams[x] = 'n';
            atEnd+=1;
          }
    }
  }
  //int t = millis();
    //while (millis() - t < 4000)
      //Serial2.print('a');
}*/

void trashShoot() {
  //if(blackminuswhite==1){
    //wait();
  //}
  count+=1;
  Serial.println("In trashshoot");
  int vL = 100, vR = 30;
  servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
  servoR.writeMicroseconds(1500 - vR);
  delay(1500);
  servoL.writeMicroseconds(1500);      // Stops for half a second
  servoR.writeMicroseconds(1500);
  hash = 0;
  hashNum = 6-blackminuswhite;
}

void Finished() {                   //stop at end of trash shoot
  finished=true;
  //Serial2.print(goSignal);
  //blackminuswhite=black-white;
      //if(blackminuswhite<0){
        //blackminuswhite=blackminuswhite*-1;
   //   }
 
  //int myPins[]= {37, 35, 47, 45, 41, 43};
  /*for (int x = 0; x < 6; x++){
    if (blackminuswhite != 0) {                               // for loop to turn on segment seven display
      digitalWrite(myPins[x], disp[blackminuswhite][x+1]);
      if (blackminuswhite==2 || blackminuswhite==3 || blackminuswhite==5)
        digitalWrite(A, HIGH);
    }
  }*/
  //for (int x = 0; x < 6; x++) {
    //if (myPins[x] != 45)
      //digitalWrite(myPins[x], HIGH);
    //digitalWrite(A, HIGH);
  //}
  delay(50000000000);
}

