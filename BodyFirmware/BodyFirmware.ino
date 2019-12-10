//#define USE_DEBUG
//#define USE_SMQDEBUG
#include "ReelTwoSMQ.h"
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
#include "core/Marcduino.h"

#define PIN_OUTPUT_ENABLE       5
#define PIN_LEG_TILT_UP         6
#define PIN_LEG_TILT_DOWN       8
#define PIN_CENTER_LEG_UP       9
#define PIN_CENTER_LEG_DOWN     7
#define PIN_CENTER_LEG_POT      A0

#define PIN_STEALTH_LEGMOTORS   12
#define PIN_STEALTH_DOMEMOTOR   13

#define PIN_STEALTH_S1          26
#define PIN_STEALTH_J1          28
#define PIN_STEALTH_S2          27
#define PIN_STEATLH_J2          29

const ServoSettings servoSettings[] PROGMEM = {
};

ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
Orientation bodyOrientation(0);
Orientation tiltOrientation(1);
StealthController stealthController(PIN_STEALTH_S1, PIN_STEALTH_J1, PIN_STEALTH_S2, PIN_STEATLH_J2, PIN_STEALTH_LEGMOTORS, PIN_STEALTH_DOMEMOTOR);
Stance stance(PIN_LEG_TILT_UP, PIN_LEG_TILT_DOWN, PIN_CENTER_LEG_UP, PIN_CENTER_LEG_DOWN, PIN_CENTER_LEG_POT);

AnimationPlayer player(servoSequencer);
JawaCommander<> jawaCommander;

////////////////////////////////////////////////////////////////////////////////

SMQMESSAGE(MARC, {
    static char marcBuffer[65];
    const char* cmd = msg.get_string(MSGID("cmd"), marcBuffer, sizeof(marcBuffer));
    Marcduino::processCommand(player, cmd);
})

SMQMESSAGE(JAWA, {
    char* cmdBuffer = jawaCommander.getBuffer();
    size_t cmdBufferSize = jawaCommander.getBufferSize();
    const char* cmd = msg.get_string(MSGID("cmd"), cmdBuffer, cmdBufferSize);
    jawaCommander.process(cmd);
})

////////////////////////////////////////////////////////////////////////////////

I2CReceiver i2c(99);

void setup()
{
    REELTWO_READY();

    pinMode(LED_BUILTIN, OUTPUT);

    stance.setOrientationSensors(bodyOrientation, tiltOrientation);
    servoDispatch.setOutputEnablePin(PIN_OUTPUT_ENABLE);
//    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    SetupEvent::ready();
    DelayCall::schedule([] { Marcduino::send(F("$84")); }, 2000);
}

////////////////

MARCDUINO_ACTION(DirectCommand, *RT, ({
    // Direct ReelTwo command
    CommandEvent::process(Marcduino::getCommand());
}))

////////////////

void loop()
{
    // float bodyYaw;
    // float bodyRoll;
    // float bodyPitch;
    // float tiltYaw;
    // float tiltRoll;
    // float tiltPitch;
    AnimatedEvent::process();

    if (stealthController.getDomeMotorMoving() || stealthController.getLegMotorsMoving())
        digitalWrite(LED_BUILTIN, HIGH);
    else
        digitalWrite(LED_BUILTIN, LOW);
    static uint32_t domeMotorStartMillis;
    if (stealthController.getStatusChanged())
    {
        if (stealthController.didDomeMotorStart())
        {
            Marcduino::send(F("$DSTART"));
            domeMotorStartMillis = millis();
        }
        else if (stealthController.didDomeMotorStop())
        {
            Marcduino::send(F("$DSTOP"));
            domeMotorStartMillis = 0;
        }
        if (stealthController.didLegMotorsStart())
        {
            Marcduino::send(F("$DRSTART"));
        }
        else if (stealthController.didLegMotorsStop())
        {
            Marcduino::send(F("$DRSTOP"));
        }
    }
    else if (domeMotorStartMillis != 0 && domeMotorStartMillis + 500 < millis())
    {
        Marcduino::send(F("$DRUN"));
        domeMotorStartMillis = 0;
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
