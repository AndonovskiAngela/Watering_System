#include <SPI.h>
#include <SD.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include "Sodaq_DS3231.h"
const int dry = 270;
int currentTime;
int timer;
const int pumpPin = 7;
const int soilSensor = A3;

//initializing objects of given classes
DateTime dt(2022, 10, 1, 0, 32, 0, 5);
LiquidCrystal_I2C lcd(0x27, 16, 2);
File myFile;
bool century = false; //flag for .getMonth()
bool h12Flag = false; //flag for .getHour()
bool pmFlag = false; //flag for .getHour()
char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char timeString[50]; //char array for printing the current time

byte decToBcd(byte val)                                                                 // Convert normal decimal numbers to binary coded decimal
{
  return ( (val / 10 * 16) + (val % 10) );                                              // Convert binary coded decimal to normal decimal numbers
}
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  pinMode(pumpPin, OUTPUT);
  pinMode(soilSensor, INPUT);
  rtc.begin();
  rtc.setDateTime(dt);
  digitalWrite(pumpPin, HIGH);
  delay(1000);
  Serial.begin(9600);
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

void loop() {
  int moisture = analogRead(soilSensor);
  DateTime now = rtc.now();
  myFile = SD.open("kvp.txt", FILE_WRITE);
  if (myFile) {
    Serial.println("SD CARD OKAY");
    snprintf(timeString, sizeof(timeString), "%02d:%02d  %02d/%02d/%d  %s", bcdToDec(now.hour()), bcdToDec(now.minute()), bcdToDec(now.date()), bcdToDec(now.month()), 2022, weekDay[bcdToDec(now.dayOfWeek())]);
    myFile.print(timeString);
    myFile.print(" Moisture: "); myFile.println(moisture, DEC);
    myFile.println();
    myFile.close();
  } else {
    Serial.println("SD CARD NOT OKAY");
  }
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(0, 0);
  lcd.print(bcdToDec(now.hour()));
  lcd.setCursor(2, 0);
  lcd.print(":");
  lcd.setCursor(3, 0);
  lcd.print(bcdToDec(now.minute()));
  if (moisture < 300) {
    digitalWrite(pumpPin, LOW);
    delay(2000);
    digitalWrite(pumpPin, HIGH);
    lcd.setCursor(14, 0);
    lcd.print(" ");
    lcd.setCursor(7, 0);
    lcd.print("Pump ON");
    lcd.setCursor(0, 1);
    lcd.print("Moisture : LOW");
  } else if (moisture > 300 && moisture < 950) {
    lcd.setCursor(0, 1);
    lcd.print("Moisture : MID ");
    digitalWrite(pumpPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("");
    lcd.setCursor(7, 0);
    lcd.print("Pump OFF");
  } else if (moisture > 950) {
    lcd.setCursor(0, 1);
    lcd.print("Moisture : HIGH ");
    digitalWrite(pumpPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("");
    lcd.setCursor(7, 0);
    lcd.print("Pump OFF");
  }
  delay(5000);
}
