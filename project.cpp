#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int stcp = 3;
int shcp = 2;
int ds = 4;
unsigned long long leds;
byte segment1, segment2, segment3, segment4, segment5;
String chosenColor;
int wins = 0, losses = 0;

void setup() {
  pinMode(stcp, OUTPUT);
  pinMode(shcp, OUTPUT);
  pinMode(ds, OUTPUT);
  for (int i = 10; i <= 12; i++) {
    pinMode(i, INPUT);
  }
  lcd.begin(16,2);
  lcd.backlight();
  displayInfo();
  Serial.begin(9600);
  randomSeed(analogRead(0));
  turnOffLeds();
}

void loop() {
  if (checkChosenColor()) {
    spinRoulette();
  }
}

void spinRoulette() {

  displayChosenColor();

  int spinTime = random(3500, 7500); 
  long startTime = millis();
  long currentSpinTime = 0;
  int delayTime = 5;
  leds = 1;

  while (currentSpinTime < spinTime) {
    segment1 = (leds >> 0) & 0xFF;
    segment2 = (leds >> 8) & 0xFF;
    segment3 = (leds >> 16) & 0xFF;
    segment4 = (leds >> 24) & 0xFF;
    segment5 = (leds >> 32) & 0x1F;

    digitalWrite(stcp, LOW);
    shiftOut(ds, shcp, MSBFIRST, segment5);
    shiftOut(ds, shcp, MSBFIRST, segment4);
    shiftOut(ds, shcp, MSBFIRST, segment3);
    shiftOut(ds, shcp, MSBFIRST, segment2);
    shiftOut(ds, shcp, MSBFIRST, segment1);
    digitalWrite(stcp, HIGH);

    leds <<= 1;
    if (leds & (1ULL << 37)) leds = 1; 

    delay(delayTime);
    currentSpinTime = millis() - startTime;

    int randomDiv = random(3) + 2;

    if (currentSpinTime > spinTime / randomDiv) {
      delayTime += 1; 
    }
  }

  int ledOnNum[] = {segment1, segment2, segment3, segment4, segment5};

  checkWinner(checkRouletteColor(ledOnNum));
  displayPoints();
  displayInfo();
}

String checkRouletteColor(int t[]) {
  String color;
  for (int i = 0; i < 5; i++) {
    if (t[i] != 0) {
      if (t[i] == 2 || t[i] == 8 || t[i] == 32 || t[i] == 128) color = "Red";
      else if (t[i] == 1 && i == 0) color = "Green";
      else color = "Yellow";
      break;
    }
  }
  return color;
}

void displayInfo() {
  lcd.setCursor(0,0);
  lcd.print("Choose the color");
  lcd.setCursor(0, 1);
  lcd.print("Yellow/Red/Green");
}

boolean isBtnPressed(int pin) {
  if (digitalRead(pin) == HIGH) {
    delay(30);
    if (digitalRead(pin) == HIGH) {
      return true;
    }
  }
  return false;
}

boolean checkChosenColor() {
  if (isBtnPressed(12)) {
    chosenColor = "Yellow";
    return true;
  } else if (isBtnPressed(11)) {
    chosenColor = "Red";
    return true;
  } else if (isBtnPressed(10)) {
    chosenColor = "Green";
    return true;
  } 
  return false;
}

void displayChosenColor() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Chosen Color:");
  lcd.setCursor(0, 1);
  lcd.print(chosenColor);
}

void turnOffLeds() {
  digitalWrite(stcp, LOW);
  for (int i = 0; i < 5; i++) {
    shiftOut(ds, shcp, MSBFIRST, 0b00000000);
  }
  digitalWrite(stcp, HIGH);
}

void checkWinner(String rouletteColor){
  if (rouletteColor.equals(chosenColor)) wins++;
  else losses++;
}

void displayPoints() {a
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wins = " + String(wins));
  lcd.setCursor(0, 1);
  lcd.print("Losses = " + String(losses));
  delay(3000);
}
