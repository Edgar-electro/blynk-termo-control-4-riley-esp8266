  /* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN ""




// Your WiFi credentials.
// Set password to "" for open networks.
const char* ssid = "androidAP";
const char* password = "12345678";

unsigned long previousMillis = 5000;
unsigned long interval = 5000;


#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Preferences.h>
#include <AceButton.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>     
using namespace ace_button;
Preferences pref;

#define DHTPIN           2//D23  pin connected with DHT

//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




#define LDR_PIN   A0
#define RelayPin1 5  //D1
#define RelayPin2 4  //D2
#define RelayPin3 14
#define RelayPin4 12

#define ButtonPin1 10 //D25
#define ButtonPin2  9  //D26 
#define ButtonPin3 13  //D27
#define ButtonPin4  3
#define ButtonPin5  1
#define wifiLed   16   //D2

//Change the virtual pins according the rooms
#define VPIN_Text           V0
#define VPIN_Mode           V1
#define VPIN_currentTemp    V2
#define VPIN_currentHumi    V3
#define VPIN_setTemp        V4
#define VPIN_setHumi        V5
#define VPIN_Heater         V6
#define VPIN_Humidifier     V7
#define VPIN_LuxLight       V8
#define VPIN_currentLDR     V9
#define VPIN_Light     V10

float setTemp = 0;
float setHumi = 0;
float currentTemp = 0;
float currentHumi = 0;
float ldrVal = 0;



// Relay and Mode State
bool heaterState = LOW; //Define integer to remember the toggle state for heater
bool humidifierState = LOW; //Define integer to remember the toggle state for Humidifier
bool modeState = LOW; //Define integer to remember the mode
bool LuxLightState = LOW;
bool LightState = LOW;
int wifiFlag = 0;

const int maxLight = 700;
const int minLight = 200;







 






DHT dht(DHTPIN, DHTTYPE);
char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;





ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);
ButtonConfig config5;
AceButton button5(&config5);






void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);
void handleEvent5(AceButton*, uint8_t, uint8_t);




// When App button is pushed - switch the state






BLYNK_WRITE(VPIN_Heater) {
  heaterState = param.asInt();
  digitalWrite(RelayPin1, !heaterState);
  pref.putBool("Heater", heaterState);
}

BLYNK_WRITE(VPIN_Humidifier) {
  humidifierState = param.asInt();
  digitalWrite(RelayPin2, !humidifierState);
  pref.putBool("Humidifier", humidifierState);
}

BLYNK_WRITE(VPIN_LuxLight) {
  LuxLightState = param.asInt();
  digitalWrite(RelayPin3, !LuxLightState);
  pref.putBool("LuxLight", LuxLightState);
}


BLYNK_WRITE(VPIN_Light) {
  LightState = param.asInt();
  digitalWrite(RelayPin4, !LightState);
  pref.putBool("Light", LightState);
}

BLYNK_WRITE(VPIN_Mode) {
  modeState = param.asInt();
  pref.putBool("Mode", modeState);
}

BLYNK_WRITE(VPIN_setTemp) {
  setTemp = param.asFloat();
  pref.putBool("setemp", setTemp);
}

BLYNK_WRITE(VPIN_setHumi) {
  setHumi = param.asFloat();
  pref.putBool("Humidity", setHumi);
}





void checkBlynkStatus() { // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 2);
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 2);
    display.println(" Blynk IoT Connected ");
    digitalWrite(wifiLed, HIGH);
    Blynk.virtualWrite(VPIN_Text, "IoT Temperature & Humidity Controller");
  }
  display.display();
  delay(1000);
}

BLYNK_CONNECTED() {
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_Text, "IoT Temperature & Humidity Controller");
  Blynk.virtualWrite(VPIN_Mode, modeState);
  Blynk.syncVirtual(VPIN_currentTemp);
  Blynk.syncVirtual(VPIN_currentHumi);
  Blynk.syncVirtual (VPIN_currentLDR);
  Blynk.syncVirtual(VPIN_setTemp);
  Blynk.syncVirtual(VPIN_setHumi);
  Blynk.virtualWrite(VPIN_Heater, heaterState);
  Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
  Blynk.virtualWrite( VPIN_LuxLight, LuxLightState);
  Blynk.virtualWrite( VPIN_Light, LightState); 
     
}

void setup()
{
  
  
   
  

  
  
  
  Serial.begin(115200);
  //Open namespace in read-write mode
  pref.begin("Relay_State", false);

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(wifiLed, OUTPUT);

  pinMode(ButtonPin1, INPUT_PULLUP);
  pinMode(ButtonPin2, INPUT_PULLUP);
  pinMode(ButtonPin3, INPUT_PULLUP);
  pinMode(ButtonPin4, INPUT_PULLUP);
  pinMode(ButtonPin5, INPUT_PULLUP);
  
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !heaterState);
  digitalWrite(RelayPin2, !humidifierState);
  digitalWrite(RelayPin3, !LuxLightState);
  digitalWrite(RelayPin4, !LightState);

  
  dht.begin();    // Enabling DHT sensor
  digitalWrite(wifiLed, LOW);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);
  
  
  button1.init(ButtonPin1);
  button2.init(ButtonPin2);
  button3.init(ButtonPin3);
  button4.init(ButtonPin4);
  
  
 Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    delay(5000);{
    WiFiManager wifiManager;
    wifiManager.autoConnect("BLYNK-IOT");
    Serial.println(WiFi.localIP());  
    }
   WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  //Blynk.begin(auth, ssid, pass);
 
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  timer.setInterval(1000L, sendSensor); // Sending Sensor Data to Blynk Cloud every 1 second
  Blynk.config(auth);
  delay(1000);

  getRelayState(); // Get the last state of Relays and Set values of Temp & Humidity

  Blynk.virtualWrite(VPIN_Heater, heaterState);
  Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
  Blynk.virtualWrite(VPIN_LuxLight, LuxLightState);
  Blynk.virtualWrite(VPIN_Light, LightState); 
  Blynk.virtualWrite(VPIN_setTemp, setTemp);
  Blynk.virtualWrite(VPIN_setHumi, setHumi);
  
 
     


  
}


void readSensor() {
  ldrVal = analogRead(LDR_PIN);
  Serial.print("LDR - "); Serial.println(ldrVal);
  currentTemp = dht.readTemperature();
  currentHumi = dht.readHumidity();
  if (isnan(currentHumi) || isnan(currentTemp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void sendSensor()
{
  readSensor();
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(VPIN_Text, "IoT Temperature & Humidity Controller");
  Blynk.virtualWrite(VPIN_currentTemp, currentTemp);
  Blynk.virtualWrite(VPIN_currentHumi, currentHumi);
  Blynk.virtualWrite(VPIN_currentLDR, ldrVal);
  
}

void getRelayState()
{
  //Serial.println("reading data from NVS");
  modeState = pref.getBool("Mode", 0);
  Blynk.virtualWrite(VPIN_Mode, modeState);
  delay(200);
  heaterState = pref.getBool("Heater", 0);
  digitalWrite(RelayPin1, !heaterState);
  Blynk.virtualWrite(VPIN_Heater, heaterState);
  delay(200);
  humidifierState = pref.getBool("Humidifier", 0);
  digitalWrite(RelayPin2, !humidifierState);
  Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
  delay(200);
  LuxLightState = pref.getBool("Lux", 0);
  digitalWrite(RelayPin3, !LuxLightState);
  Blynk.virtualWrite(VPIN_LuxLight, LuxLightState);
  delay(200);
  LightState = pref.getBool("Light", 0);
  digitalWrite(RelayPin4, !LightState);
  Blynk.virtualWrite(VPIN_Light, LightState);
  delay(200); 
  
  setTemp = pref.getBool("setemp", 0);
  Blynk.virtualWrite(VPIN_setTemp, setTemp);
  delay(200);
  setHumi = pref.getBool("Humidity", 0);
  Blynk.virtualWrite(VPIN_setHumi, setHumi);
  delay(200);
}


void DisplayData()  {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 7);
  display.println("----------");
  display.setCursor(2, 20);
  display.print(int(currentTemp));
  display.print((char)247);
  display.println("C");
  display.setCursor(60, 20);
  display.print(": ");
  display.print(int(currentHumi));
  display.println("%");
  display.setCursor(0, 34);
  display.println("----------");

  display.setTextSize(1);
  display.setCursor(0, 57);
  display.print("sTemp:");
  display.print(int(setTemp));
  display.print((char)247);
  display.println("C");
  display.setTextSize(1);
  display.setCursor(67, 57);
  display.print("sHumi:");
  display.print(int(setHumi));
  display.println("%");


  if (modeState == 1) {
    display.setTextSize(1);
    display.setCursor(20, 45);
    display.print("Automatic Mode");
    if (currentTemp < setTemp) {
      heaterState = 1;
      digitalWrite(RelayPin1, !heaterState);
      pref.putBool("Heater", heaterState);
      Serial.println("Heater ON");
      Blynk.virtualWrite(VPIN_Heater, heaterState);
    } else {
      heaterState = 0;
      digitalWrite(RelayPin1, !heaterState);
      pref.putBool("Heater", heaterState);
      Serial.println("Heater OFF");
      Blynk.virtualWrite(VPIN_Heater, heaterState);
    } if (currentHumi < setHumi) {
      humidifierState = 1;
      digitalWrite(RelayPin2, !humidifierState);
      Serial.println("Humidifier ON");
      pref.putBool("Humidifier", humidifierState);
      Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
    } else {
      humidifierState = 0;
      digitalWrite(RelayPin2, !humidifierState);
      Serial.println("Humidifier OFF");
      pref.putBool("Humidifier", humidifierState);
      Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
    }
  } else {
    display.setTextSize(1);
    display.setCursor(32, 45);
    display.print("Manual Mode");
  }
  display.display();
}


void loop()
{





  
   //LDR control Relay 2
      if( ldrVal < minLight){
        if(LuxLightState == 0){
          digitalWrite(RelayPin3,LOW ); // turn ON relay 2
          LuxLightState = 1;
          // Update Button Widget
          Blynk.virtualWrite(VPIN_LuxLight, LuxLightState);
        }
      }
      else if (ldrVal > maxLight){
        if(LuxLightState == 1){
           digitalWrite(RelayPin3,HIGH ); // turn OFF relay 2
           LuxLightState = 0;
           // Update Button Widget
           Blynk.virtualWrite(VPIN_LuxLight, LuxLightState);
          }
      } 
    
  
  
  Blynk.run();
  timer.run();
  DisplayData();
  button1.check();
  button2.check();
  button3.check();
  button4.check();
  button5.check();

}
void button5Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Mode");
  switch (eventType) {
    case AceButton::kEventReleased:
      modeState = !modeState;
      pref.putBool("Mode", modeState);
      Blynk.virtualWrite(VPIN_Mode, modeState);
      break;
  }
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Heater");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin1, heaterState);
      heaterState = !heaterState;
      pref.putBool("Heater", heaterState);
      Blynk.virtualWrite(VPIN_Heater, heaterState);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Humidifier");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin2, humidifierState);
      humidifierState = !humidifierState;
      pref.putBool("VPIN_Humidifier", humidifierState);
      Blynk.virtualWrite(VPIN_Humidifier, humidifierState);
      break;
  }
}


void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("LUXLight");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin3, LuxLightState);
      LuxLightState = !LuxLightState;
      pref.putBool("VPIN_LuxLight", LuxLightState);
      Blynk.virtualWrite(VPIN_LuxLight, LuxLightState);
      break;
  }
}

void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("Light");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin4, LightState);
      LightState = !LightState;
      pref.putBool("VPIN_Light", LightState);
      Blynk.virtualWrite(VPIN_Light, LightState);
      break;
  }
}
