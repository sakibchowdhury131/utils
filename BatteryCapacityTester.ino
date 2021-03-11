#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>


struct savedData {
  float voltage;
  float current;
  float capacity;
};


#define SCREEN_WIDTH      128 // OLED display width, in pixels
#define SCREEN_HEIGHT     32 // OLED display height, in pixels
#define voltagePin        A0
#define currentPin        A1
#define OLED_RESET        4
#define button            3
#define connectionThresh  3.4
#define dischargeThresh   3.7


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float RL = 13.2;
float Dvoltage = 0.0;
float Dcurrent = 0.0;
float Dcapacity = 0.0;
unsigned long previousMillis = 0; // Previous time in ms
unsigned long millisPassed = 0;  // Current time in ms
bool button_pressed = false;


void setup(){
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(button), eraseEEPROM, RISING);
  readEEPROM();


  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Battery Capacity Meter");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Using Last Saved Values...");
  display.display();
  delay(2000);
}

void loop(){
  calculate();
}

bool check_battery_availability() {
  float voltage = 0.0;
  for (uint8_t i = 0; i < 100; i++) {
    voltage += analogRead(voltagePin) * 5/1000; 
  }
  voltage /= 100;
  if (voltage > connectionThresh) {
    return true;
  } else {
    return false;
  }
}

bool check_battery_discharged() {
  float voltage = 0;
  for (uint8_t i = 0; i < 100; i++) {
    voltage += analogRead(voltagePin) * 5 /1000; 
  }
  voltage = voltage / 100;
  if (voltage < dischargeThresh && voltage > connectionThresh) {
    return true;
  } else {
    return false;
  }
}


void eraseEEPROM(){
  Serial.println("Erasing EEPROM...");
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM erased...");
  Dcapacity = 0;
  button_pressed = true; 
}

void readEEPROM(){
  Serial.println("Reading EEPROM...");
  int eepromStartAddress = 0;
  savedData storedData;
  EEPROM.get(eepromStartAddress, storedData);
  Dvoltage = storedData.voltage;
  Dcurrent = storedData.current;
  Dcapacity = storedData.capacity;
}

void save2EEPROM(savedData data){
  Serial.println("writing to EEPROM");
  int eepromStartAddress = 0;
  EEPROM.put(eepromStartAddress, data);
}


void calculate(){
  if (check_battery_availability()){
    if (!check_battery_discharged()){
      float voltage = 0;
      for (uint8_t i = 0; i<100; i++){
        voltage += analogRead(voltagePin)*5/1000;
      }
      millisPassed = millis() - previousMillis;
      Dvoltage = voltage/100;
      Dcurrent = Dvoltage*1000/RL;   //in mA
      Dcapacity = Dcapacity + Dcurrent * (millisPassed / 3600000.0); 
      previousMillis = millis();
      Serial.print("Voltage: "); Serial.print(Dvoltage); Serial.println(" V");
      Serial.print("Current: "); Serial.print(Dcurrent); Serial.println(" mA");
      Serial.print("Capacity: "); Serial.print(Dcapacity); Serial.println(" mAh");

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("Voltage: "); display.print(Dvoltage); display.println(" V");
      display.print("Current: "); display.print(Dcurrent); display.println(" mA");
      display.print("Capacity: "); display.print(Dcapacity); display.println(" mAh");
      display.display();
      delay (1000);
      savedData x;
      x.voltage = Dvoltage;
      x.current = Dcurrent;
      x.capacity = Dcapacity; 
      save2EEPROM(x);
    } else {
      Serial.println("Battery fully Discharged...");
      readEEPROM();
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Battery Fully Discharged");
      display.print("Total Capacity: "); display.print(Dcapacity); display.println(" mAh");
      display.display();
    }
  } else {
    Serial.println("no battery connected...");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("No Battery Connected...");
    display.display();
  }

  if (button_pressed){
    button_pressed = false;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("EEPROM Erased...");
    display.display();
    delay(1000);
  }
}