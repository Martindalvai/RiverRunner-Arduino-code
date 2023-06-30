#include <Wire.h>
#include <Sunrise.h>

const int Pin_Enable = 8;

/* I2C Address and the new one if it's going to be changeds*/
const uint8_t   SUNRISE_ADDR            = 0x68;
const uint8_t   SUNRISE_ADDR_NEW        = 0x69;

// Two different measurement modes
const uint16_t CONTINUOUS              = 0x0000;
const uint16_t SINGLE                  = 0x0001;


uint16_t Number_Samples = 8; // Number of samples per measurement
uint16_t Period = 16; //  Measurement period in seconds
int readPeriodMs = Number_Samples*200; // One sample takes 200ms

Sunrise sunrise(Pin_Enable);

void setup() {
  pinMode(Pin_Enable, OUTPUT);
  digitalWrite(Pin_Enable, HIGH);
  delay(STABILIZATION_MS);
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Current Sensor Measurement Configurations:");
  read_sensor_config(SUNRISE_ADDR); // read current settings
  delay(100);
  sunrise.setMeterControl(SUNRISE_ADDR, 0x10); // set meter control, more info in the Sunrise I2C document 
  delay(100);
  sunrise.setSampleNumber(SUNRISE_ADDR, Number_Samples); // Set number of samples used
  delay(100);
  sunrise.setMeasPeriod(SUNRISE_ADDR, Period); // Set measurement period (continuous mode) 
  delay(100);
  sunrise.setABC(SUNRISE_ADDR, false); // Enable/Disable ABC
  delay(100);
  sunrise.setMode(SUNRISE_ADDR, SINGLE); // Set measuremet mode
  delay(100);
  sunrise.setNewAddress(SUNRISE_ADDR, SUNRISE_ADDR_NEW); // Change I2C Address
  Serial.println();
  digitalWrite(Pin_Enable, LOW);
  delay(STABILIZATION_MS);
}

void loop() {
  // put your main code here, to run repeatedly:
}


void read_sensor_config(uint8_t address) {
  uint8_t measMode = sunrise.readMode(address);
  delay(100);
  uint16_t measPeriod = sunrise.readMeasPeriod(address);
  delay(100);
  uint16_t numSamples = sunrise.readNumberSamples(address);
  delay(100);
  uint16_t abcPeriod = sunrise.readABCPeriod(address);
  delay(100);
  uint8_t  meterControl = sunrise.readMeterControl(address);

  Serial.print("Measurement Mode: ");
  Serial.println(measMode);

  Serial.print("Measurement Period, sec: ");
  Serial.println(measPeriod);

  Serial.print("Number of Samples: ");
  Serial.println(numSamples);
  
  if((0U == abcPeriod) ||  (0xFFFFU == abcPeriod) || (meterControl & 0x02U)) {
    Serial.println("ABCPeriod: disabled");
  } else {
    Serial.print("ABCPeriod, hours: ");
    Serial.println(abcPeriod);  
  }  
  
  Serial.print("MeterControl: ");
  Serial.println(meterControl, HEX); 
}
