/*BUILT-IN HEADERS*/
#include <Wire.h> //I2C communication
#include <SoftwareSerial.h> //Serial communication

/*CUSTOM HEADERS*/
#include "RTClib.h" //Digital clock
#include "TSL2561.h" //Luminosity sensor

/*CALL SIGNS*/
RTC_DS1307 RTC; //Digital clock
TSL2561 tsl(TSL2561_ADDR_FLOAT);  //Luminosity sensor
SoftwareSerial serial(19,18); //19=RX1, 18=TX1 - CURRENT DEVICE: SDCard

/*GLOBAL VARIABLES*/
float value_amplifier = 30.00/512.00; //Coefficient for current convertion
int tmp102Address1 = 72; //Temperture Sensors: one that has its ADD0 connected to GND
int tep102Address2 = 75; //Temperture Sensors: one that has its ADD0 connected to SCL
int currentSensor1 = A1; //Analog Input 0, connected to current senser #1

/*SETUP: Run these commands only once*/
void setup(){

  analogReference(DEFAULT); //Set analog reference voltage as 5V
  Serial.begin(9600); //Begin serial communication
  Wire.begin(); //Begin i2c communication
  RTC.begin(); //Begin communication with the Digital xlock
  tsl.begin(); //Begin communication with the Luminosity sensor
 
  if (! RTC.isrunning()){ //Report if the Digital xlock is not detected
    serial.println("The RTC module is not detected.");
  }
  
  if (! tsl.begin()){ //Report if the Luminsity sensor is not detected
    Serial.println("The Luminsity sensor is not detected.");
  }
  
  //Setting for the Luminosity sensor
  tsl.setGain(TSL2561_GAIN_16X); //Gain = 16x
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS); //integration time = 13ms
  
  //Date & Time adjustment command line: 
  //RTC.adjust(DateTime(__DATE__, __TIME__));
}

/*LOOP: Run this loop until power is disconnected from the Arduino*/
void loop(){

  printDate(); //Date and Time (from Digital Clock)

  uint32_t lum = tsl.getFullLuminosity(); //Call the function to get reading
  uint16_t ir, full; ir = lum >> 16; full = lum & 0xFFFF; //Unit conversion
  Serial.print("Full Spectrum: "); Serial.print(full);   Serial.print("\t");
  Serial.print("Visible Light: "); Serial.print(full - ir);   Serial.print("\t");
  Serial.print("Infrared: "); Serial.print(ir);   Serial.print("\t");
  Serial.print("Lux: "); Serial.println(tsl.calculateLux(full, ir));

  float celsius1 = getTemperature(tmp102Address1); //1st temperature sensor
  Serial.print("Temperature Sensor #1: ");
  Serial.print(celsius1);
  Serial.print(" C, ");
  float fahrenheit1 = (1.8 * celsius1) + 32; //Convertion from C to F
  Serial.print(fahrenheit1);
  Serial.println(" F");

  float celsius2 = getTemperature(tep102Address2); //2nd temperature sensor #1
  Serial.print("Temperature Sensor #2: ");
  Serial.print(celsius2);
  Serial.print(" C, ");
  float fahrenheit2 = (1.8 * celsius2) + 32; //Convertion from C to F
  Serial.print(fahrenheit2);
  Serial.println(" F");

  int value = getCurrent(currentSensor1); //Obtain digital representation of current value
  int calibrated_value = map(value, -512, 512, -512, 512); //Calibration for sensor #1 only
  float current1 = calibrated_value*value_amplifier; //Convert digital value to real current
  
  Serial.print("Current Sensor #1: ");
  Serial.print(current1);
  Serial.print(" A, ");
  float current1_m = current1*1000.00; //Convertion from A to mA
  Serial.print(current1_m);
  Serial.println(" mA (error: +/-0.06A)");

int value2 = analogRead(A2);
Serial.println(value2);

  Serial.println(); //New line to make the format neater!!
  delay(5000); //Repeat every 5000ns
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
  Serial.print("0");}
  Serial.print(now.minute(), DEC); 
  Serial.print(":");
  // if time less than 10 secs, print 0:
  if (now.second() < 10){
  Serial.print("0");}
  Serial.println(now.second(), DEC);
}

/*FUNCTION: Temperature sensor*/
float getTemperature(int tmp102Address){
  Wire.requestFrom(tmp102Address,2); 
  byte MSB = Wire.read();
  byte LSB = Wire.read();
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 
  float celsius = TemperatureSum*0.0625;
  return celsius;
}

/*FUNCTION: Current sensor*/
int getCurrent(int currentSensor){
  //int value = analogRead(currentSensor); //Read from the specificed analog input pin
  int value = analogRead(A0); //Read from the specificed analog input pin

  int value_adjusted = map(value, 0, 1023, -512, 512); //Mapping
  return value_adjusted; //return digital value reading
}
