#import <Servo.h>
Servo upDown;
Servo leftRight;
Servo leftBrow;
Servo rightBrow;
#define second() (millis()/1000)



//=============================== data reading settings

const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;

//=============================== time settings

unsigned long lastMillis = 0;
unsigned long currentTime = 0;
unsigned long noMoreData = 0;

//=============================== following settings

float faceX = 1400;
float faceY = 1000;
float servoX = 1400;
float servoY = 1000;
int followSpeed = 5;
int boredTimer = 2000;
int timeSinceLastFace;

//=============================== random movement settings

int randomEvent;
int eventType;
int eventOptions = 2;
int eyebrowOptions = 2;
int eyebrowType;
int lr;
int randomMin = 10;
int randomMax = 10;
unsigned long randomStart;
int intervalA;
int intervalB;
int intervalC;
int intervalD;
boolean isRandom;

//=============================== reading settings

int rows = 8;
int words = 40;
int currentRow, currentWord;
int readSpeed = 100;

//=============================== servo setup

int xCentre = 1400;
int yCentre = 1000;
int maxMove = 200;

//===============================

void setup() {
  Serial.begin(9600);
  leftRight.attach(9);
  upDown.attach(3);
  leftBrow.attach(5);
  rightBrow.attach(10);
  upDown.writeMicroseconds(1000);
  leftRight.writeMicroseconds(1400);
  resetRandom();
}

//====================

void loop() {
  upDown.writeMicroseconds(yCentre);
  leftRight.writeMicroseconds(xCentre);
  return;
  currentTime = millis();
  recieveData();
  if (second() == randomEvent && !isRandom) {
    //resetRandom();
    isRandom = true;
    randomStart = millis();
    Serial.println("random time!");
  }
   //followSmooth();
   //return;
  if (millis() > noMoreData) {
    if (isRandom) {
      startRandomEvent(eventType);
           Serial.print("random Event type");
           Serial.println(eventType);
    } else {
      readBook();
      eyebrowsRead();
    }
  } else {
    // followInstant();
    followSmooth();
  }
}


//====================

void resetRandom() {
  isRandom = false;
  randomEvent = second() + int(random(randomMin, randomMax));
  eventType = round(random(1, eventOptions+0.8));
  eyebrowType = round(random(1, eyebrowOptions+0.25));
  lr = round(random(1,2));
  intervalA = 1000 + random(500, 2000);
  intervalB = intervalA + random(500, 1000);
  intervalC = intervalB + random(500, 1000);
  intervalD = intervalC + random(500, 1000);
  Serial.println("random reset");
  Serial.println(intervalA);
  Serial.println(intervalB);
  Serial.println(intervalC);
  Serial.println(intervalD);
}

void startRandomEvent(int type) {
  if (type == 1) {
    lookUpAndFrown();
  } else if(type == 2){
    eyebrowRaise(lr);
  }
}

void recieveData() {

  if (Serial.available() > 0) {

    char x = Serial.read(); // sets char to whatever just got sent

    if (x == endMarker) { // checks if the current character is the end marker and deals with
      readInProgress = false; // the data
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }

    if (readInProgress) { // updates the array of stored number, if it goes above what is specified
      inputBuffer[bytesRecvd] = x; // it will write over last digit
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { // checks if the current character is the start marker and choses to record it
      bytesRecvd = 0;
      readInProgress = true;
    }
  }

}

//=================

void parseData() {
  // split the data up into x and y values

  char * strtokIndx;

  strtokIndx = strtok(inputBuffer, ","); // get first part of string
  faceX = map(atof(strtokIndx), 0, 500, xCentre+maxMove,xCentre-maxMove);

  strtokIndx = strtok(NULL, ",");
  faceY = map(atof(strtokIndx), 0, 500, yCentre-maxMove, yCentre+maxMove); // maxmove sets the max movement of the servo
  noMoreData = millis() + boredTimer;
}

//=============== follow instant goes to the face instantly but is too jittery

void followInstant() {
  upDown.writeMicroseconds(faceY);
  leftRight.writeMicroseconds(faceX);
}

//=============== follow smooth isnt effected by the speed of data transfer and is given a coordinate to move towards slowly

void followSmooth() {
  if (currentTime - lastMillis > followSpeed) {
    if (faceX > servoX) {
      servoX++;//=map(faceX-servoX,0,300,0,maxSpeed);
    } else if (faceX < servoX) {
      servoX--;//=map(faceX-servoX,0,300,0,maxSpeed);
    }
    if (faceY > servoY) {
      servoY++;//map(faceY-servoY,0,300,0,maxSpeed);
    } else if (faceY < servoY) {
      servoY--;//=map(faceY-servoY,0,300,0,maxSpeed);
    }

    if (!isRandom) {
      upDown.writeMicroseconds(servoY);
      leftRight.writeMicroseconds(servoX);
      leftBrow.writeMicroseconds(map(servoY, 1300, 700, 1720 - 100, 1720 - 200) - map(servoX, xCentre - maxMove, xCentre + maxMove, 50, 0));
      rightBrow.writeMicroseconds(map(servoY, 700, 1300, 1000 + 100, 1000 + 200) + map(servoX, xCentre - maxMove, xCentre + maxMove, 50, 0));
    } else {
      eyebrowMove(eyebrowType);
      upDown.writeMicroseconds(servoY);
      leftRight.writeMicroseconds(servoX);
    }
  }
}

//==============

void readBook() {
  if (currentTime - lastMillis >= readSpeed) {
    lastMillis = currentTime;
    servoX = xCentre - (words * 5) + (currentWord * 10); // sets the servo position similar to how a for loop does, just without the chance of getting stuck in the loop
    servoY = yCentre + 200 + (currentRow * 20);
    currentWord++;
    upDown.writeMicroseconds(servoY);
    leftRight.writeMicroseconds(servoX);
    if (currentWord > words) { // if at the end of the row, go back to the start
      currentRow++;
      currentWord = 0;
Serial.println("read a row");
      if (currentRow > rows) { // if at the last row of the 'book' go back to the first line
        currentRow = 0;
     
      }
    }
  }
}

//================ eyebrow control. Is seperate so the eyebrows can move independantly of the eyes

void eyebrowsRead(){
    leftBrow.writeMicroseconds(1720 + map(servoX, xCentre - maxMove, xCentre + maxMove, 75, -75) - map(servoY, 1300, 700, 0, 300));
    rightBrow.writeMicroseconds(950 - map(servoX, xCentre - maxMove, xCentre + maxMove, -75, 75) + map(servoY, 1300, 700, 0, 300));
}
