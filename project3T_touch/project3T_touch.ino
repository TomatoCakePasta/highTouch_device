#include "SoftwareSerial.h"
#include "DFPlayer_Mini_Mp3.h"

SoftwareSerial mySerial(10, 11); // RX, TX

int currentTrack = 1;       // 現在の再生曲番号
const int TOTAL_TRACKS = 3; // 18; // SDカード内の曲数

long baseline = 0;
long t = 0;

int isPlaying = 0;

int prevT = 0;
int cnt = 0;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
  digitalWrite(8, LOW);

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

  // ベースライン取得（人Aが触れた状態）
  delay(2000);
  for (int i = 0; i < 50; i++) {
    baseline += readCap();
    delay(10);
  }
  baseline /= 50;

  Serial.begin(9600);
  Serial.print("baseline = ");
  Serial.println(baseline);
}

long readCap() {
  long t = 0;
  digitalWrite(8, LOW);
  delayMicroseconds(10);
  digitalWrite(8, HIGH);
  while (digitalRead(9) == LOW && t < 5000) t++;
  return t;
}

void loop() {
  t = readCap();
  long delta = t - baseline;

  Serial.print("t:");
  Serial.print(t);
  Serial.print(" delta:");
  Serial.println(delta);

  checkTrack(1, delta);

  // if (delta > 200) {   // ★ここが「他人が触れた」判定
  //   Serial.println("TOUCH BY OTHER");
  // }

  delay(10);
} 

void checkTrack(int n, long delta) {
  if ((prevT < n) && (delta < n)) {
    cnt++;
  }
  else {
    cnt = 0;
  }

  prevT = delta;

  // 人の手が触れた
  if (cnt > 7) {
    cnt = 0;

    if (currentTrack > TOTAL_TRACKS){
      currentTrack = 1; // 最後の曲なら最初に戻す
    }

    Serial.println("Play");
    mp3_play(currentTrack);
    delay(600);

    currentTrack++;
  }
}