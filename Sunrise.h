#ifndef __SUNRISE_H__
#define __SUNRISE_H__

#include <Arduino.h>
#include <Wire.h>

#define SUNRISE_ADDRESS         (0x68) // default I2C address

const int STABILIZATION_MS         = 35; // 35ms default
const int EEPROM_UPDATE_DELAY_MS   = 25; // 25ms default

//Register addresses
const uint8_t SUNRISE_REGISTER_ERROR_STATUS             = 0x01;
const uint8_t SUNRISE_REGISTER_CALIBRATION_COMMAND      = 0x82;
const uint8_t SUNRISE_REGISTER_MEASUREMENT_MODE         = 0x95;
const uint8_t SUNRISE_REGISTER_MEASUREMENT_PERIOD       = 0x96;
const uint8_t SUNRISE_REGISTER_NUMBER_SAMPLES           = 0x98;
const uint8_t SUNRISE_REGISTER_ABC_PERIOD               = 0x9A;
const uint8_t SUNRISE_REGISTER_METER_CONTROL            = 0xA5;
const uint8_t SUNRISE_REGISTER_I2C_ADDRESS              = 0xA7;
const uint8_t SUNRISE_REGISTER_CALIBRATION_STATUS       = 0xC1;
const uint8_t SUNRISE_REGISTER_START_MEASUREMENT        = 0xC3;
const uint8_t SUNRISE_REGISTER_ABC_TIME                 = 0xC4;

const uint16_t MODE_CONTINUOUS                          = 0x0000;
const uint16_t MODE_SINGLE                              = 0x0001;
const uint16_t CALIBRATION_FACTORY                      = 0x7C02;
const uint16_t CALIBRATION_ZERO                         = 0x7C07;

class Sunrise {
	public:
	Sunrise(int PIN_ENABLE);	//Constructor
	void setNewAddress(uint8_t address=SUNRISE_ADDRESS, uint8_t ADDRESS_NEW=SUNRISE_ADDRESS);
	void setMode(uint8_t address=SUNRISE_ADDRESS,uint16_t mode=MODE_SINGLE);
	void setSampleNumber(uint8_t address=SUNRISE_ADDRESS,uint16_t NumberSamples=8);
	void setABC(uint8_t address=SUNRISE_ADDRESS, bool enable=true);
	void setIIR(uint8_t address=SUNRISE_ADDRESS, bool enable=true);
	void setMeasPeriod(uint8_t address=SUNRISE_ADDRESS, uint16_t period=16);
	void setMeterControl(uint8_t address=SUNRISE_ADDRESS, uint8_t status=0x10);
	void readStatus(uint8_t state[], uint8_t address=SUNRISE_ADDRESS);
	uint16_t readCO2_single(uint8_t state[], uint8_t address=SUNRISE_ADDRESS, uint16_t Wait_For_Measurement_MS=2000);
	uint16_t readCO2_single_nofilter(uint8_t address=SUNRISE_ADDRESS, uint16_t Wait_For_Measurement_MS=2000);
	//uint16_t readCO2_continuous(uint8_t address=SUNRISE_ADDRESS);
	void clearCalibrationStatus(uint8_t address=SUNRISE_ADDRESS);
	uint8_t readCalibrationStatus(uint8_t address=SUNRISE_ADDRESS);
	void startCalibration(uint8_t address=SUNRISE_ADDRESS, uint16_t mode = CALIBRATION_FACTORY);
	
	uint8_t readMode(uint8_t address=SUNRISE_ADDRESS);
	uint16_t readMeasPeriod(uint8_t address=SUNRISE_ADDRESS);
	uint16_t readABCPeriod(uint8_t address=SUNRISE_ADDRESS);
	uint8_t readMeterControl (uint8_t address=SUNRISE_ADDRESS);
	uint16_t readNumberSamples(uint8_t address=SUNRISE_ADDRESS);
	bool status(uint8_t address=SUNRISE_ADDRESS);
	
	
	private:
	int _PIN_ENABLE;
	void SunriseEnable();
	void SunriseDisable();
	bool wakeup(uint8_t address=SUNRISE_ADDRESS);
	int SunriseWireRequest(uint8_t sunr_addr, uint8_t bytes_numbers, uint8_t offset_to_read, bool stop);
};

#endif