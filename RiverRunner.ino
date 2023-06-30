/*
Continous measurements: CO2, CH4, RH, T, p
Save to SD Card with Date-Named .txt. File
Sunrise: GND: GND, COMSEL; VCC: VBB, VDDIO; D8: EN; SDA, SCL 10kO pullup resistor to VCC
TGS2611 on analog in
DS3231 & BME280 on I2C
SD on SPI communication: MOSI - pin 11; MISO - pin 12; CLK - pin 13; CS - pin 10
LED blinks if initialisation OK & during measurements


Martin Dalvai
16.11,2020
 */
 
// include libraries
#include <Wire.h>
#include <Sunrise.h>
#include <DS3231.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//  I2C Addresses
const uint8_t ADDRESS_SUNRISE = 0x69;    
const uint8_t ADDRESS_DS3231  = 0x68;
const uint8_t ADDRESS_BME280  = 0x76;  

// Status Flags of modules:
boolean STATUS_SUNRISE = false;           // false = not okay, true = okay
boolean STATUS_DS3231 = false;
boolean STATUS_SD = false;
boolean STATUS_BME280 = false;             

// PIN definition
const int Pin_Enable = 8;                 // Digital Pin to enable Sunrise sensor
#define StatusLED 6
#define TGS A3

// define variables
const float Ardu_V = 3.3 ;            // acctual Volt on Arduino 5V pin
int Ardu_c = 1024.0;    // resolution of ADC of Arduino
float TGS_V = 0;
float TGS_c = 0; 

//sunrise
uint16_t Wait_For_Measurement_MS;         // Time the CO2 sensor needs for measurement
uint16_t CO2;                             // Variable to save CO2 value
uint8_t calibration_status;
uint8_t SunriseState[24];                 // Array to save previous CO2 measurement

Sunrise sunrise(Pin_Enable);

// bme280
Adafruit_BME280 bme280;

// DS3231
DateTime timestamp;  
RTClib RTClock;

// SD
char SDFileName[] = "20yymmdd.txt";
char SDFileSetting[] = "settings.txt";          // Name of file on SD card with settings
File SDCardFile; 

// delay between measurements
int Delay = 5000; 

//********************************************************
void setup() {
  Serial.begin(9600);                       // Start serial communication
  Wire.begin();  

  pinMode(StatusLED, OUTPUT);
  digitalWrite(StatusLED, LOW); 
  pinMode(Pin_Enable, OUTPUT);
  digitalWrite(Pin_Enable, HIGH);
  delay(STABILIZATION_MS);
     
  Serial.println("Initialising...");
  Init_Sunrise();
  Init_DS3231();
  Init_SD();
  Init_BME280();
  Load_Settings();

  start_routine();
}

//********************************************************
void loop() {
   if(STATUS_SUNRISE && STATUS_DS3231 && STATUS_BME280 && STATUS_SD){
     digitalWrite(StatusLED, LOW);
       delay(500);
      }else{
        Init_DS3231();
        Init_SD();
        Init_BME280();
        Init_Sunrise();
    digitalWrite(StatusLED, LOW);
  }
  
  timestamp = RTClock.now();
  CO2 = sunrise.readCO2_single(SunriseState,ADDRESS_SUNRISE,Wait_For_Measurement_MS);
  delay(100);
  tgs();
  SD_Save();

  PrintSerial();
  delay(Delay);
}

//********************************************************
/***********************
* Set time and date properties of files*
*  written to the SD card              *
**********************/
void dateTime(uint16_t* date, uint16_t* time){
  if (timestamp.second() >=0 && timestamp.second() <= 60){
 
  *date = FAT_DATE(timestamp.year(),timestamp.month(),timestamp.day());  
  *time = FAT_TIME(timestamp.hour(),timestamp.minute(),timestamp.second());
} }
/*******************
* Initialize DS3231*
*******************/
void Init_DS3231() {
  timestamp = RTClock.now();
  if (timestamp.second() >=0 && timestamp.second() <= 60){
    STATUS_DS3231 = true;
  } else {
    STATUS_DS3231 = false;
  }
}
/*******************
* Initialize BME280*
*******************/
void Init_BME280() {
  if (bme280.begin(ADDRESS_BME280)) {
    STATUS_BME280 = true;
  } else {
    STATUS_BME280 = false;
  }
}
/*******************
* Sunrise CO2 Sensor*
*******************/
void Init_Sunrise(){
  STATUS_SUNRISE = sunrise.status(ADDRESS_SUNRISE); // Check connection
  if(STATUS_SUNRISE){
    delay(200);
    uint16_t numSamples = sunrise.readNumberSamples(ADDRESS_SUNRISE); //Read number of samples takes at each measurement
    if(numSamples==0){ // Error reading number of samples
      STATUS_SUNRISE = false;
      return; 
    }
    Wait_For_Measurement_MS = 200*numSamples; // Takes 200ms per samplep
  }
}
/********************
* Initialize SD card*
********************/
void Init_SD(){
  if(SD.begin()){
    STATUS_SD = true;
    SdFile::dateTimeCallback(dateTime); // Set time date of last SD card access
  } else {
    STATUS_SD = false;
  }
}
/*******************
* Starting routine*
*******************/
void start_routine(){
   if(STATUS_SUNRISE && STATUS_DS3231 && STATUS_BME280 && STATUS_SD){
    Serial.println("Status ok");
    Serial.println("Initialization done");
    for (int i=0; i<3; i++){
      digitalWrite(StatusLED, HIGH);
      delay(500);
      digitalWrite(StatusLED, LOW);
      delay(500);
      }
  }else{
    Serial.print("Sunrise:  ");  Serial.println(STATUS_SUNRISE);
    Serial.print("RTC    :  ");  Serial.println(STATUS_DS3231);
    Serial.print("SD     :  ");  Serial.println(STATUS_SD);
    Serial.print("BME280 :  ");  Serial.println(STATUS_BME280);
    return;
  }  
}
/*******************
* Calculate values from TGS2611*
*******************/
void tgs()  {
  TGS_c = analogRead(TGS);        // counts read by the ADC
  TGS_V =  TGS_c * Ardu_V / Ardu_c;    // Volts read into Arduino after Sensor  
}
/**********************************
* Save measurements to the SD card*
**********************************/
void SD_Save(){
  if (!STATUS_SD){ // Error access SD card
    return;
  }
  SDFileName[2] = (timestamp.year()-2000)/10+48;
  SDFileName[3] = (timestamp.year()-2000)%10+48;
  SDFileName[4] = timestamp.month()/10+48;
  SDFileName[5] = timestamp.month()%10+48;
  SDFileName[6] = timestamp.day()/10+48;
  SDFileName[7] = timestamp.day()%10+48;
  SDCardFile = SD.open(SDFileName, FILE_WRITE);
  if (SDCardFile) {  
  // Time Print
  SDCardFile.print(timestamp.day());                  SDCardFile.print("-");
  SDCardFile.print(timestamp.month());                SDCardFile.print("-");
  SDCardFile.print(timestamp.year());                 SDCardFile.print(" ");
  SDCardFile.print(timestamp.hour());                 SDCardFile.print(":");
  SDCardFile.print(timestamp.minute());               SDCardFile.print(":");
  SDCardFile.print(timestamp.second());               SDCardFile.print(",");
  SDCardFile.print(timestamp.unixtime());             SDCardFile.print(",");

   // Measurement print
  SDCardFile.print(CO2);                              SDCardFile.print(",");
  SDCardFile.print(TGS_c);                            SDCardFile.print(",");
  SDCardFile.print(TGS_V);                            SDCardFile.print(",");
  SDCardFile.print(bme280.readTemperature());         SDCardFile.print(",");
  SDCardFile.print(bme280.readPressure()/ 100000);    SDCardFile.print(",");
  SDCardFile.println(bme280.readHumidity());
  
  SDCardFile.close();
  }else{ //error opening file
    STATUS_SD = false;
    Init_SD();
  }
}
/**********************************
* Print measurements to Serial monitor*
**********************************/
void PrintSerial(){
  // Time print
  Serial.print(timestamp.day());                  Serial.print("-");
  Serial.print(timestamp.month());                Serial.print("-");
  Serial.print(timestamp.year());                 Serial.print(" ");
  Serial.print(timestamp.hour());                 Serial.print(":");
  Serial.print(timestamp.minute());               Serial.print(":");
  Serial.print(timestamp.second());               Serial.print(",");
  Serial.print(timestamp.unixtime());             Serial.print(",");

  // Measurement print
  Serial.print(CO2);                              Serial.print(",");
  Serial.print(TGS_c);                            Serial.print(",");
  Serial.print(TGS_V);                            Serial.print(",");
  Serial.print(bme280.readTemperature());         Serial.print(",");
  Serial.print(bme280.readPressure()/ 100000);    Serial.print(",");
  Serial.println(bme280.readHumidity());
  }

 /********************************
* Read settings from the SD card*
********************************/
void Load_Settings(){
  if (!STATUS_SD){ // Error access SD card
    return;
  }
  SDCardFile = SD.open(SDFileSetting);
  if (SDCardFile) {
    String serialString="";
    while (SDCardFile.available()) {
      char aChar = SDCardFile.read();
      if (aChar == '\n' or aChar == '\r') break;
      serialString += aChar;
    }
    char SerialChars[serialString.length()+1];    
    serialString.toCharArray(SerialChars, serialString.length()+1);
    if (sscanf(SerialChars,"%d",&Delay) == 0)  {
      STATUS_SD = false;
    }
    SDCardFile.close();
  }else{ //error opening file
    STATUS_SD = false;
  }
}
