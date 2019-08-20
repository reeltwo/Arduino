#define USE_DEBUG
//#define USE_SMQDEBUG
#include "ReelTwo.h"
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "body/Stance.h"
#include "core/StealthController.h"
#include "i2c/I2CReceiver.h"
#include "Orientation.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"

#include "core/JawaCommander.h"

#define PIN_OUTPUT_ENABLE   5
#define PIN_LEG_TILT_UP     6
#define PIN_LEG_TILT_DOWN   8
#define PIN_CENTER_LEG_UP   9
#define PIN_CENTER_LEG_DOWN 7
#define PIN_CENTER_LEG_POT  A0

#define PIN_STEALTH_S1      26
#define PIN_STEALTH_J1      28
#define PIN_STEALTH_S2      27
#define PIN_STEATLH_J2      29

const ServoSettings servoSettings[] PROGMEM = {
};

ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
Orientation bodyOrientation(0);
Orientation tiltOrientation(1);
JawaCommander<> jawaCommander;
StealthController stealthController(PIN_STEALTH_S1, PIN_STEALTH_J1, PIN_STEALTH_S2, PIN_STEATLH_J2);
Stance stance(PIN_LEG_TILT_UP, PIN_LEG_TILT_DOWN, PIN_CENTER_LEG_UP, PIN_CENTER_LEG_DOWN, PIN_CENTER_LEG_POT);

////////////////////////////////////////////////////////////////////////////////
//
// SMQ Message Board
//
////////////////////////////////////////////////////////////////////////////////
// SMQMESSAGEBOARD(mainMessageBoard)
// {
//     SMQMESSAGE("JAWA", {
//         char* cmdBuffer = jawaCommander.getBuffer();
//         size_t cmdBufferSize = jawaCommander.getBufferSize();
//         const char* cmd = msg.get_string(MSGID("cmd"), cmdBuffer, cmdBufferSize);
//         jawaCommander.process('\r');
//     }),
//     SMQMESSAGE("ServoDispatch", {
//         byte num = msg.get_integer(MSGID("num"));
//         if (num < servoDispatch.getNumServos())
//         {
//             int32_t curPos = servoDispatch.currentPos(num);
//             uint32_t startDelay = msg.get_integer(MSGID("startDelay"));
//             uint32_t moveTime = msg.get_integer(MSGID("moveTime"));
//             int32_t startPos = msg.get_integer(MSGID("startPos"));
//             int32_t endPos = msg.get_integer(MSGID("endPos"));
//             int32_t relPos = msg.get_integer(MSGID("relPos"));
//             if (startPos == -1)
//                 startPos = curPos;
//             if (relPos > 0)
//                 endPos = curPos + relPos;
//             servoDispatch.moveTo(num, startDelay, moveTime, startPos, endPos);
//         }
//     }),
// }
// SMQMESSAGEBOARD_END(mainMessageBoard);

////////////////////////////////////////////////////////////////////////////////

constexpr uint32_t MARCMSG(char c1, char c2, char c3) {
    return ((((unsigned long)c1) << 16) | (((unsigned long)c2) << 8) | (((unsigned long)c3) << 0));
}

#include "Marcduino.h"

////////////////////////////////////////////////////////////////////////////////

I2CReceiver i2c(99);

void setup()
{
    REELTWO_READY();
    DEBUG_PRINTLN("Hello1");

    stance.setOrientationSensors(bodyOrientation, tiltOrientation);
    servoDispatch.setOutputEnablePin(PIN_OUTPUT_ENABLE);
    DEBUG_PRINTLN("Hello2");
//    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));
    DEBUG_PRINTLN("Hello3");

    SetupEvent::ready();

}

void loop()
{
    float bodyYaw;
    float bodyRoll;
    float bodyPitch;
    float tiltYaw;
    float tiltRoll;
    float tiltPitch;
    AnimatedEvent::process();
    if (stealthController.getStatusChanged())
    {
        DEBUG_PRINT("S1=");
        DEBUG_PRINT(stealthController.getS1());
        DEBUG_PRINT(" S2=");
        DEBUG_PRINT(stealthController.getS2());
        DEBUG_PRINT(" J1=");
        DEBUG_PRINT(stealthController.getJ1());
        DEBUG_PRINT(" J2=");
        DEBUG_PRINTLN(stealthController.getJ2());
    }
    // if (bodyOrientation.getYawChanged(bodyYaw))
    // {
    //     DEBUG_PRINT("BODY YAW    : ");
    //     DEBUG_PRINTLN(bodyYaw);
    // }
    // if (bodyOrientation.getPitchChanged(bodyPitch))
    // {
    //     DEBUG_PRINT("BODY PITCH  : ");
    //     DEBUG_PRINTLN(bodyPitch);
    // }
    // if (bodyOrientation.getRollChanged(bodyRoll))
    // {
    //     DEBUG_PRINT("BODY ROLL   : ");
    //     DEBUG_PRINTLN(bodyRoll);
    // }
    // if (tiltOrientation.getYawChanged(tiltYaw))
    // {
    //     DEBUG_PRINT("TILT YAW    : ");
    //     DEBUG_PRINTLN(tiltYaw);
    // }
    // if (tiltOrientation.getPitchChanged(tiltPitch))
    // {
    //     DEBUG_PRINT("TILT PITCH  : ");
    //     DEBUG_PRINTLN(tiltPitch);
    // }
    // if (tiltOrientation.getRollChanged(tiltRoll))
    // {
    //     DEBUG_PRINT("TILT ROLL   : ");
    //     DEBUG_PRINTLN(tiltRoll);
    // }
}
