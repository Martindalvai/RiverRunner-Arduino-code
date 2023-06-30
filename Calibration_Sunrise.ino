#include <Wire.h>
#include <Sunrise.h>

const int Pin_Enable = 8;                 // Digital Pin to enable Sunrise sensor
const uint8_t ADDRESS_SUNRISE = 0x69;     //I2C Address of Sunrise Senseair
uint16_t Wait_For_Measurement_MS;         // Time the CO2 sensor needs for measurement
bool STATUS_SUNRISE = false;
uint16_t CO2;                             // Variable to save CO2 value
uint8_t calibration_status;
uint8_t SunriseState[24]; 

Sunrise sunrise(Pin_Enable);

void setup() {
  pinMode(Pin_Enable, OUTPUT);
  digitalWrite(Pin_Enable, HIGH);
  delay(STABILIZATION_MS);
  Serial.begin(9600);                       // Start serial communication
  Wire.begin();                             // Start I2C communication
  Serial.println("Sunrise Zero Calibration:");
  Init_Sunrise();
  if(STATUS_SUNRISE){
    Serial.println("Sensor found.");
  }else{
    Serial.println("ERROR: Sensor not found!");
    return;
  }

  // Disable ABC Calibration
  sunrise.setABC(ADDRESS_SUNRISE, false);

  // Clear Calibration Status
 sunrise.clearCalibrationStatus(ADDRESS_SUNRISE);
  
  // Write Calibration Command
  sunrise.startCalibration(ADDRESS_SUNRISE, CALIBRATION_ZERO);

  // Take Single Measurement
  delay(100);
  CO2 = sunrise.readCO2_single(SunriseState,ADDRESS_SUNRISE,Wait_For_Measurement_MS);
  delay(100);
  Serial.print("Read CO2 concentration: ");
  Serial.print(CO2);
  Serial.println("ppm");

  // Read Calibration Status to check it
  calibration_status = sunrise.readCalibrationStatus(ADDRESS_SUNRISE);
  Serial.print("Calibration Status: ");
  Serial.println(calibration_status,BIN);

  Serial.println("Calibration complete.");
  Serial.println("Make sure to remove the Calibration Program from the Arduino before powering it up again!");

  digitalWrite(Pin_Enable, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

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
