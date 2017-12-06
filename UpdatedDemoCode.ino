#include <Servo.h>                          // Use the Servo library (included with Arduino IDE)  
int hash=0;                                 // Count the number of hash marks passed
Servo servoL;                               // Define the left and right servos
Servo servoR;
double thres = .1;                          // threshold to detect if a light is on or off
float tm;
int incoming, hashNum;
bool onLight = false;
int goSignal = 103;                         // ASCII value for a 'g'
#define Rx 17                               // DOUT to pin 10
#define Tx 16                               // DIN to pin 11
bool sent = false, finished = false;
int count = 0, readyCars = 0, numLights = 0;

int A = 51, B = 2, C = 3, D = 4, E = 5, F = 6, G = 7;
int disp[7][7] = {{1, 1, 1, 1, 1, 1, 0},    // 2D array for storing HIGH/LOW values for 7-seg
               {0, 1, 1, 0, 0, 0, 0},
               {1, 1, 0, 1, 1, 0, 1},
               {1, 1, 1, 1, 0, 0, 1},
               {0, 1, 1, 0, 0, 1, 1},
               {1, 0, 1, 1, 0, 1, 1}};

void setup()                                // set up variables
{
  Serial.begin(9600);                       // Set up serial monitors 1 and 2 for debugging and communication
  Serial2.begin(9600);
  servoL.attach(13);                        // Attach (programmatically connect) servos to pins on Arduino
  servoR.attach(12);

  pinMode(A, OUTPUT);                       // setting pin modes for the 7-seg pins
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  hashNum = 6;                              // Initially hashNum is 6 because there are 6 hashes that need to
                                            // be passed. Later is will be changed to 6-numLights for the
                                            // trash shoot
}

void loop()                                 // main loop
{
  LineFollowing(hashNum);                   // line follow as long as bot has passed fewer than hashNum hashes
  SenseLight();                             // incrementally sense for a light
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
  delayMicroseconds(230);                   // Short delay to allow capacitor charge in QTI module
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
  delayMicroseconds(230);                   // Short delay to allow capacitor charge in QTI module
                                            // Set direction of pins as INPUT
  int leftSide = digitalRead(9);
  int leftMid = digitalRead(10);
  int rightMid = digitalRead(11);
  int rightSide = digitalRead(53);
  int vL, vR;


                                            // all the possible cases of QTI sensors over the black line or hash mark
  if (rightSide == 0 && rightMid == 1 && leftMid == 1 && leftSide == 1) {
    vL = -25;                             
    vR = 100;
  }
  if (rightSide == 0 && rightMid == 0 && leftMid == 1 && leftSide == 1) {
    vL = -50;
    vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 0 && leftSide == 1) {
    vL = -100;
    vR = 100;
  }

  if (rightSide == 0 && rightMid == 0 && leftMid == 1 && leftSide == 0) {
    vL = -100;
      vR = 100;
  }
  if (rightSide == 0 && rightMid == 1 && leftMid == 1 && leftSide == 0) {
    vL = 75;
    vR = 75;
  }
  if (rightSide == 1 && rightMid == 0 && leftMid == 0 && leftSide == 0) {
    vL = 100;
      vR = -100;
  }
  if (rightSide == 1 && rightMid == 1 && leftMid == 0 && leftSide == 0) {
    vL = 100;
    vR = -50;
  }
  if (rightSide == 1 && rightMid == 1 && leftMid == 1 && leftSide == 0) {
    vL = 100;
    vR = -25;
  }
  if (rightSide == 0 && rightMid == 1 && leftMid == 0 && leftSide == 0) {
    vL = 100;                             
    vR = -100;
  }
  if (rightSide == 0 && rightMid == 0 && leftMid == 0 && leftSide == 0) {
    vL = -100;
    vR = -100;
    delay(20);
  }
  if (rightSide == 1 && rightMid == 1 && leftMid == 1 && leftSide == 1) // this is when the bot is on a hash mark
  {
    hash++;                                         // increment the number of hashes past
    if (numLights > 0 && hash == 1)                 // when driving down the trash shoot, send 'g' on first hash
      Serial2.print('g');
    if(hash == maxHash){                            // check if bot has reached number of desired hashes (either 6 or 6-numLights)
      servoL.writeMicroseconds(1500);               // stop the bot
      servoR.writeMicroseconds(1500);
      Communicate();                                // start communicating with other bots
      if (maxHash == 6-numLights)                   // once the bot has reached the final hash on the trash shoot, stop driving
        Finished();
    if (sent)
        return;
    }
    else {                                          // if the bot has not reached the final hash, drive over the hash mark
      vL = 100;
      vR = 100;
      servoL.writeMicroseconds(1500 + vL);          // Move forward - delay is so it doesnt immediately stop again
      servoR.writeMicroseconds(1500 - vR);
      delay(700);
    }
  }
  servoL.writeMicroseconds(1500 + vL);              // Steer robot to recenter it over the line
  servoR.writeMicroseconds(1500 - vR);
  
  delay(10);                                        // Delay for 10 milliseconds
}


void SenseLight() {                                 // function to sense if passing a light
  if (volts(A2)>thres) {                            // get the voltage coming into analog read 2 and see if it is above the threshold
   if (!onLight && (numLights == 0 || (millis() - tm > 800 ))) {  // don't want to read the same light more than once so wait 800 ms
      numLights++;                                  // increment the number of lights read
      tm = millis();            
   }
    onLight = true;
  }
  else
    onLight = false;
}

float volts(int adPin)                              // function to calculate the voltage
{ 
  return float(analogRead(adPin)) * 5.0 / 1024.0;   // Calculate the voltaage input in A2
} 

void Communicate() {                                // once the bot has reached the final hash, it will communicate with the others
  for (int x = 2; x <= 7; x++)                      // display its number on the 7-seg
    if (numLights != 0) {
      digitalWrite(x, disp[numLights][x-1]);
      if (numLights==2 || numLights==3 || numLights==5)
        digitalWrite(51, HIGH);
    }
  delay(20);                                        // slight delay to make sure the 7-seg turns on
  if (!sent) {                                      // Send the number of lights read to the main computer
    Serial.print("NumLights: ");
    Serial.println(numLights);
    Serial2.print(numLights);
    sent = true;
  }
  Listen();                                         // start listening for other bots to send a 'g'
}

void Listen() {
  tm = millis();
  while (true) {
    if (count == 1)                                   // if bot has received 'g' and driven down the trash
      Finished();                                     // shoot then stop doing anything
      
    if(Serial2.available())                           // listen for incoming messages from other bots
     incoming = Serial2.read();
    if (incoming > 64 && incoming < 124 && incoming == 103) { // if the incoming signal is a 'g' then increment the number
      readyCars++;                                          // of cars who have driven down the trash shoot
      tm = millis();
    }
  
    if (readyCars == numLights-1 || numLights == 1) { // if bot is first bot or the bot before it has driven down the
      trashShoot();                                   // trash shoot then start driving down the shoot
      count++;
      break;
    }
    else
      incoming = NULL;
    delay(100);
    if (millis() - tm > 30000) {                    // if no bot has sent a 'g' for 30 seconds, we send one
      Serial2.print('g');
      readyCars++;
    }
  }
}

void trashShoot() {                                 // function to drive down the trash shoot
  int vL = 100, vR = 30;                            // prepare bot to make a turn over
  servoL.writeMicroseconds(1500 + vL);              // drive over to the trash shoot
  servoR.writeMicroseconds(1500 - vR);
  delay(1500);
  servoL.writeMicroseconds(1500);                   // Stops
  servoR.writeMicroseconds(1500);
  hash = 0;                                         // reset hashes
  hashNum = 6-numLights;                            // this becomes the max number of hashes that the bot wants to drive over
                                                    // ends and goes back to main loop to start line following again
}

void Finished() {                                   // function for after bot has completed everything
  long tillStop = millis();
  tm = millis();                                    // if no bot has sent a 'g' for 30 seconds, we send one
  while (millis() - tillStop < 180000) {
    if (millis() - tm > 30000) {
      Serial2.print('g');
      tm = millis();
    }
  }
  delay(50000000000);                               // delay for a looooooong time
}

