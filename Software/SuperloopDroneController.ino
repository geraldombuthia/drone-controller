#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Wire.h>
#include <WiFi.h>
#include <RH_ASK.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LiquidCrystal_I2C.h>

#define vx 34//left controller
#define vy 35
#define sw 26
#define vx1 36//right controller
#define vy1 39
#define sw1 25
#define eeSize 1
#define rx 5
#define tx 13
#define voltPin 32

const char *ssid = "Faiba4G";
const char  *password = "Lexus570";

float ref_voltage = 5.0;
float adc_voltage = 0.0;
float in_voltage = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
float curr = 0.0;

int adc_value = 0;
int vxVal = 0;
int vyVal = 0;
int vx1Val = 0;
int vy1Val = 0;
int id = 0;
int eval;
long cardSize;
bool swVal;
bool sw1Val;
bool activate = false;
String dataToLog = "";
String pathTxt = "";
String card = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600, 60000);
RH_ASK rf_driver(2000, rx, tx, 0);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String randomGen() {
  randomSeed(analogRead(27));
  eval = random(300);
  Serial.println("Name Generated");
  return String(eval);
}
void lcdPrint(String txt, int col, int row, bool clr, int delTime = 0, bool var = false) {
  if (clr) {
    lcd.clear();
  }
  lcd.setCursor(col, row);
  lcd.print("   ");
  lcd.setCursor(col, row);
  lcd.print(txt);
  delay(delTime);
}
void lcdBegin() {
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcdPrint("Drone Controller", 0, 0, true, 0);
  lcdPrint("Setting up...", 0, 1, false, 2000);
  lcdPrint("LCD ON", 0, 0, true, 2000);
  Serial.println("LCD initalised");
}
void pinInit() {
  pinMode(sw, INPUT_PULLUP);
  pinMode(sw1, INPUT_PULLUP);
  Serial.println("Pin Initialised");
}
void controls() {
  vxVal = map(analogRead(vx), 0, 4095, 0, 255);
  vyVal = map(analogRead(vy), 0, 4095, 0, 255);
  vx1Val = map(analogRead(vx1), 0, 4095, 0, 255);
  vy1Val = map(analogRead(vy1), 0, 4095, 0, 255);
  swVal = digitalRead(sw);
  sw1Val = digitalRead(sw1);
  lcdPrint(String(vxVal), 3, 0, false);
  lcdPrint(String(vyVal), 3, 1, false );
  lcdPrint(String(vx1Val), 9, 0, false);
  lcdPrint(String(vy1Val), 9, 1, false);
  lcdPrint(String(swVal), 15, 0, false);
  lcdPrint(String(sw1Val), 15, 1, false);
}
void sdCardMount() {
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");
    Serial.println("Restarting system");
    lcdPrint("Card Mount Fail", 0, 0, true, 2000);
    lcdPrint("Restarting ...", 0, 1, false, 1000);
    ESP.restart();
    activate = false;
  }
  Serial.println("SD card Mounted");
  activate = true;
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    lcdPrint("No SD card", 0, 0, true, 1000);
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    card = "MMC";
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    card = "SDSC";
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    card = "SDHC";
    Serial.println("SDHC");
  } else {
    card = "UNKNOWN";
    Serial.println("UNKNOWN");
  }
  lcdPrint("SD Type: " + card, 0, 0, true, 1000);
  cardSize = SD.cardSize() / (1024 * 1024);
  lcdPrint("SD size: " + String(cardSize) + String("MB"), 0, 1, false, 3000);
  Serial.printf("SD Card Size: %luMB\n", cardSize);
}
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  lcdPrint("File Path :", 0, 0, true, 1000);
  lcdPrint(pathTxt, 0, 1, false, 3000);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
void dataLog() {
  long tm = millis();
  dataToLog = String(id++) + "\t" + String(getTime()) + "\t" + String(in_voltage) + "\t" + String(curr) + "\t" + String(vxVal) + "\t" + String(vyVal) + "\t" + String(vx1Val) + "\t" + String(vy1Val) + "\t" + String(swVal) + "\t" + String(sw1Val) + "\t\r\n";
  appendFile(SD, pathTxt.c_str(), dataToLog.c_str());
}
void sendData() {
  String toSend = String(vxVal) + "," + String(vyVal) + "," + String(vx1Val) + "," + String(vy1Val) + "," + String(swVal) + "," + String(sw1Val);
  const char *msg = toSend.c_str();
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
}
void vSense() {
  adc_value = analogRead(32);
  adc_voltage  = (adc_value * ref_voltage) / 4096.0;
  in_voltage = (adc_voltage / (R2 / (R1 + R2))) * 0.6667;
  Serial.print("The voltage is: ");
  Serial.println(in_voltage);
}
void currSense() {
  unsigned int x = 0;
  float AcsValue = 0.0, Samples = 0.0, AvgAcs = 0.0, AcsValueF = 0.0;
  for (int x = 0; x < 5; x++) {
    AcsValue = analogRead(33);
    Samples = Samples + AcsValue;
    delay (3);
  }
  AvgAcs = Samples / 150.0; //Taking Average of Samples
  curr = (2.5 - (AvgAcs * (3.3 / 4096.0)) ) / 0.185;
  Serial.print("The current is: ");
  Serial.println(curr);
}
String getTime() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("The time is: ");
  Serial.println(formattedTime);
  return formattedTime;
}
String getDate() {
  timeClient.update();
  String formattedDate = timeClient.getFormattedDate();
  Serial.print("The date is: ");
  Serial.println(formattedDate.substring(0, formattedDate.indexOf("T")));
  return formattedDate.substring(0, formattedDate.indexOf("T"));
}
void setup()
{
  Serial.begin(115200);
  lcdBegin();
  pinInit();
  rf_driver.init();
  sdCardMount();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WIFI connected");
  lcdPrint("Wifi connected", 0, 0, true, 100);
  lcdPrint(ssid, 0, 1, false, 1000);
  timeClient.begin();
  timeClient.setTimeOffset(10800);
  lcdPrint("File Path", 0, 0, true, 100);
  lcdPrint(pathTxt, 0, 1, false, 1000);
  pathTxt = ("/" + String("data") + String(randomGen()) + String(".txt"));
  String flightTxt = String("Controller On.\nInitialized tests and completed successfully.\nMission details..........\n")
                     + "\tLCD Okay\n\tFilename >>>" + String(pathTxt) + "\n\tPins initialised Okay\n\tTransceiver Transmission Okay\n\tSD card Mounted Successfully. Type:"
                     + String(card) + "; Size:+" + String(cardSize) + "MB;\n\tConnected to Wifi >>>" + String(ssid) + " on " + String(getDate()) + " at " + String(getTime())
                     + " successfully\n\tGround Station System execution begins now\n\r\r\nId\tTime\t\tVoltage\tCurrent\tMemory\tLx\tLy\tRx\tRy\tLs\tRs\n"
                     + "...................................................................................................................................\n";
  delay(10);
  writeFile(SD, pathTxt.c_str(), flightTxt.c_str());
  lcdPrint("SD card write...", 0, 0, true, 100);
  lcdPrint("Successful", 0, 1, false, 2000);
  lcdPrint("Lx:", 0, 0, true);
  lcdPrint("Ly:", 0, 1, false);
  lcdPrint("Rx:", 6, 0, false);
  lcdPrint("Ry:", 6, 1, false);
  lcdPrint("Ls:", 12, 0, false);
  lcdPrint("Rs:", 12, 1, false);
  delay(1000);
}
void loop()
{
  getTime();
  vSense();
  currSense();
  controls();
  dataLog();
  sendData();
}
