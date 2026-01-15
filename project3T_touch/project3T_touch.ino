#include <DFPlayer_Mini_Mp3.h>

#include "SoftwareSerial.h"

SoftwareSerial mySerial(10, 11); // RX, TX

int currentTrack = 1;       // current playing song's number
const int TOTAL_TRACKS = 3; // total songs in the SD card

int isPlaying = 0;

int prevT = 0;
int cnt = 0;

// Threshold
const int THRESHOLD = 400;

void setup() {
  // pressure sensor
  pinMode(A0, INPUT);

  mySerial.begin(9600);
  mp3_set_serial(mySerial);
  mp3_set_volume(25);
  delay(1000);
  mp3_play(1);
  delay(1000);
  mp3_play(2);
  delay(1000);
  mp3_play(3);
  delay(1200);

  mp3_stop(); // 初期状態は停止

  Serial.begin(9600);
}

void loop() {
  bool isTouched = false;

  isTouched = checkTouch();

  if (isTouched) {
    playSound();
  }

  delay(10);
} 

bool checkTouch() {
  int pres_data;
  pres_data = readSensor();
  Serial.println(pres_data);

  if (pres_data <= THRESHOLD) {
    cnt++;
  }
  else {
    cnt = 0;
  }

  if (cnt > 2) {
    cnt = 0;
    return true;
  }

  return false;
}

int readSensor() {
  return analogRead(A0);
}

void playSound() {
  if (currentTrack > TOTAL_TRACKS) {
    currentTrack = 1;
  }

  Serial.println("Play");
  mp3_play(currentTrack);
  delay(600);

  currentTrack++;
}