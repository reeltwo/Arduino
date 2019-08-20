// BODY PINOUT
// =======================================
// D20 - SDA (Blue)
// D21 - SCL (Purble)
// D3  - OUTPUT_ENABLED_PIN
// D8  - CBI_DATAIN_PIN
// D9  - CBI_CLOCK_PIN
// D10 - CBI_LOAD_PIN

#define USE_DEBUG
#define USE_SERVO_DEBUG
#define USE_VERBOSE_SERVO_DEBUG
#define USE_SMQDEBUG
//#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"    /* include first to enable SMQ support */
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "body/DataPanel.h"
#include "body/ChargeBayIndicator.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"

#include "core/JawaCommander.h"

#define OUTPUT_ENABLED_PIN  3
#define CBI_DATAIN_PIN      8
#define CBI_CLOCK_PIN       9
#define CBI_LOAD_PIN        10

#define GROUP_DOORS      0x0003
#define GROUP_SMALLDOORS 0x0001
#define GROUP_BIGDOORS   0x0002
#define GROUP_ARMS       0x0004

#define DOOR_DATAPANEL  0
#define DOOR_LEFT       1
#define UPPER_ARM       2

#define DOOR_CHARGEBAY  3
#define CPUARM_EXTEND   4
#define CPUARM_LIFT     5
#define LOWER_ARM       6
#define DOOR_RIGHT      7


const ServoSettings servoSettings[] PROGMEM = {
    // Servo #1 used for volt meter for now
    { 2,  GROUP_SMALLDOORS, 1000, 1790 },  /* 0: data panel */
    { 3,  GROUP_BIGDOORS,   1000, 1790 },  /* 1: left body door */
    { 4,  GROUP_ARMS,       1000, 1790 },  /* 2: upper utility arm */
    { 12, GROUP_SMALLDOORS, 1000, 1790 },  /* 3: charge bay door */
    { 13, 0,                1000, 1790 },  /* 4: cpu arm extend */
    { 14, 0,                1000, 1790 },  /* 5: cpu arm lift */
    { 15, GROUP_ARMS,       1000, 1790 },  /* 6: lower utlity arm */
    { 16, GROUP_BIGDOORS,   1000, 1790 },  /* 7: right body door */
};
ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
JawaCommander<> jawaCommander;

LedControlMAX7221<3> ledChain1(CBI_DATAIN_PIN, CBI_CLOCK_PIN, CBI_LOAD_PIN);
ChargeBayIndicator chargeBayIndicator(ledChain1, A1);
DataPanel dataPanel(ledChain1);

void openUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 100, 1000);
}

void closeUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 100, 1790);
}

void openLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 300, 1000);
}

void closeLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 300, 1790);
}

void openLeftBodyDoor()
{
    servoDispatch.moveTo(DOOR_LEFT, 150, 100, 1790);
}

void closeLeftBodyDoor()
{
    servoDispatch.moveTo(DOOR_LEFT, 150, 100, 950);
}

void openRightBodyDoor()
{
    servoDispatch.moveTo(DOOR_RIGHT, 150, 100, 1000);
}

void closeRightBodyDoor()
{
    servoDispatch.moveTo(DOOR_RIGHT, 150, 100, 1790);
}

void openDataPanelDoor()
{
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 100, 1790);
    dataPanel.setSequence(DataPanel::kNormal);
}

void closeDataPanelDoor()
{
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 100, 1000);
    dataPanel.setSequence(DataPanel::kDisabled);
}

void openChargeBayDoor()
{
    servoDispatch.moveTo(DOOR_CHARGEBAY, 150, 100, 1750);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kNormal);
}

void closeChargeBayDoor()
{
    servoDispatch.moveTo(DOOR_CHARGEBAY, 150, 100, 1000);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);
}

void openCPUARM()
{
    openRightBodyDoor();
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 100, 1000);
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1000);
    }, 1000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1750);
    }, 3000);
}

void closeCPUARM()
{
    servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1750);
    }, 1000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1000);
    }, 3000);
    DelayCall::schedule([] {
        closeRightBodyDoor();
    }, 4000);
}

////////////////////////////////////////////////////////////////////////////////
//
// SMQ Message Board
//
////////////////////////////////////////////////////////////////////////////////
SMQMESSAGEBOARD(mainMessageBoard)
{
    SMQMESSAGE("SHUTDOWN", {
    }),
    SMQMESSAGE("ServoDispatch", {
        byte num = msg.get_integer(MSGID("num"));
        if (num < servoDispatch.getNumServos())
        {
            int32_t curPos = servoDispatch.currentPos(num);
            uint32_t startDelay = msg.get_integer(MSGID("startDelay"));
            uint32_t moveTime = msg.get_integer(MSGID("moveTime"));
            int32_t startPos = msg.get_integer(MSGID("startPos"));
            int32_t endPos = msg.get_integer(MSGID("endPos"));
            int32_t relPos = msg.get_integer(MSGID("relPos"));
            if (startPos == -1)
                startPos = curPos;
            if (relPos > 0)
                endPos = curPos + relPos;
            servoDispatch.moveTo(num, startDelay, moveTime, startPos, endPos);
        }
    }),
}
SMQMESSAGEBOARD_END(mainMessageBoard);

////////////////////////////////////////////////////////////////////////////////

void setup()
{
    REELTWO_READY();

    Wire.begin();
    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });

    SetupEvent::ready();

    dataPanel.setSequence(DataPanel::kDisabled);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);

    servoDispatch.moveTo(CPUARM_EXTEND, 150, 100, 1000);

    // // Open all servos in 4 seconds
    // DelayCall::schedule([] { openLeftBodyDoor(); openRightBodyDoor(); }, 4000);
    // DelayCall::schedule([] { openChargeBayDoor(); openDataPanelDoor(); }, 4000);
    DelayCall::schedule([] { openLowerArm(); }, 4000);
    // DelayCall::schedule([] { openCPUARM(); openLeftBodyDoor(); openChargeBayDoor(); openDataPanelDoor(); }, 4000);
    // // Close all servos in 8 seconds
    // DelayCall::schedule([] { closeLeftBodyDoor(); closeRightBodyDoor(); }, 8000);
    // DelayCall::schedule([] { closeChargeBayDoor(); closeDataPanelDoor(); }, 8000);
    DelayCall::schedule([] { closeLowerArm(); }, 8000);
    // DelayCall::schedule([] { closeCPUARM(); closeLeftBodyDoor(); closeChargeBayDoor(); closeDataPanelDoor(); }, 12000);
}

long lastTime;
void loop()
{
    AnimatedEvent::process();
    if (lastTime + 100 < millis())
    {
        int val = analogRead(0);
        DEBUG_PRINT("Vibration : ");
        DEBUG_PRINTLN(val);
        lastTime = millis();
    }
}
