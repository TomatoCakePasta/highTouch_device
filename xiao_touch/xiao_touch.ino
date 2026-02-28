#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

#define PIN_MP3_RX 6
#define PIN_MP3_TX 7

SoftwareSerial ss_mp3_player(PIN_MP3_RX, PIN_MP3_TX);
DFPlayerMini_Fast mp3_player;

#define THRESHOLD 600
#define LONG_PRESS_MS 3000

enum PlayMode {
  NORMAL,
  MELODY
};

PlayMode playMode = NORMAL;

struct PlayerState {
  int currentTrack;
};

PlayerState player = {1};

// 演奏モード用メロディ（/ は除外）
const int melody[] = {
  3,3,4,5, 5,4,3,2, 1,1,2,3, 3,2,2,
  3,3,4,5, 5,4,3,2, 1,1,2,3, 2,1,1
};

const int harmony[] = {
  1,1,2,3, 3,2,1,6, 3,3,4,5, 4, 3,3
};

const int MELODY_LEN = sizeof(melody) / sizeof(melody[0]);
const int HARMONY_LEN = sizeof(harmony) / sizeof(harmony[0]);
int melodyIdx = 0;

const bool HARMONY_MODE = true;

// 押下管理
unsigned long pressStart = 0;
bool isPressing = false;

void setup() {
  Serial.begin(9600);
  delay(1000);

  ss_mp3_player.begin(9600);
  delay(1000);

  if (!mp3_player.begin(ss_mp3_player)) {
    Serial.println("DFPlayer not found");
    while (1);
  }

  mp3_player.reset();
  delay(1000);
  mp3_player.volume(30);
  delay(1000);

  for (int i = 1; i <= 10; i++) {
    mp3_player.playFromMP3Folder(i);
    Serial.println(i);
    delay(700);
  }

  Serial.println("DFPlayer ready");
}

void loop() {
  handleTouch();
}

void handleTouch() {
  int pres = readSensor();

  if (pres >= THRESHOLD) {
    if (!isPressing) {
      isPressing = true;
      pressStart = millis();
    }

    // 長押し判定
    if (millis() - pressStart >= LONG_PRESS_MS) {
      toggleMode();
      isPressing = false;
      delay(500); // チャタリング防止
    }
  }
  else {
    if (isPressing) {
      // 短押し
      playSound();
    }
    isPressing = false;
  }
}

void toggleMode() {
  if (playMode == NORMAL) {
    playMode = MELODY;
    melodyIdx = 0;
    Serial.println("→ MELODY MODE");
  }
  else {
    playMode = NORMAL;
    Serial.println("→ NORMAL MODE");
  }
}

void playSound() {
  if (playMode == NORMAL) {
    if (player.currentTrack > 3) {
      player.currentTrack = 1;
    }

    mp3_player.playFromMP3Folder(player.currentTrack);
    Serial.print("Normal Play: ");
    Serial.println(player.currentTrack);
    player.currentTrack++;
  }
  else {
    int track = 0;

    if (HARMONY_MODE) {
      track = harmony[melodyIdx] + 5;


      melodyIdx++;
      if (melodyIdx >= HARMONY_LEN) {
        melodyIdx = 0;
      }
    }    
    else {
      track = melody[melodyIdx] + 5;

      melodyIdx++;
      if (melodyIdx >= MELODY_LEN) {
        melodyIdx = 0;
      }
    }
    mp3_player.playFromMP3Folder(track);

    Serial.print("Melody Play: ");
    Serial.println(track);
  }

  delay(300); // 再生間隔
}

int readSensor() {
  return analogRead(A0);
}