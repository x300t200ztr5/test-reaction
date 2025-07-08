#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonPin = 2;
const int buzzerPin = 3;

// Настройка: если true — реакция <100мс считается "Too early"
bool minimumReactionEnabled = true;

unsigned long startTime;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // внутренний подтягивающий резистор
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Ready?");
  display.display();

  delay(2000);
}

void loop() {
  waitForStart();
  runReactionTest();
}

void waitForStart() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println("Press button to start");
  display.display();

  while (digitalRead(buttonPin) == HIGH);  // ждем нажатия
  delay(50);                               // антидребезг
  while (digitalRead(buttonPin) == LOW);  // ждем отпускания кнопки
  delay(100);
}

void runReactionTest() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("Wait...");
  display.display();

  delay(random(2000, 5000));  // Ждем 2-5 секунд

  bool fakeTap = random(0, 100) < 20;  // 20% шанс обманки

  if (fakeTap) {
    // Обманка: просто пик и пауза 2-3 секунды
    tone(buzzerPin, 1000, 100);
    delay(random(2000, 3000));
    // После паузы запускаем настоящий TAP
  }

  // Реальный TAP (после обманки или сразу)
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("TAP!");
  display.display();
  tone(buzzerPin, 1000, 100);
  startTime = millis();

  while (true) {
    if (digitalRead(buttonPin) == LOW) {
      delay(50);
      while (digitalRead(buttonPin) == LOW);

      unsigned long reactionTime = millis() - startTime;

      if (minimumReactionEnabled && reactionTime < 100) {
        showTooEarly();
      } else {
        showReaction(reactionTime);
      }
      return;
    }
  }
}

void showTooEarly() {
  tone(buzzerPin, 400, 300);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println("Too early!");
  display.display();
  delay(2000);
}

String getReactionComment(unsigned long time) {
  if (time < 120) return "Very fast";
  if (time < 160) return "Fast";
  if (time < 250) return "Normal";
  if (time < 400) return "Slow";
  return "Very slow";
}

void showReaction(unsigned long time) {
  tone(buzzerPin, 1500, 200);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Your reaction:");
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print(time);
  display.println(" ms");

  display.setTextSize(1);
  display.setCursor(10, 50);
  display.print(getReactionComment(time));
  display.display();
  delay(3000);
}
