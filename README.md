# RiverRunner-Arduino-code
The RiverRunner is a low cost prototype to measure in situ dissolved CH4 and CO2. The main components are an Arduino mini as microporcessor, a real time clock with EEPROM memory, a micro SD card module and a voltage regulator. To measure CH4 we use a TGS2611-C Sensor (Figaro, Japan) and to measure CO2 a miniature NDIR senosr (Senseair, Sweden). To measure temperature, relative humidity and pressure the digital miniature environmental sensor BME280 (Bosch Sensortec GmbH, Germany) is used. All measurement data is stored on the local SD card and the proptotype is powered by two he Li-Ion 18650 batteries connected in parallel.

All codes are specifically designed for pin configuartion and wiring of the RiverRunner prototype and to be used on Arduino microprocessors.

RiverRunner.ino: The here presented Arduino code for the microprocessor of the prototype can be uploaded and compiled with the integrated development environment Arduino IDE.
At the beginning all pins are defined and the microprocessor runs an initialization, where it checks for the availability of all modules. If any module is not found the microprocessor writes an error message and then still goes into the loop. In the loop the microprocessor reads values from the sensors and the RTC at defined time intervals and stores all values on the local SD card.

Additionall code to operate the Sunrise sensor:
Sunrise.h              : A library to use the Senseair Sunrise sensor in combination with the RiverRunner prototype
SetSunrise.ino         : Arduino code to set configurations of Sunrise sensor to operate on RiverRunner prototype
Calibration_Sunrise.ino: Arduino code to set zero calibration on Sunrise Sensor
