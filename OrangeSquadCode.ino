#include <Servo.h>                           // Use the Servo library (included with Arduino IDE)  
int hash=0;
Servo servoL;                                // Define the left and right servos
Servo servoR;
double thres = .1;
float tm;
int incoming, hashNum;
bool onLight = false;
int goSignal = 103;
int button = 0; // might be unnecessary
int L;
int R;
int tagnum = 0;

#define Rx 10 // DOUT to pin 10
#define Tx 11 // DIN to pin 11

int buttonState=0;
bool sent = false;            // For demo to send once when bot reaches the end
bool finished = false;
int count = 0, readyCars = 0;

int A = 35, B = 33, C = 37, D = 45, E = 47, F = 43, G = 51;
int disp[7][7] = {{1, 1, 1, 1, 1, 1, 0},
               {0, 1, 1, 0, 0, 0, 0},
               {1, 1, 0, 1, 1, 0, 1},
               {1, 1, 1, 1, 0, 0, 1},
               {0, 1, 1, 0, 0, 1, 1},
               {1, 0, 1, 1, 0, 1, 1}};
               
int numTags = 0;

void setup()
{
  numTags = 0;
  Serial.begin(9600);                        // Set up Arduino Serial Monitor at 9600 baud
  Serial2.begin(9600);
  servoL.attach(12);                         // Attach (programmatically connect) servos to pins on Arduino
  servoR.attach(13);

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
  //SenseRFID();
  
  delay(10);
}


void LineFollowing(int maxHash) {
pinMode(4, INPUT); // QTI far right
  pinMode(5, INPUT); // mid right
  pinMode(6, INPUT); // mid left
  pinMode(2, INPUT); // far left
  pinMode(3, INPUT); // QTI sensing sub
                                        // update QTIs and convert to string
 // might make an update qti methiod
  long Q1 = rcTime(4); // far right
  if (Q1 < 300) {Q1 = 0;}
  else {Q1 = 1;}
  long Q2 = rcTime(5); // mid right
  if (Q2 < 300) {Q2 = 0;}
  else { Q2 = 1;}
  long Q3 = rcTime(6); // mid left
  if (Q3 < 300) {Q3 = 0;}
  else {Q3 = 1;}
  long Q4 = rcTime(2); // far left
  if (Q4 < 300) {Q4 = 0;}
  else {Q4 = 1;}
  long Q5 = rcTime(3); // Q sense sub
  if (Q5 < 300) {Q5 = 0;} // sensing sub
  else {Q5 = 1;}
  int QTI;
  String Q = String(Q4) + String(Q3) + String(Q2) + String(Q1);
  // far left, mid left, mid right, far right
  QTI = Q.toInt();
  //Serial.println(QTI);
  Serial.println(hash);
  
  switch (QTI) {                        // check and see where we are
    case 1000: // turn left fast
      L = -600; 
      R = -600;
      //delay(50);
      break;
    case 1100: // turn left ish
      L = -500; 
      R = -500;
      //delay(0);
      break;
    case 100: // wiggle
      L = -100; // USED TO BE ZERO
      R = -100;
      //delay(80);
      break;
    case 110: // straight ahead
      L = 50;
      R = -50;
      //delay(50);
      break;
    case 10: // wiggle
      L = 100;
      R = 0;
      //delay(80);
      break;
    case 11: // turn right 
      L = 500;
      R = 500;
      //delay(0);
      break;
    case 1: // turn a lot right
      L = 600;
      R = 600;
      //delay(50);
      break;
    case 0: // back up
      L = -60;
      R = 60;
      //delay(50);
      break;
          case 1110:
      L = -1400;
      R = -1400;
      //delay(200);
      break;
    case 111:
      L = 1400;
      R = 1400;
      //delay(200);
      break;
    case 1111: // hash mark
    hash++;
    if(numTags > 0 && hash == 1) {
      Serial2.print('g');
    }
    if(hash == maxHash){
      servoL.writeMicroseconds(1510);
      servoR.writeMicroseconds(1495);
      Communicate();
      Serial.print(maxHash);
      Serial.print("   ");
      Serial.println(numTags);
      if (maxHash == numTags) {
        Finished();
      }
      if (sent)
        return;
    }
    else {
      /*L = 10;
      R = -5;
      servoL.writeMicroseconds(1500 + L);      // Stops for half a second
      servoR.writeMicroseconds(1500 + R);*/
      SenseRFID();
      //delay(500);
      L = 600;
      R = -600;
      servoL.writeMicroseconds(1500 + L);      // Move forward - delay is so it doesnt immediately stop again
      servoR.writeMicroseconds(1500 + R);
      delay(300);
    }
  }
  
  servoL.writeMicroseconds(1500 + L);      // Steer robot to recenter it over the line
  servoR.writeMicroseconds(1500 + R);
  
  delay(10);                                // Delay for 10 milliseconds (1/100 second)
}


void SenseRFID() {
  //int sensingSub = digitalRead(3);
  long Q5 = rcTime(3); // Q sense sub
  if (Q5 < 300) {Q5 = 0;} // sensing sub
  else {Q5 = 1;}
  delay(50);
  Serial.println("sensing");
  if (Q5==1) {// get the voltage coming into analog read 2 and see if it is above the threshold
      Serial.println("found");
      numTags++;                    // increment the number of lights read            
    } 
  Serial.println(numTags);
}


void Communicate() {
  int myPins[] = {33, 37, 45, 47, 43, 51};
  for (int x = 0; x < 6; x++)
    if (numTags != 0) {                               // for loop to turn on segment seven display
      digitalWrite(myPins[x], disp[numTags][x+1]);
      if (numTags==2 || numTags==3 || numTags==5)
        digitalWrite(A, HIGH);
    }
   //delay(2000);                                        // display numTags for 4 seconds
   //for (int x = 0; x < 6; x++){                      // turn the seven segment display off
    //if (numTags != 0) {
      //digitalWrite(x, LOW);
      //if (numTags==2 || numTags==3 || numTags==5)
        //digitalWrite(A, LOW);
  //}
   
  if (!sent) {                                          // Send the number of lights read to the main computer
    Serial.print("numTags: ");
    Serial.println(numTags);
    Serial2.print(numTags);
    sent = true;
  }
  Listen();
}

void Listen() {
  if (count == 1)
    Finished();
    
  if(Serial2.available()) { // Is data available from XBee?
   incoming = Serial2.read();
  }
    if (incoming > 64 && incoming < 124 && incoming == 103)
      readyCars++;
    Serial.println(readyCars);
    if (readyCars == numTags-1 || numTags == 1) {
      Serial.print(incoming);
      trashShoot();
      count++;
      return;
    }
    else {
    if (incoming != NULL && incoming != numTags) { // maybe I don't need this if tree?
      int myPins[] = {33, 37, 45, 47, 43, 51};
        for (int x = 0; x < 6; x++) {
        digitalWrite(x+1, HIGH);
        //digitalWrite(A, HIGH);
        if (numTags==2 || numTags==3 || numTags==5)
        digitalWrite(A, HIGH);
      }
      delay(1000);
        for (int x = 0; x < 6; x++)  {
        digitalWrite(x+1, LOW);
        digitalWrite(A, LOW);
      }
    }
    incoming = NULL;
  }
  delay(1000);
  
  Listen();
}


void trashShoot() {
  int vL = 100, vR = -30;
  servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
  servoR.writeMicroseconds(1500 + vR);
  delay(2000);
  servoL.writeMicroseconds(1500);      // Stops for half a second
  servoR.writeMicroseconds(1500);
  hash = 0;
  hashNum = 6-numTags;
}

void Finished() {
  delay(100);
  char g = 'g';
 // Serial2.flush();
  //Serial2.print(g);
  //Serial2.flush();
  //delay(100);
  //int myPins[] = {33, 37, 45, 47, 43, 51};
  //for (int x = 0; x < 6; x++) { {
    //if (x+1 != 5)
      //digitalWrite(x+1, HIGH);
    //digitalWrite(A, HIGH);
  //}
  delay(50000000000);
  
}

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


