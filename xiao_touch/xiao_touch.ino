#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

#define PIN_MP3_RX 6
#define PIN_MP3_TX 7

// RX means Receive, TX means Transmit

// DFPlayer's TX -> XIAO's RX (PIN6)
// DFPlayer's RX <- XIAO's TX (PIN7)

SoftwareSerial ss_mp3_player(PIN_MP3_RX, PIN_MP3_TX);
DFPlayerMini_Fast mp3_player;

const int TOTAL_TRACKS = 3; // total songs in the SD card

const int THRESHOLD = 600;

struct PlayerState {
  int currentTrack;
};

PlayerState player = {1};

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Start");

  ss_mp3_player.begin(9600);
  delay(1000);

  if (!mp3_player.begin(ss_mp3_player)) {
    Serial.println("DFPlayer not found");
    while (1);
  }

  mp3_player.reset();
  delay(1500);

  Serial.println("DFPlayer ready");

  mp3_player.volume(30);
  delay(1500);

  mp3_player.play(1);
  delay(1000);
  mp3_player.play(1);
  delay(1000);
  mp3_player.play(2);
  delay(1000);
}

void loop() {
  
  // bool isTouched = false;

  // isTouched = checkTouch();

  // if (isTouched) {
  //   playSound();
  // }

  playSound();

  delay(3000);
}


bool checkTouch() {
  static int cnt = 0;

  int pres_data;
  pres_data = readSensor();
  Serial.println(pres_data);

  if (pres_data <= THRESHOLD) {
    cnt++;
  }
  else {
    cnt = 0;
  }

  if (cnt > 1) {
    cnt = 0;
    return true;
  }

  return false;
}

int readSensor() {
  return analogRead(A0);
}

void playSound() {
  if (player.currentTrack > TOTAL_TRACKS) {
    player.currentTrack = 1;
  }

  Serial.println("Play");
  mp3_player.play(player.currentTrack);
  delay(600);

  player.currentTrack++;
}