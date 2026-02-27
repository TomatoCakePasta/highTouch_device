#include <DFPlayerMini_Fast.h>

#define PIN_MP3_RX D6  // DFPlayer TX → XIAO RX
#define PIN_MP3_TX D7   // DFPlayer RX ← XIAO TX

HardwareSerial mp3Serial(1);   // UART1を使用
DFPlayerMini_Fast mp3_player;

const int TOTAL_TRACKS = 3;
const int THRESHOLD = 600;

struct PlayerState {
  int currentTrack;
};

PlayerState player = {1};

// ========== IMPORTANT ===========
// CLOSE Serial Monitor
// OR You Can't use RX/TX pins

void setup() {
  Serial.begin(115200);
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

  mp3_player.volume(30);
  delay(1000);

  mp3_player.play(1);
}

void loop() {

  if (checkTouch()) {
    playSound();
  }
}

bool checkTouch() {
  static int cnt = 0;

  int pres_data = analogRead(A0);
  Serial.println(pres_data);

  if (pres_data < THRESHOLD) {
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
  if (player.currentTrack > TOTAL_TRACKS) {
    player.currentTrack = 1;
  }

  Serial.println("Play");
  mp3_player.play(player.currentTrack);
  delay(300);

  player.currentTrack++;
}