#include <DFPlayerMini_Fast.h>

#define PIN_MP3_RX D6  // DFPlayer TX → XIAO RX
#define PIN_MP3_TX D7   // DFPlayer RX ← XIAO TX

HardwareSerial mp3Serial(1);   // UART1を使用
DFPlayerMini_Fast mp3_player;

const int TOTAL_TRACKS = 3;
#define THRESHOLD 600

#define LONG_PRESS_MS 3000

bool readyForNext = false;

enum PlayMode {
  NORMAL,
  MELODY
};

PlayMode playMode = NORMAL;

struct PlayerState {
  int currentTrack;
};

PlayerState player = {1};

// melody
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

// mode change detection
unsigned long pressStart = 0;
bool isPressing = false;

// ========== IMPORTANT ===========
// CLOSE Serial Monitor
// OR You Can't use RX/TX pins

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Start");

  // UART1 初期化（RX, TX指定）
  mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);
  delay(1000);

  if (!mp3_player.begin(mp3Serial)) {
    Serial.println("DFPlayer not found");
    while (1);
  }

  mp3_player.reset();
  delay(1500);

  Serial.println("DFPlayer ready");

  mp3_player.volume(25);
  delay(1000);

  mp3_player.playFromMP3Folder(1);
  delay(1000);
}

void loop() {

  handleTouch();

  // if (checkTouch()) {
  //   playSound();
  // }
}

void handleTouch() {
  int pres = readSensor();

  if (pres <= 10) {
    readyForNext = true;
  }

  if (pres >= THRESHOLD) {
    if (!isPressing) {
      isPressing = true;
      pressStart = millis();
    }

    // hold detection
    if (millis() - pressStart >= LONG_PRESS_MS) {
      toggleMode();
      isPressing = false;

      // guard chattering
      delay(500);
    }
    else {
      if (isPressing && readyForNext) {
        playSound();
        readyForNext = false;
        isPressing = false;
      }
    }
  }
}

void toggleMode() {
  if (playMode == NORMAL) {
    playMode = MELODY;
    melodyIdx = 0;
    Serial.println("to MELODY MODE");
  }
  else {
    playMode = NORMAL;
    Serial.println("NORMAL MODE");
  }
}

bool checkTouch() {
  static int cnt = 0;

  int pres_data = analogRead(A0);
  Serial.println(pres_data);

  if (pres_data > THRESHOLD) {
    cnt++;
  } else {
    cnt = 0;
  }

  if (cnt > 1) {
    cnt = 0;
    return true;
  }

  return false;
}

void playSound() {
  if (playMode == NORMAL) {
    if (player.currentTrack > TOTAL_TRACKS) {
      player.currentTrack = 1;
    }

    Serial.println("Play");
    mp3_player.playFromMP3Folder(player.currentTrack);
    delay(300);

    player.currentTrack++;
  }
  // MELODY MODE
  else {
    int track = 0;

    if (HARMONY_MODE) {
      track = harmony[melodyIdx] + 5;

      melodyIdx++;
      if (melodyIdx >= MELODY_LEN) {
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

  delay(300);
}

int readSensor() {
  return analogRead(A0);
}