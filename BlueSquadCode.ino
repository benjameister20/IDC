#include <Servo.h>                           // Use the Servo library (included with Arduino IDE)  
#include <SoftwareSerial.h> //Includes SoftwareSerial library
int hash=0; //Variable used to count hashmarks  
int L; //L and R values used to determine wheel spin
int R;
Servo servoL;                                // Define the left and right servos
Servo servoR;
int incoming, hashNum; //variables used to determine when to go and stop on the trashshoot
char goSignal = 'g'; //char that indicates our bot has reached the first hash on trashshoot
#define Rx 11 // DOUT to pin 10
#define Tx 10 // DIN to pin 11
SoftwareSerial Xbee(Rx, Tx); // initializes Xbee
bool sent = false;            // For demo to send once when bot reaches the end
int count = 0, readyCars = 0; //variables used to determine when to leave main track
int pins[]= {45,43,41,39,49,51}; //array of pin numbers for 7 segment display
int A = 47, B = 45, C = 43, D = 41, E = 39, F = 49, G = 51; // assigned letters to pin numbers
int disp[7][7] = {{1, 1, 1, 1, 1, 1, 0}, //Matrix used to correctly light up 7 segment display
               {0, 1, 1, 0, 0, 0, 0},
               {1, 1, 0, 1, 1, 0, 1},
               {1, 1, 1, 1, 0, 0, 1},
               {0, 1, 1, 0, 0, 1, 1},
               {1, 0, 1, 1, 0, 1, 1}};
               
int hallhash = 0; //variable that holds location of deathstar

void setup()
{
  Serial.begin(9600);                        // Set up Arduino Serial Monitor at 9600 baud
  Xbee.begin(9600);                          // Set up Xbee at 9600 baud
  servoL.attach(13);                         // Attach (programmatically connect) servos to pins on Arduino
  servoR.attach(12);

  pinMode(A, OUTPUT);   //initializing pins
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  hashNum = 6;          // setting hashnum to 6
}

void loop() //main loop running linefollowing code
{
  LineFollowing(hashNum); 
  
  
  delay(10);
}


void LineFollowing(int maxHash) { //line following method, sets maxhash to 6
 
 pinMode(4, INPUT);   //initializing pins
 pinMode(5, INPUT);
 pinMode(6, INPUT);
 pinMode(7, INPUT);

long Q1 = rcTime(4); // This block of code using the delay time calculated by rcTime() to determine whether the QTI is on black or white
  if (Q1 < 300) {
    Q1 = 0;
  }
  else {
    Q1 = 1;
  }
  long Q2 = rcTime(5);
  if (Q2 < 300) {
    Q2 = 0;
  }
  else {
    Q2 = 1;
  }
  long Q3 = rcTime(6);
  if (Q3 < 300) {
    Q3 = 0;
  }
  else {
    Q3 = 1;
  }
  long Q4 = rcTime(7);
  if (Q4 < 300) {
    Q4 = 0;
  }
  else {
    Q4 = 1;
  }

  int QTI;
  String Q = String(Q4) + String(Q3) + String(Q2) + String(Q1); //converts QTI values into a string so its easier to understand in switch cases 
  QTI = Q.toInt();
  switch (QTI) { //switch cases used in line following, every possible case covered
    case 1000:
      L = -600;
      R = -600;
      //delay(50);
      break;
    case 1100:
      L = -500;
      R = -500;
      //delay(0);
      break;
    case 100:
      L = 0;
      R = -100;
      //delay(80);
      break;
    case 110:
      L = 50;
      R = -50;
      //delay(50);
      break;
    case 10:
      L = 100;
      R = 0;
      //delay(80);
      break;
    case 11:
      L = 500;
      R = 500;
      //delay(0);
      break;
    case 1:
      L = 600;
      R = 600;
      //delay(50);
      break;
    case 0:
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
      
    case 1111:
      hash++;  //counts hashes passed
      
      if(hallhash>0 && hash==1)  //checks to see if the bot is on the first hash on the trash shoot
      {
        Xbee.print('g'); //sents 1 go signal 
      
      }
      if(hash == maxHash){ //checks to see if at end of main track
      servoL.writeMicroseconds(1500);
      servoR.writeMicroseconds(1500);
      Communicate();
      Serial.print(maxHash);
      Serial.print("   ");
      Serial.println(hallhash);
      if (maxHash == hallhash) { //checks to see if at end of trashshoot 
        
        Finished(); //calls finished method
      }
      if (sent) //if the bot has already reached the end of the main track, sent will be true, returns 
        return;
    }
    SenseHall(); //calls sense hall each time the bot stops on a hash
   
      L = 0;
      R = 0;
      servoL.writeMicroseconds(1500 + L);      // Stops for half a second
      servoR.writeMicroseconds(1500 + R);
      delay(500);
      L = 600;
      R = -600;
      servoL.writeMicroseconds(1500 + L);      // Move forward - delay is so it doesnt immediately stop again
      servoR.writeMicroseconds(1500 + R);
      delay(500);
    
    break;
  }
  
  servoL.writeMicroseconds(1500 + L);      // Steer robot to recenter it over the line
  servoR.writeMicroseconds(1500 + R);
  
  delay(10);                                // Delay for 10 milliseconds (1/100 second)
}



 

void Communicate() {
  for (int x = 0; x < 6; x++)
    if (hallhash != 0) {                               // for loop to turn on segment seven display
      digitalWrite(pins[x], disp[hallhash][x+1]);
      if (hallhash==2 || hallhash==3 || hallhash==5)
        digitalWrite(A, HIGH);
    }
   
  if (!sent) {                                          // Send the deathstar position to the main computer
    Xbee.print(hallhash);
    sent = true;
  }
  Listen(); //calls listen
}

void Listen() {
  if (count == 1) //checks to see if bot is at the end of trashshoot 
    Finished();

  
    
  if(Xbee.available()) { // Is data available from XBee?
   incoming = Xbee.read();
  }

    if (incoming > 64 && incoming < 124 && incoming == 103) //used to listen for other bot's go signals
      readyCars++;
      
    if (readyCars == hallhash-1||hallhash==1) { 
      
      trashShoot(); //calls trashshoot method to move onto trashshoot
      count++;
      return;
    }
    else {
    
    incoming = NULL;
  }
  delay(1000);
  
  
  Listen();
}

void trashShoot() { //method to move from main track to trashshoot
  
  Serial.println("In trashshoot");
  int vL = 100, vR = 30;
  servoL.writeMicroseconds(1500 + vL);      // Stops for half a second
  servoR.writeMicroseconds(1500 - vR);
  delay(1500);
  servoL.writeMicroseconds(1500);      // Stops for half a second
  servoR.writeMicroseconds(1500);
  hash = 0;
  hashNum = 6-hallhash; //calculates position down the trashoot the bot needs to stop at 
}

void Finished() {
  
  delay(5000000000);
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
void SenseHall(){

  float reading = (analogRead(0)); //reads signal from hall effect sensor
  float mag = reading * 5.0; //amplifies signal so treshold can be more precise 
  digitalWrite(3, HIGH);
  if (mag > 2650) { //checks ambient treshhold vs. what the sensor is reading
    
    hallhash = hash; //if threshold is met, set that hash as deathstar location
    Serial.print(hallhash); //prints location to main computer

}
}




