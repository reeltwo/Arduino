#if 1

#include "FuzzyServoBoard.h"
#include <Wire.h>

/*
This example sketch sweeps all servo channels between 700 and 2300 microseconds.
*/

//The power MOSFET control pin (labeled "PIN") is controlled by this arduino pin
#define POWER_MOSFET_PIN  7

//Create the servo controller object to control up to 32 servos.
FuzzyServoBoard ServoController;

int servoPosition;

#define SERVOMIN  1000 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  2000 // this is the 'maximum' pulse length count (out of 4096)

void setup()
{
  Serial.begin(115200);
  Serial.println("Fuzzy 32 Channel Servo Range calibration");


  Wire.begin();
  Wire.setClock(400000); //Set i2c frequency to 400 kHz.

  //The begin() method is required to initialize the servo controller object.
  ServoController.begin(27570000, 27190000, 50);
  ServoController.setOutputAll(false);

  //Please refer to Chip_Frequency_Calibrator sketch for more advanced settings.

  //Enable the power MOSFET on the servo board.
  // pinMode(POWER_MOSFET_PIN, OUTPUT);
  // digitalWrite(POWER_MOSFET_PIN, HIGH);

  //ServoController.setPWMAll(SERVOMAX);
  Serial.print("Enter Servo Number (1-32): "); Serial.flush();
}

void sweepServo(int servoNumber, int servoMin, int servoMax)
{
  boolean abort = false;
    Serial.print("Testing servo #");
    Serial.println(servoNumber);
  pinMode(POWER_MOSFET_PIN, OUTPUT);
  digitalWrite(POWER_MOSFET_PIN, HIGH);
    for (servoPosition = servoMax; !Serial.available() && servoPosition >= servoMin; servoPosition -= 25) 
    { 
      Serial.println(servoPosition);
      ServoController.setPWM(servoNumber, servoPosition);
      delay(500);
    }
    if (!Serial.available())
    {
      Serial.println("=====");
      for (servoPosition = servoMin; !Serial.available() && servoPosition <= servoMax; servoPosition += 25) 
      { 
        Serial.println(servoPosition);
        ServoController.setPWM(servoNumber, servoPosition);
        delay(500);
      }
    }
  ServoController.setOutputAll(false);
  pinMode(POWER_MOSFET_PIN, OUTPUT);
  digitalWrite(POWER_MOSFET_PIN, LOW);
  if (Serial.available())
  {
    Serial.println("ABORT");
    while (Serial.available() > 0) Serial.read();
  }
}

String inputBuffer;
int state;
int servoNumber;
int servoMin;
int servoMax;

void loop()
{
    while (Serial.available() > 0)
    {
        char ch = Serial.read();
        if (ch == '\r')
        {
            Serial.println();
            switch (state++)
            {
              case 0:
                servoNumber = inputBuffer.toInt();
                servoNumber = max(1, servoNumber);
                servoNumber = min(32, servoNumber);
                Serial.print("Enter Servo Min (700-2300): ");
                break;
              case 1:
                servoMin = max(700, inputBuffer.toInt());
                servoMin = min(servoMin, 2300);
                Serial.print("Enter Servo Max (700-2300): ");
                break;
              case 2:
                servoMax = min(2300, inputBuffer.toInt());
                servoMax = max(servoMax, 700);
                break;
            }
            Serial.flush();
            if (state == 3)
            {
              int tmp = servoMin;
              servoMin = min(servoMin, servoMax);
              servoMax = max(tmp, servoMax);
              Serial.print("Sweep "); Serial.print(servoNumber);
              Serial.print(" from "); Serial.print(servoMin);
              Serial.print(" to "); Serial.println(servoMax);
              sweepServo(servoNumber, servoMin, servoMax);
              Serial.print("Enter Servo Number (1-32): ");
              Serial.flush();
              state = 0;
            }
            inputBuffer = "";
        }
        else
        {
          Serial.print(ch); Serial.flush();
            inputBuffer += ch;
        }
    }
}

#elif 0
#include "FuzzyServoBoard.h"
#include <Wire.h>

/*
This sketch uses the arduino controller to measure PCA9685 output signal, and 
calculate the internal oscillator frequency of the PCA9685. Due to the inaccurate 
nature of the internal oscillator, the PCA9685 core frequency may be as much as 5%~10% 
off and differs chip by chip. So each chip needs to be calibrated if higher 
degree of accuracy is desired for robotic projects.

The calculated clock frequency is then used as the calibrator value for future usage. 

This sketch used the clock source from user's arduino control board 
(ex. UNO, NANO, MEGA2560, ZERO, etc...). Therefore the accuracy is limited by 
the on-board crystal oscillator which has a typical value of 0.002% ~ 0.005%. (20~50 ppm)


The measured difference of core frequency using different prescale (thus different 
update frequency) is very small, around 0.03% maximum.  Thus one-time calibration 
for each chip should be good enough for different servos.

*/


/*
Wiring

Arduino PIN                         Servo Board Pin
5V                                  5V
GND                                 GND
SDA                                 SDA (A4 for UNO)
SCL                                 SCL (A5 for UNO)
PIN                                 POWER_MOSFET_PIN (5 in example)
CHIP_1_INTERRUPT_INPUT_PIN (2)      CHIP_1_FEEDBACK_CHANNEL (16)

*/

#define POWER_MOSFET_PIN  7
#define CHIP_1_FEEDBACK_CHANNEL 16
#define CHIP_2_FEEDBACK_CHANNEL 32
#define CHIP_1_INTERRUPT_INPUT_PIN 2
#define CHIP_2_INTERRUPT_INPUT_PIN 3


//For oscilloscope usage
#define CHIP_1_OSCILLOSCOPE_CHANNEL 2
#define CHIP_2_OSCILLOSCOPE_CHANNEL 18
#define NUMBER_OF_SAMPLES 500

//Create the servo controller object to control up to 32 servos.
FuzzyServoBoard ServoController;

volatile uint32_t chipPulseCounter = 0;
volatile uint32_t numberOfSamples = NUMBER_OF_SAMPLES;
volatile bool printProgress = false;
volatile uint32_t progressStep = 0;
volatile uint32_t startMicros;
volatile uint32_t endMicros;

void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < 10; i++)
{
delay(400);
  Serial.println("Fuzzy 32 Channel Servo Board");
  Serial.println("[Chip Frequency Calibrator] example started.");
  Serial.println(i);
}

  Wire.begin();
  Wire.setClock(400000); //set i2c frequency to 400kHz

  //The begin() method is required to initialize the servo controller object.
  ServoController.begin();

  //Enable the power MOSFET on the servo board.
  pinMode(POWER_MOSFET_PIN, OUTPUT);
  digitalWrite(POWER_MOSFET_PIN, HIGH);


  /*
  Modify this value (targetUpdateFrequency) to desired update frequency.
  Achievable value range is 24 to 1526 Hz. according to the datasheet.
  A classic servo runs at 50 Hz, and the resolution is around 4.88us at this update frequency.
  Increase the update frequency reduce both the response time and resolution, however, if 
  the update frequency goes too high, the servo might shake or stop working. 
  
  Approximate values for some update frequencies:
  Update frequency        Period      Resolution
   50    Hz.                20    ms.      4.88 us.
   60    Hz.                16.67 ms.      4.07 us.
  100    Hz.                10    ms.      2.71 us.
  122.07 Hz.                8.19  ms.      2.00 us.
  150    Hz.                6.67  ms.      1.63 us.
  200    Hz.                5     ms.      1.22 us.
  244.14 Hz.                4.1   ms.      1.00 us.
  */
  float targetUpdateFrequency = 50;
  
  
  /*
  Alternatively, user may set the target resolution for calibration.
  Uncomment the two lines below to set the desired frequency using desired resolution.
  */
  //float targetResolution = 1.0;
  //targetUpdateFrequency = getUpdateFrequencyFromResolution(targetResolution);
  

  ServoController.setPrescale(PCA9685_DEFAULT_PRESCALE_VALUE);
  ServoController.setPWM(CHIP_1_FEEDBACK_CHANNEL, 1500);
  Serial.println("Running first pass using default prescale...");
  float clock1 = getCalculatedClockFrequency(PCA9685_CHIP_1, NUMBER_OF_SAMPLES);
  uint8_t prescale1 = getCalculatedPrescale(clock1, targetUpdateFrequency);
  Serial.println("Now running second pass...");
  Serial.print("Using target update frequency of ");
  Serial.print(targetUpdateFrequency);
  Serial.println(" Hz");
  Serial.print("Resetting prescale = ");
  Serial.println(prescale1);
  ServoController.setPrescale(prescale1);
  Serial.println("Verifying result...");
  clock1 = getCalculatedClockFrequency(PCA9685_CHIP_1, NUMBER_OF_SAMPLES);
  Serial.println("Now running third pass...");
  prescale1 = getCalculatedPrescale(clock1, targetUpdateFrequency);
  ServoController.setPrescale(prescale1);
  clock1 = getCalculatedClockFrequency(PCA9685_CHIP_1, NUMBER_OF_SAMPLES);


  ServoController.setPrescale(PCA9685_DEFAULT_PRESCALE_VALUE);
  ServoController.setPWM(CHIP_2_FEEDBACK_CHANNEL, 1500);
  Serial.println("Running first pass using default prescale...");
  float clock2 = getCalculatedClockFrequency(PCA9685_CHIP_2, NUMBER_OF_SAMPLES);
  uint8_t prescale2 = getCalculatedPrescale(clock2, targetUpdateFrequency);
  Serial.println("Now running second pass...");
  Serial.print("Using target update frequency of ");
  Serial.print(targetUpdateFrequency);
  Serial.println(" Hz");
  Serial.print("Resetting prescale = ");
  Serial.println(prescale2);
  ServoController.setPrescale(prescale2);
  Serial.println("Verifying result...");
  clock2 = getCalculatedClockFrequency(PCA9685_CHIP_2, NUMBER_OF_SAMPLES);
  Serial.println("Now running third pass...");
  prescale2 = getCalculatedPrescale(clock2, targetUpdateFrequency);
  ServoController.setPrescale(prescale2);
  clock2 = getCalculatedClockFrequency(PCA9685_CHIP_2, NUMBER_OF_SAMPLES);

  ServoController.setOutputAll(false);

  // Output final results for two PCA9685 chips
  Serial.println("Now printing final results...");
  Serial.println("*******************************************");
  Serial.println("Use the following values for calibration:");
  Serial.print("Chip 1 clock frequency = ");
  Serial.println((uint32_t)clock1);
  Serial.print("Chip 2 clock frequency = ");
  Serial.println((uint32_t)clock2);
  Serial.println("*******************************************");
  Serial.println("Sketch ended.");

}

void loop()
{



}


float getUpdateFrequencyFromResolution(float targetResolution)
{
  float updateFrequency;
  updateFrequency = 1000000.0 / (4096 * targetResolution);
  return updateFrequency;
}

float getResolutionMicroseconds(float calculatedClockFrequency, uint8_t prescale)
{
  float resolution;
  resolution = 1000000.0 / (4096.0 * getCalculatedUpdateFrequency(calculatedClockFrequency, prescale));
  return resolution;
}

float getCalculatedUpdateFrequency(float calculatedClockFrequency, uint8_t prescale)
{
  float calculatedUpdateFrequency;
  calculatedUpdateFrequency = calculatedClockFrequency / (float)(((uint32_t)prescale + 1) << 12);
  return calculatedUpdateFrequency;
}

uint8_t getCalculatedPrescale(float calculatedClockFrequency, float targetUpdateFrequency)
{
  //rounding method from here: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/issues/40
  uint32_t calculatedPrescale;
  calculatedPrescale = calculatedClockFrequency * 100 / (4096 * targetUpdateFrequency);
  if ((calculatedPrescale % 100) >= 50) calculatedPrescale += 100;
  calculatedPrescale = (calculatedPrescale / 100) - 1;
  return (uint8_t)calculatedPrescale;
}


float getCalculatedClockFrequency(uint8_t chipID,uint32_t samples)
{
  ServoController.setOutputAll(false);
  chipPulseCounter = 0;
  numberOfSamples = samples;

  uint8_t pin;
  if (chipID == PCA9685_CHIP_1)pin = CHIP_1_INTERRUPT_INPUT_PIN;
  if (chipID == PCA9685_CHIP_2)pin = CHIP_2_INTERRUPT_INPUT_PIN;



  uint8_t prescale = ServoController.getPrescale(chipID);
  progressStep = (numberOfSamples / 10);


  Serial.print("Getting calculated clock frequency for chip ");
  Serial.print(chipID);
  Serial.print(" on pin ");
  Serial.println(pin);
  Serial.print("Prescale = ");
  Serial.println(prescale);
  Serial.print("Samples = ");
  Serial.println(numberOfSamples);

  Serial.print("Progress ");
  attachInterrupt(digitalPinToInterrupt(pin), chipPulseCount, RISING);
  ServoController.setPWMAll(1500);
  
  
  while (chipPulseCounter <= numberOfSamples)
  {
    if (printProgress == true)
    {
      Serial.print(".");
      printProgress = false;
    }
  }
  Serial.println(" Done");

  ServoController.setOutputAll(false);
  detachInterrupt(digitalPinToInterrupt(pin));


  uint32_t duration = endMicros - startMicros;
  float period = (float)duration / (float)numberOfSamples;
  float measuredUpdateFrequency = (float)1000000 / period;
  
  float nominalUpdateFrequency = ServoController.getNominalUpdateFrequency(NOMINAL_CLOCK_FREQUENCY, prescale);
  uint32_t calculatedClockFrequency = (uint32_t)(((float)NOMINAL_CLOCK_FREQUENCY) * measuredUpdateFrequency / nominalUpdateFrequency);
  
  float error = 100*(calculatedClockFrequency - (float)NOMINAL_CLOCK_FREQUENCY) / (float)NOMINAL_CLOCK_FREQUENCY;

  Serial.print("Period Count = ");
  Serial.println(chipPulseCounter-1);
  Serial.print("Duration(us) = ");
  Serial.println(duration);
  Serial.print("Period(us) = ");
  Serial.println(period);
  Serial.print("Measured update frequency = ");
  Serial.println(measuredUpdateFrequency);
  Serial.print("Calculated clock frequency = ");
  Serial.println(calculatedClockFrequency);
  Serial.print("Frequency error = ");
  Serial.print(error);
  Serial.println("%");
  Serial.print("Resolution = ");
  Serial.print(getResolutionMicroseconds(calculatedClockFrequency, prescale));
  Serial.println(" microseconds");

  Serial.println();

  return calculatedClockFrequency;
}

void chipPulseCount()
{
  if (chipPulseCounter == 0)startMicros = micros();
  if (chipPulseCounter == numberOfSamples) endMicros = micros();
  if ((chipPulseCounter%progressStep) == 0) printProgress = true;
  chipPulseCounter++;
}

#elif 0

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

#define LSM9DS1_SCK A5
#define LSM9DS1_MISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 6
#define LSM9DS1_MCS 5
// You can also use software SPI
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_SCK, LSM9DS1_MISO, LSM9DS1_MOSI, LSM9DS1_XGCS, LSM9DS1_MCS);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);


void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}


void setup() 
{
  Serial.begin(115200);

//  while (!Serial) {
//    delay(1); // will pause Zero, Leonardo, etc until serial console opens
//  }
  delay(1000); 
  Serial.println("LSM9DS1 data read demo");
  delay(1000);  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
  setupSensor();
}

void loop() 
{
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;

  lsm.getEvent(&a, &m, &g, &temp); 

  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("\tY: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("\tZ: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");

  Serial.print("Mag X: "); Serial.print(m.magnetic.x);   Serial.print(" gauss");
  Serial.print("\tY: "); Serial.print(m.magnetic.y);     Serial.print(" gauss");
  Serial.print("\tZ: "); Serial.print(m.magnetic.z);     Serial.println(" gauss");

  Serial.print("Gyro X: "); Serial.print(g.gyro.x);   Serial.print(" dps");
  Serial.print("\tY: "); Serial.print(g.gyro.y);      Serial.print(" dps");
  Serial.print("\tZ: "); Serial.print(g.gyro.z);      Serial.println(" dps");

  Serial.println();
  delay(200);
}

#else


#include "TeecesPSI.h"
#include "HoloLights.h"
#include "HoloDisplay.h"
#include "SerialCommand.h"
#include "I2CCommand.h"
#include <ArduinoJson.h>
//#include "CommandProcessor.h"

class Mqtt {
  public:
    static String device;
    static void publish( String topic, String message, int qos = 0, bool retained = false ) {
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& data = jsonBuffer.createObject();
      data["cmd"] = "MQTT-PUB";
      data["topic"] = "src/" + device + "/" + topic;
      data["message"] = message;
      if ( qos != 0 ) data["qos"] = qos;
      if ( retained) data["retained"] = retained;
      data.printTo(Serial);
      Serial.println();
    }
    static void handleLine(String& line) {
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(line);
      onMqttMessage(root["topic"], root["message"], root["qos"], root["retained"]);
    }

    static void onMqttMessage(String topic, String message, int qos, bool retained) {
    // add your own subscriber here 
      Serial.println(" Mqtt Message arrived");
    }
};

// create a name for this device
String Mqtt::device = "R2P2";

Teeces<1> teecesChain(6, 7, 8);
PSI frontPSI = teecesChain[0];
PSI rearPSI = teecesChain[1];
PSI rearPSI2 = teecesChain[2];
HoloDisplay frontHolo(45);
HoloLights rearHolo(2);
HoloLights topHolo(3, 12);
HoloLights radarEye(4, 24);
#if 0
// LogicEngine frontLogics; //PIN5
// LogicEngine rearLogics;  //PIN6
// I2CPeriscope<0x??> periscope;
// I2CMagicPanel<0x??> magicPanel;
// DomeDoor doors[13];
// //SerialCommand<Serial> serial;
I2CCommand<0x23> i2c;

// class MagicPanelI2C
// {
// public:
// 	MagicPanelI2C()
// 	{
// 		i2c = 99;
// 	}


// };

// MagicPanel magicpanel;

class Base
{
public:
	Base() {}
};

class Unit1 :
	public Base
{
public:
	Unit1() {}
};

class Unit2 :
	public Base
{
public:
	Unit2() {}
};

struct Foo
{
	int cmd;
	Base& type;
};

template <int numDevices = 2>
class CommandProcessor
{
public:
	CommandProcessor(const struct Foo (&ledPins)[numDevices]) {}
};

Unit1 unit1;
Unit2 unit2;
CommandProcessor<> cmd({{1, unit1},{2,unit2}});
#endif

void setup()
{
  Serial.begin(115200);
  DelayedSetup::ready();
	// cmd.setTopFrontLogicDisplay(tfld);
	// cmd.setBottomFrontLogicDisplay(bfld);
	// cmd.setRearLogicDisplay(rld);

	// cmd.setFrontPSI(fpsi);
	// cmd.setRearPSI(rpsi);

	// cmd.setFrontHolo(fholo);
	// cmd.setRearHolo(rholo);
	// cmd.setTopHolo(tholo);

	// cmd.setMagicPanel(mpanel);
	// cmd.setPeriscope(periscope);

	//teecesChain.setPSIPower(true);
	//frontHolo.init();
}

String line;
void loop()
{
  while (Serial.available()) {
    char ch = Serial.read();
    if ( ch == '\r' || ch == '\n' ) {
      Mqtt::handleLine(line);
      line = "";
    } else
      line += ch;
  }
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a 0.1 second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                       // wait for a 0.1 second
  Mqtt::publish( "system/upTime", String(millis(), 10), 0, false);
  Mqtt::publish("system/host", Mqtt::device, 0, false);
  Mqtt::publish("system/alive", "true", 0, false);

	// serial.process();
	//i2c.process();

	//teecesChain.animate();
	//frontHolo.animate();
}

#endif
