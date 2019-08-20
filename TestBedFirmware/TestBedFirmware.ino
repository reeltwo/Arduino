//#define USE_DEBUG
//#define USE_SERVO_DEBUG
//#define USE_VERBOSE_SERVO_DEBUG
//#define USE_SMQDEBUG
#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "dome/BadMotivator.h"
#include "dome/HoloLights.h"
#include "dome/HoloDisplay.h"
#include "dome/Logics.h"
#include "dome/TeecesPSI.h"
#include "dome/FireStrip.h"
#include "i2c/PeriscopeI2C.h"
#include "i2c/MagicPanelI2C.h"
#include "core/Marcduino.h"

#include "body/DataPanel.h"
#include "body/ChargeBayIndicator.h"

#include "MicrophoneAmplitude.h"
#include "Orientation.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"

#include "core/JawaCommander.h"

#define CBI_DATAIN_PIN 10 // 13 //2 
#define CBI_CLOCK_PIN  11 // 12 //4
#define CBI_LOAD_PIN   12 // 11 //8

#define SMALL_PANEL        0x0001
#define MEDIUM_PANEL       0x0002
#define BIG_PANEL          0x0004
#define PIE_PANEL          0x0008
#define TOP_PIE_PANEL      0x0010
#define MINI_PANEL         0x0020

#define HOLO_HSERVO        0x1000
#define HOLO_VSERVO        0x2000

#define DOME_PANELS_MASK        (SMALL_PANEL|MEDIUM_PANEL|BIG_PANEL)
#define PIE_PANELS_MASK         (PIE_PANEL)
#define ALL_DOME_PANELS_MASK    (MINI_PANEL|DOME_PANELS_MASK|PIE_PANELS_MASK|TOP_PIE_PANEL)
#define HOLO_SERVOS_MASK        (HOLO_HSERVO|HOLO_VSERVO)

#define PANEL_GROUP_1      (1L<<14)
#define PANEL_GROUP_2      (1L<<15)
#define PANEL_GROUP_3      (1L<<16)
#define PANEL_GROUP_4      (1L<<17)
#define PANEL_GROUP_5      (1L<<18)
#define PANEL_GROUP_6      (1L<<19)
#define PANEL_GROUP_7      (1L<<20)
#define PANEL_GROUP_8      (1L<<21)
#define PANEL_GROUP_9      (1L<<22)
#define PANEL_GROUP_10     (1L<<23)

const ServoSettings servoSettings[] PROGMEM = {
    { 1,  1000, 1650, PANEL_GROUP_1|SMALL_PANEL|HOLO_HSERVO  },    /* 0: door 4 */
    { 2,  1500, 2300, PANEL_GROUP_2|SMALL_PANEL|HOLO_VSERVO  },    /* 1: door 3 */
    { 3,   900, 1650, PANEL_GROUP_3|SMALL_PANEL|HOLO_HSERVO  },    /* 2: door 2 */
    { 4,  1200, 1900, PANEL_GROUP_4|SMALL_PANEL|HOLO_VSERVO  },    /* 3: door 1 */
    { 5,  1200, 2000, PANEL_GROUP_5|MEDIUM_PANEL|HOLO_HSERVO },    /* 4: door 5 */
    { 7,  1200, 2000, PANEL_GROUP_6|BIG_PANEL|HOLO_VSERVO    },    /* 5: door 9 */
    { 8,  1275, 1975, PANEL_GROUP_7|MINI_PANEL               },    /* 6: mini door 2 */
    { 9,  1550, 1900, PANEL_GROUP_8|MINI_PANEL               },    /* 7: mini front psi door */
    { 10, 1250, 1900, PANEL_GROUP_9|PIE_PANEL                },    /* 8: pie panel 1 */
    { 0,     0,    0, 0  },  /* 9: unused */
    { 0,     0,    0, 0  },  /* 10: unused */
    { 0,     0,    0, 0  },  /* 11: unused */
    { 0,     0,    0, 0  },  /* 12: unused */
    { 0,     0,    0, 0  },  /* 13: unused */
    { 0,     0,    0, 0  },  /* 14: unused */
    { 0,     0,    0, 0  },  /* 15: unused */
    { 17, 1250, 1900, PANEL_GROUP_10|TOP_PIE_PANEL },  /* 16: pie panel 1 */
};
// Pump 8
BadMotivator badMotivator(8);
FireStrip fireStrip(9);
ServoDispatchFuzzyNoodlePCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
HoloLights frontHolo(2, HoloLights::kRGB);
HoloLights rearHolo(3, HoloLights::kRGB);
HoloLights topHolo(4, HoloLights::kRGB);
LogicEngineDeathStarFLD<> FLD(1, LogicEngineFLDDefault);
LogicEngineDeathStarRLDStaggerOdd<> RLD(2, LogicEngineRLDDefault);
JawaCommander<> jawaCommander;

LedControlMAX7221<3> ledChain1(CBI_DATAIN_PIN, CBI_CLOCK_PIN, CBI_LOAD_PIN);
ChargeBayIndicator chargeBayIndicator(ledChain1);
DataPanel dataPanel(ledChain1);
TeecesPSI somePSI(ledChain1);

AnimationPlayer player(servoSequencer);

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

SMQMESSAGE(ServoDispatch, {
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
})

////////////////////////////////////////////////////////////////////////////////

void setup()
{
    REELTWO_READY();

    Wire.begin();
    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    servoDispatch.setOutputEnablePin(7, false);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });

    frontHolo.assignServos(&servoDispatch, 0, 1);
    topHolo.assignServos(&servoDispatch, 2, 3);
    rearHolo.assignServos(&servoDispatch, 4, 5);

    SetupEvent::ready();

    servoDispatch.ensureEnabled();

    DelayCall::schedule([] { Marcduino::send(F("$73")); }, 1000);
}

////////////////

void resetSequence()
{
    Marcduino::send(F("$s"));
    CommandEvent::process(F( 
        "LE000000|0\n"
        "FSOFF\n"
        "BMOFF\n"
        "HPA000|0\n"
        "CB00000\n"
        "DP00000\n"));
}

////////////////

#include "MarcduinoPanel.h"

////////////////

#include "MarcduinoSequence.h"

////////////////

#include "MarcduinoLogics.h"

////////////////

#include "MarcduinoHolo.h"

////////////////

#include "MarcduinoMagicPanel.h"

////////////////

#include "MarcduinoPeriscope.h"

////////////////

MARCDUINO_ACTION(DirectCommand, *RT, ({
    // Direct ReelTwo command
    CommandEvent::process(Marcduino::getCommand());
}))

////////////////

void loop()
{
    AnimatedEvent::process();
}
