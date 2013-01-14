/*BUILT-IN HEADERS*/
#include <Wire.h> //I2C communication
#include <SoftwareSerial.h> //Serial communication

/*CUSTOM HEADERS*/
#include "RTClib.h" //Digital Clock
#include "TSL2561.h" //Luminosity Sensor
#include "OneWire.h" //1-Wire library for DS18S20 Temperture Sensor

/*GLOBAL VARIABLES*/
float value_amplifier1 = 30.00/512.00; //Coefficient for current convertion (ACS714)
float value_amplifier2 = 30.00/1024.00; //Coefficient for current convertion (ACS715)
int currentSensor1 = A1; //Current Senser (ACS714): Analog Input A1
int currentSensor2 = A2; //Current Sensor (ACS715): Analog Input A2
int tmp102Address1 = 72; //Temperture Sensor(TMP102): ADD0 connected to NOTHING
int tmp102Address2 = 73; //Temperture Sensor(TMP102): ADD0 connected to VCC
int DS18S20_Pin = 12; //DS18S20 Signal Pin on digital 13
int led = 13; //LED indicator light

/*Devices I/O*/
RTC_DS1307 RTC; //Digital clock
TSL2561 tsl(TSL2561_ADDR_FLOAT);  //Luminosity sensor
SoftwareSerial serial(19,18); //19=RX1, 18=TX1 - CURRENT DEVICE: SDCard
OneWire ds(DS18S20_Pin);  //DS18S20 OwnWire Temperture Sensor #1

/*SETUP: Run these commands only once*/
void setup(){

  analogReference(DEFAULT); //Set analog reference voltage as 5V
  Serial.begin(9600); //Begin serial communication
  Wire.begin(); //Begin i2c communication
  RTC.begin(); //Begin communication with the Digital xlock
  tsl.begin(); //Begin communication with the Luminosity sensor
  pinMode(led, OUTPUT); //Set pin 13 as digital output for the LED

  if (! RTC.isrunning()){ //Report if the Digital xlock is not detected
    serial.println("The RTC module is not detected.");
  }

  if (! tsl.begin()){ //Report if the Luminsity sensor is not detected
    Serial.println("The Luminsity sensor is not detected.");
  }

  //Setting for the Luminosity sensor
  tsl.setGain(TSL2561_GAIN_0X); //Gain = 0x
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS); //Integration time = 13ms

  //Date & Time adjustment command line: 
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

/*LOOP: Run this loop until power is disconnected from the Arduino*/
void loop(){

  printDate(); //Date and Time (from Digital Clock)

  uint32_t lum = tsl.getFullLuminosity(); //Call the function to get reading
  uint16_t ir, full; 
  ir = lum >> 16; 
  full = lum & 0xFFFF; //Unit conversion
  Serial.print("Full Spectrum: "); 
  Serial.print(full);   
  Serial.print("\t");
  Serial.print("Visible Light: "); 
  Serial.print(full - ir);   
  Serial.print("\t");
  Serial.print("Infrared: "); 
  Serial.print(ir);   
  Serial.print("\t");
  Serial.print("Lux: "); 
  Serial.println(tsl.calculateLux(full, ir));

  float celsius1 = getTemperature102(tmp102Address1); //Temperature Sensor (TMP102)
  Serial.print("Temperature Sensor #1: ");
  Serial.print(celsius1);
  Serial.print(" C, ");
  float fahrenheit1 = (1.8 * celsius1) + 32; //Convertion from C to F
  Serial.print(fahrenheit1);
  Serial.println(" F");

  float celsius2 = getTemperature102(tmp102Address2); //Temperature Sensor (TMP102)
  Serial.print("Temperature Sensor #2: ");
  Serial.print(celsius2);
  Serial.print(" C, ");
  float fahrenheit2 = (1.8 * celsius2) + 32; //Convertion from C to F
  Serial.print(fahrenheit2);
  Serial.println(" F");

  float celsius3 = getTemperatureDS18S20();
  Serial.print("Temperature Sensor #3: ");
  Serial.print(celsius3);
  Serial.print(" C, ");
  float fahrenheit3 = (1.8 * celsius3) + 32; //Convertion from C to F
  Serial.print(fahrenheit3);
  Serial.println(" F");

  float value1 = getCurrent714(currentSensor1); //Obtain digital representation of current value
  float calibrated_value1 = map((long)value1, -512, 512, -511, 513); //Calibration specificly for sensor #1 only
  float current1 = calibrated_value1*value_amplifier1; //Convert digital value to real current
  Serial.print("Current Sensor #1: "); //1st current sensor
  Serial.print(current1);
  Serial.print(" A, ");
  float current1_m = current1*1000.00; //Convertion from A to mA
  Serial.print(current1_m);
  Serial.println(" mA");

  float value2 = getCurrent715(currentSensor2); //Obtain digital representation of current value
  float calibrated_value2 = map((long)value2, 0, 1023, 0, 1023); //Calibration specificly for sensor #2 only
  float current2_m = (((long)calibrated_value2 * 5000/1024)-500) * 1000 / 133; //Convert digital value to real current in mA
  float current2 = (float)current2/1000.00; //Convertion from mA to A
  Serial.print("Current Sensor #2: "); //1st current sensor
  Serial.print(current2);
  Serial.print(" A, ");
  Serial.print(current2_m);
  Serial.println(" mA");
  Serial.println(); //New line

  for(int counter=0; counter<10; counter++){
    digitalWrite(led, HIGH); //turn the LED on
    delay(500);  //wait for half a second
    digitalWrite(led, LOW); //turn the LED off
    delay(500); //wait for half a second
  }
}

/*FUNCTION: Digital Clock*/
void printDate(){
  DateTime now = RTC.now();
  //Print the time information in decimal value:
  Serial.print("Date: ");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.println(now.year(), DEC);
  Serial.print("Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  //If time less than 10 mins, print 0:
  if (now.minute() < 10){
    Serial.print("0");
  }
  Serial.print(now.minute(), DEC); 
  Serial.print(":");
  // if time less than 10 secs, print 0:
  if (now.second() < 10){
    Serial.print("0");
  }
  Serial.println(now.second(), DEC);
}

/*FUNCTION: Temperature Sensor (TMP102: I2C)*/
float getTemperature102(int tmp102Address){
  Wire.requestFrom(tmp102Address,2); 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  float celsius = TemperatureSum*0.0625;
  return celsius;
}

/*FUNCTION: Temperature Sensor (DS18S20: 1-Wire)*/
float getTemperatureDS18S20(){
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}

/*FUNCTION: Current Sensor (ACS714: -/+30A)*/
float getCurrent714(int currentSensor){
  int value_sum1 = 0;
  int value_sum2 = 0;
  int value_sum3 = 0;
  int value_sum4 = 0;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum1 += value;
    delay(3);
  }
  int value1 = value_sum1/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum2 += value;
    delay(3);
  }
  int value2 = value_sum2/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum3 += value;
    delay(3);
  }
  int value3 = value_sum3/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum4 += value;
    delay(3);
  }
  int value4 = value_sum4/30;
  float value_sum = (value1+value2+value3+value4)*0.25;
  float value_adjusted = map((long)value_sum, 0, 1023, -512, 512); //General mapping for ACS714
  return value_adjusted; //return digital value reading
}

/*FUNCTION: Current Sensor (ACS715: +30A)*/
float getCurrent715(int currentSensor){
  int value_sum1 = 0;
  int value_sum2 = 0;
  int value_sum3 = 0;
  int value_sum4 = 0;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum1 += value;
    delay(3);
  }
  int value1 = value_sum1/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum2 += value;
    delay(3);
  }
  int value2 = value_sum2/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum3 += value;
    delay(3);
  }
  int value3 = value_sum3/30;
  for(int i=0; i<30; i++){
    int value = analogRead(currentSensor); //Read from the specificed analog input pin
    value_sum4 += value;
    delay(3);
  }
  int value4 = value_sum4/30;
  float value_sum = (value1+value2+value3+value4)*0.25;
  return value_sum; //return digital value reading
}
