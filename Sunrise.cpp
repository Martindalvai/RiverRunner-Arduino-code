#include <Sunrise.h>
#include <Arduino.h>
#include <Wire.h>


// Constructor
Sunrise::Sunrise(int PIN_ENABLE) { // Digital Pin to enable sensor
  _PIN_ENABLE = PIN_ENABLE;
}

void Sunrise::setNewAddress(uint8_t address, uint8_t ADDRESS_NEW){
  int error;
  int numBytes = 1;
  if (ADDRESS_NEW > 0x7F || ADDRESS_NEW<0x01){ //Address out of range
	  return;
  }
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_I2C_ADDRESS, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return;
    }
  if (Wire.read()==address){ // Nothing to do, sensor has same address
    SunriseDisable();
	  return;
  } else {
      if(!(wakeup(address))) {
        SunriseDisable();
		    return;
      }
      Wire.beginTransmission(address);
      Wire.write(SUNRISE_REGISTER_I2C_ADDRESS);
      Wire.write(ADDRESS_NEW);
      error = Wire.endTransmission(true);
      delay(EEPROM_UPDATE_DELAY_MS);
    
      if(error != 0) {
        SunriseDisable();
		    return;
      }
  }
  SunriseDisable();
  return;
}

void Sunrise::setMode(uint8_t address,uint16_t mode){
  int error;
  int numBytes = 1;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_MEASUREMENT_MODE, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return;
    }
  error = Wire.read(); // Contains current mode
  if (error==mode){ // Nothing to do, sensor is already in the right mode
    SunriseDisable();
	  return;
  } else if(mode == 0x0000){ // Set mode to continuous
      if(!(wakeup(address))) {
        SunriseDisable();
		    return;
      }
      Wire.beginTransmission(address);
      Wire.write(SUNRISE_REGISTER_MEASUREMENT_MODE);
      Wire.write(mode);
      error = Wire.endTransmission(true);
      delay(EEPROM_UPDATE_DELAY_MS);
    
      if(error != 0) {
        SunriseDisable();
		    return;
      }
  } else { // Set mode to single
      if(!wakeup(address)) {
        SunriseDisable();
		    return;
      }
      Wire.beginTransmission(address);
      Wire.write(SUNRISE_REGISTER_MEASUREMENT_MODE);
      Wire.write(mode);
      error = Wire.endTransmission(true);
      delay(EEPROM_UPDATE_DELAY_MS);
    
      if(error != 0) {
        SunriseDisable();
        return;
      }
  }
  SunriseDisable();
  return;
}
  
void Sunrise::setSampleNumber(uint8_t address,uint16_t NumberSamples){
  int error;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  error = SunriseWireRequest(address, 2, SUNRISE_REGISTER_NUMBER_SAMPLES, true);
  if(error != 2 ) {
    SunriseDisable();
    return;
  }
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t N = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo; // Current sample number
  if(N==NumberSamples){
    SunriseDisable();
	  return;
  }
  byteHi = (NumberSamples >> 8); // high byte
  byteLo = (NumberSamples & 0xff); //low byte
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_NUMBER_SAMPLES);
  Wire.write(byteHi);
  Wire.write(byteLo);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

void Sunrise::setABC(uint8_t address, bool enable){
  /* Wakeup */
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }

  /* Request values */
  int error = SunriseWireRequest(address, 1, SUNRISE_REGISTER_METER_CONTROL /* from address*/, true /* STOP*/);    
  if(error != 1 ) {
    SunriseDisable();
    return;
  }

  uint8_t  meterControl = Wire.read();

  if(enable) {
    Serial.println("Enabling ABC...");
    meterControl &= (uint8_t)~0x02U;
  } else {
    Serial.println("Disabling ABC...");
    meterControl |= 0x02U;
  }

  /* Wakeup */
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_METER_CONTROL);
  Wire.write(meterControl);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

void Sunrise::setIIR(uint8_t address, bool enable){
   /* Wakeup */
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }

  /* Request values */
  int error = SunriseWireRequest(address, 1, SUNRISE_REGISTER_METER_CONTROL /* from address*/, true /* STOP*/);    
  if(error != 1 ) {
    SunriseDisable();
    return;
  }

  uint8_t  meterControl = Wire.read();

  if(enable) {
    Serial.println("Enabling IIR...");
    meterControl &= (uint8_t)~0x0CU; //Set Bit 3/4 to 0
  } else {
    Serial.println("Disabling IIR...");
    meterControl |= 0x0CU;
  }

  /* Wakeup */
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_METER_CONTROL);
  Wire.write(meterControl);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

void Sunrise::setMeasPeriod(uint8_t address, uint16_t period){
  int error;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  error = SunriseWireRequest(address, 2, SUNRISE_REGISTER_MEASUREMENT_PERIOD, true);
  if(error != 2 ) {
    SunriseDisable();
    return;
  }
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t N = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo; // Current sample number
  if(N==period){
    SunriseDisable();
	  return;
  }
  byteHi = (period >> 8); // high byte
  byteLo = (period & 0xff); //low byte
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_MEASUREMENT_PERIOD);
  Wire.write(byteHi);
  Wire.write(byteLo);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

void Sunrise::setMeterControl(uint8_t address, uint8_t status){
  int error;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_METER_CONTROL);
  Wire.write(status);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

void Sunrise::readStatus(uint8_t state[],uint8_t address){
  int error;
  int numRegState = 24;
  SunriseEnable();
  error = SunriseWireRequest(address, numRegState, SUNRISE_REGISTER_ABC_TIME, true /* STOP*/); 
  if(error != numRegState) {
    SunriseDisable();
    return;
  }
  
  for(int n = 0 ; n < numRegState ; n++) {
    state[n] = Wire.read();
  }
  SunriseDisable();
  return;
}

uint16_t Sunrise::readCO2_single(uint8_t state[], uint8_t address, uint16_t Wait_For_Measurement_MS){
  int error;
  int numRegRead = 7;
  int numRegState = 24;
  SunriseEnable();
  if(!wakeup(address)) {
    SunriseDisable();
    return 0;
  }

  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_ABC_TIME);
  for(int reg_n =0; reg_n < numRegState; reg_n++) {
    Wire.write(state[reg_n]);
  }
  error = Wire.endTransmission(true);
  if(error != 0) {
    SunriseDisable();
    return 1;
  }
  delay(EEPROM_UPDATE_DELAY_MS);
  if(!wakeup(address)) {
    SunriseDisable();
    return 2;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_START_MEASUREMENT);
  Wire.write(0x01); // begin measurement
  error = Wire.endTransmission(true);
  if(error != 0) {
    SunriseDisable();
    return 3;
  }
  delay(Wait_For_Measurement_MS);
  byte error_status = 0x80;
  if(!(wakeup(address))) {
    SunriseDisable();
    return 4;
  }
  while(error_status != 0x00){
  error = SunriseWireRequest(address, 1, SUNRISE_REGISTER_ERROR_STATUS, true);    
  if(error != 1 ) {  
    SunriseDisable();
    return 5;
  }
	error_status = Wire.read();
	error_status &= 0x80;
  }
  delay(EEPROM_UPDATE_DELAY_MS);
  if(!wakeup(address)) {
    SunriseDisable();
    return 6;
  }
  error = SunriseWireRequest(address, numRegRead, SUNRISE_REGISTER_ERROR_STATUS, true);    
  if(error != numRegRead ) {  
    SunriseDisable();
    return 7;
  }
  /* Read values */
  /* Error status */
  uint8_t eStatus = Wire.read();

  /* Reserved */
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();

  byteHi = Wire.read();
  byteLo = Wire.read();

  /* CO2 value */
  byteHi = Wire.read();
  byteLo = Wire.read();
  uint16_t co2Val = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  /* Wakeup */
  if(!wakeup(address)) {
    SunriseDisable();
    return 8;
  }

  /* Read sensor state data from 0xC4-0xDB and save it for next measurement */
  error = SunriseWireRequest(address, numRegState, SUNRISE_REGISTER_ABC_TIME, true /* STOP*/); 
  if(error != numRegState) {
    SunriseDisable();
    return 8;
  }
  
  for(int n = 0 ; n < numRegState ; n++) {
    state[n] = Wire.read();
  }
  SunriseDisable();
  return co2Val;
}

uint16_t Sunrise::readCO2_single_nofilter(uint8_t address, uint16_t Wait_For_Measurement_MS){
  int error;
  int numRegRead = 7;
  int numRegState = 24;
  SunriseEnable();
  if(!wakeup(address)) {
    SunriseDisable();
    return 0;
  }

  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_START_MEASUREMENT);
  Wire.write(0x01); // begin measurement
  error = Wire.endTransmission(true);
  if(error != 0) {
    SunriseDisable();
    return 3;
  }
  delay(Wait_For_Measurement_MS);
  byte error_status = 0x80;
  if(!(wakeup(address))) {
    SunriseDisable();
    return 4;
  }
  while(error_status != 0x00){
  error = SunriseWireRequest(address, 1, SUNRISE_REGISTER_ERROR_STATUS, true);    
  if(error != 1 ) {  
    SunriseDisable();
    return 5;
  }
	error_status = Wire.read();
	error_status &= 0x80;
  }
  delay(EEPROM_UPDATE_DELAY_MS);
  if(!wakeup(address)) {
    SunriseDisable();
    return 6;
  }
  error = SunriseWireRequest(address, numRegRead, SUNRISE_REGISTER_ERROR_STATUS, true);    
  if(error != numRegRead ) {  
    SunriseDisable();
    return 7;
  }
  /* Read values */
  /* Error status */
  uint8_t eStatus = Wire.read();

  /* Reserved */
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();

  byteHi = Wire.read();
  byteLo = Wire.read();

  /* CO2 value */
  byteHi = Wire.read();
  byteLo = Wire.read();
  uint16_t co2Val = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;
  
  SunriseDisable();
  return co2Val;
}






/*uint16_t Sunrise::readCO2_continuous(uint8_t address){
  int error;
  int numBytes = 7;

  if(!(wakeup(address))) {
    return 0;
  }

    error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_ERROR_STATUS, true);    
  if(error != numBytes ) {  
    return 0;
  }

  uint8_t eStatus = Wire.read();

  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  byteHi = Wire.read();
  byteLo = Wire.read();

  byteHi = Wire.read();
  byteLo = Wire.read();
  uint16_t co2Val = ((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo;

  return co2Val;

}*/

void Sunrise::clearCalibrationStatus(uint8_t address){
  uint8_t clear = 0x00;
  int error;
  /* Wakeup */
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }

  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_CALIBRATION_STATUS);
  Wire.write(clear);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

uint8_t Sunrise::readCalibrationStatus(uint8_t address){
  int error;
  uint8_t status;
  /* Wakeup */
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 0;
  }
  error = SunriseWireRequest(address, 1, SUNRISE_REGISTER_CALIBRATION_STATUS, true);    
  if(error != 1) {  
  SunriseDisable();
    return 0;
  }
  status = Wire.read();

  SunriseDisable();
  return status;
}

void Sunrise::startCalibration(uint8_t address, uint16_t mode){
  int error;
  uint8_t byteHi = (mode >> 8); // high byte
  uint8_t byteLo = (mode & 0xff); //low byte
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return;
  }
  Wire.beginTransmission(address);
  Wire.write(SUNRISE_REGISTER_CALIBRATION_COMMAND);
  Wire.write(byteHi);
  Wire.write(byteLo);
  error = Wire.endTransmission(true);
  delay(EEPROM_UPDATE_DELAY_MS);
  if(error != 0) {
    SunriseDisable();
    return;
  }
  SunriseDisable();
  return;
}

uint8_t Sunrise::readMode(uint8_t address){
  int error;
  int numBytes = 1;
  uint8_t mode;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 2;
  }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_MEASUREMENT_MODE, true);    
  if(error != numBytes ) {  
  SunriseDisable();
    return 2;
  }
  mode = Wire.read();
  SunriseDisable();
  return mode;
}

uint16_t Sunrise::readMeasPeriod(uint8_t address){
  int error;
  int numBytes = 2;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 0;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_MEASUREMENT_PERIOD, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return 0;
    }	
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t measPeriod = (((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo);
  SunriseDisable();
  return measPeriod;
}

uint16_t Sunrise::readABCPeriod(uint8_t address){
  int error;
  int numBytes = 2;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 0;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_ABC_PERIOD, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return 0;
    }	
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t period = (((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo);
  SunriseDisable();
  return period;
}

uint8_t Sunrise::readMeterControl (uint8_t address){
  int error;
  int numBytes = 1;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 0xFF;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_METER_CONTROL, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return 0xFF;
    }	
  uint8_t control = Wire.read();
  SunriseDisable();
  return control;
}

uint16_t Sunrise::readNumberSamples(uint8_t address){
  int error;
  int numBytes = 2;
  SunriseEnable();
  if(!(wakeup(address))) {
    SunriseDisable();
    return 0;
    }
  error = SunriseWireRequest(address, numBytes, SUNRISE_REGISTER_NUMBER_SAMPLES, true /* STOP*/);    
    if(error != numBytes ) {  
      SunriseDisable();
      return 0;
    }	
  uint8_t byteHi = Wire.read();
  uint8_t byteLo = Wire.read();
  uint16_t NSamples = (((int16_t)(int8_t) byteHi << 8) | (uint16_t)byteLo);
  SunriseDisable();
  return NSamples;
}

bool Sunrise::status(uint8_t address){
  SunriseDisable();
	if(wakeup(address)){
    SunriseDisable();
		return true;
	} else {
    SunriseDisable();
		return false;
	}
}

void Sunrise::SunriseEnable(){
  digitalWrite(_PIN_ENABLE, HIGH);
  delay(STABILIZATION_MS);
}

void Sunrise::SunriseDisable(){
  digitalWrite(_PIN_ENABLE, LOW);
  delay(STABILIZATION_MS);
}

bool Sunrise::wakeup(uint8_t address){
  int error;
  int attemps = 5;
  do {   
    Wire.beginTransmission(address);
    error = Wire.endTransmission(true);
  } while(((error != 0 /*success */) && (error != 2 /*Received NACK on transmit of address*/) && (error != 1 /* BUG in STM32 library*/)) && (--attemps > 0)); 
  /* STM32 driver can stack under some conditions */
  if(error == 4) {
    /* Reinitialize I2C*/
    Wire.begin();
    return false;
  } 
  return (attemps > 0);
}

int Sunrise::SunriseWireRequest(uint8_t sunr_addr, uint8_t bytes_numbers, uint8_t offset_to_read, bool stop){
  int error;
  Wire.beginTransmission(sunr_addr);
  Wire.write(offset_to_read); //starting register address, from which read data
  Wire.endTransmission(false);
  error = Wire.requestFrom(sunr_addr, bytes_numbers /* how many bytes */, (uint8_t)stop /* STOP*/);
  return error;
}



