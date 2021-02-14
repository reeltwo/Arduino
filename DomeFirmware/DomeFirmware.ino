//#define USE_DEBUG
// #define USE_SERVO_DEBUG

// PINOUT
// =======================================
// D20 - SDA (Blue)
// D21 - SCL (Purble)
// A8 - Rear Holo Pos Vertical/Horizontal?
// A9 - Rear Holo Pos Vertical/Horizontal?
// A10 - Top Holo Pos Vertical/Horizontal?
// A11 - Top Holo Pos Vertical/Horizontal?
// A12 - Big Door Pos
// A13 - Front Holo Touche Prob (non functional?)
// A14 - Front Holo Horizontal Pos
// A15 - Front Holo Vertical Pos

// D45 - HP_FRONT_LED_PIN
// D2  - HP_REAR_LED_PIN
// D3  - HP_TOP_LED_PIN
// D4  - RADAR_EYE_LED_PIN
// D5  - FRONT_LOGIC_PIN
// D6  - REAR_LOGIC
// D7  - OUTPUT_ENABLED_PIN
// D8  - SMOKE_PIN
// D9  - Rear Neo PSI
// D10 - Yoda Holo

// D53 - SDCS (SC) (Gray Wire)
// D47 - OCS (OC) (Orange Wire)
// D49 - RST (R) (Green Wire)
// D48 - D/C (DC) (Brown Wire)
// D52 - SCK (CK) (White Wire)
// D51 - MOSI (SI)(Blue Wire)
// D50 - MISO (SO) (Yellow Wire)

#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"
#include "dome/HoloLights.h"
#include "dome/HoloDisplay.h"
#include "dome/Logics.h"
#include "dome/NeoPSI.h"
#include "dome/BadMotivator.h"
#include "MicrophoneAmplitude.h"
#include "i2c/PeriscopeI2C.h"
#include "i2c/MagicPanelI2C.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "ServoFeedback.h"
#include "Orientation.h"

#include "core/DelayCall.h"
#include "core/Marcduino.h"
#include "core/JawaCommander.h"
#include "core/HeartBeat.h"

#define HP_FRONT_LED_PIN    45
#define HP_REAR_LED_PIN     2
#define HP_TOP_LED_PIN      3
#define RADAR_EYE_LED_PIN   4
// D5  - FRONT_LOGIC_PIN
// D6  - REAR_LOGIC
#define OUTPUT_ENABLED_PIN  7
#define SMOKE_PIN           8
#define REAR_NEOPSI_PIN     9
#define HP_YODA_LED_PIN     10

#define SMALL_PANEL         0x0001
#define MEDIUM_PANEL        0x0002
#define BIG_PANEL           0x0004
#define PIE_PANEL           0x0008
#define TOP_PIE_PANEL       0x0010
#define MINI_PANEL          0x0020

#define HOLO_HSERVO         0x1000
#define HOLO_VSERVO         0x2000

#define DOME_PANELS_MASK        (SMALL_PANEL|MEDIUM_PANEL|BIG_PANEL)
#define PIE_PANELS_MASK         (PIE_PANEL)
#define ALL_DOME_PANELS_MASK    (MINI_PANEL|DOME_PANELS_MASK|PIE_PANELS_MASK|TOP_PIE_PANEL)
#define DOME_DANCE_PANELS_MASK  (DOME_PANELS_MASK|PIE_PANELS_MASK)
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
    { 1,  1650, 1000, PANEL_GROUP_4|SMALL_PANEL },  /* 0: door 4 */
    { 2,  2300, 1500, PANEL_GROUP_3|SMALL_PANEL },  /* 1: door 3 */
    { 4,  1650, 900,  PANEL_GROUP_2|SMALL_PANEL },  /* 2: door 2 */
    { 6,  1950, 1200, PANEL_GROUP_1|SMALL_PANEL },  /* 3: door 1 */
    { 19, 1900, 1300, PANEL_GROUP_5|MEDIUM_PANEL }, /* 4: door 5 */
    { 9,  2000, 1200, PANEL_GROUP_6|BIG_PANEL },    /* 5: door 9 */
    { 8,  1975, 1275, MINI_PANEL },                 /* 6: mini door 2 */
    { 7,  1900, 1550, MINI_PANEL },                 /* 7: mini front psi door */
    { 3,  1900, 1250, PANEL_GROUP_10|PIE_PANEL },   /* 8: pie panel 1 */
    { 10, 1700, 1075, PANEL_GROUP_9|PIE_PANEL },    /* 9: pie panel 2 */
    { 11, 2000, 1200, PANEL_GROUP_8|PIE_PANEL },    /* 10: pie panel 3 */
    { 12, 1450,  750, PANEL_GROUP_7|PIE_PANEL },    /* 11: pie panel 4 */
    { 5,  1850, 1250, TOP_PIE_PANEL },              /* 12: dome top panel */
    { 23,  800, 1600, HOLO_HSERVO },                /* 13: horizontal front holo */
    { 22,  800, 1800, HOLO_VSERVO },                /* 14: vertical front holo */

    { 21,  800, 1600, HOLO_HSERVO },                /* 15: horizontal top holo */
    { 20,  800, 1325, HOLO_VSERVO },                /* 16: vertical top holo */

    { 18,  800, 1600, HOLO_VSERVO },                /* 17: vertical rear holo */
    { 17,  800, 1800, HOLO_HSERVO },                /* 18: horizontal rear holo */
};

const uint8_t servoFeedbackPins[] PROGMEM = {
    A8,     // Rear Holo Pos Vertical/Horizontal?
    A9,     // Rear Holo Pos Vertical/Horizontal?
    A10,    // Top Holo Pos Vertical/Horizontal?
    A11,    // Top Holo Pos Vertical/Horizontal?
    A12,    // Big Door Pos
    A14,    // Front Holo Horizontal Pos
    A15     // Front Holo Vertical Pos
};

ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
ServoFeedback<SizeOfArray(servoFeedbackPins)> servoFeedback(servoFeedbackPins);
HoloDisplay frontHolo(HoloLights::kRGBW, HoloLights::kFrontHolo);
HoloLights rearHolo(HP_REAR_LED_PIN, HoloLights::kRGBW, HoloLights::kRearHolo);
HoloLights topHolo(HP_TOP_LED_PIN, HoloLights::kRGBW, HoloLights::kTopHolo, 12);
HoloLights radarEye(RADAR_EYE_LED_PIN, HoloLights::kRGBW, HoloLights::kRadarEye, 24);
HoloLights yoda(HP_YODA_LED_PIN, HoloLights::kRGBW, HoloLights::kOtherHolo);
MicrophoneAmplitude mic;
LogicEngineDeathStarFLDInverted<> FLD(LogicEngineFLDDefault);
LogicEngineDeathStarRLDInverted<> RLD(LogicEngineRLDDefault);
PeriscopeI2C periscope;
MagicPanelI2C magicPanel;
Orientation orientation;
BadMotivator badMotivator(SMOKE_PIN);
NeoRearPSI<REAR_NEOPSI_PIN> rearPSI(LogicEngineRearPSIDefault);

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

SMQMESSAGE(DomePanelGroupEasing, {
    uint32_t groupMask = msg.get_integer(MSGID("mask"));
    uint32_t easing = msg.get_integer(MSGID("easing"));
    if (groupMask != 0)
    {
        servoDispatch.setServosEasingMethod(groupMask, Easing::getEasingMethod(easing));
    }
})

SMQMESSAGE(DomePanelGroupMove, {
    uint32_t groupMask = msg.get_integer(MSGID("mask"));
    uint32_t startDelay = msg.get_integer(MSGID("startDelay"));
    uint32_t moveTimeMin = msg.get_integer(MSGID("moveTimeMin"));
    uint32_t moveTimeMax = msg.get_integer(MSGID("moveTimeMax"));
    uint32_t easing = msg.get_integer(MSGID("easing"));
    float pos = msg.get_float(MSGID("pos"));
    if (groupMask != 0)
    {
        servoDispatch.setServosEasingMethod(groupMask, Easing::getEasingMethod(easing));
        servoDispatch.moveServosTo(groupMask, startDelay, moveTimeMin, moveTimeMax, pos);
    }
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

SMQMESSAGE(FACE, {
    float x = msg.get_float(MSGID("x"));
    float y = msg.get_float(MSGID("y"));
    float joy = msg.get_float(MSGID("joy"));
    frontHolo.setHoloPosition(x, y);
    rearHolo.setHoloPosition(x, y);
    topHolo.setHoloPosition(x, y);
})

////////////////////////////////////////////////////////////////////////////////

void setup()
{
    REELTWO_READY();

    Wire.begin();
    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    // servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN, true);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });
    servoDispatch.setServosEasingMethod(HOLO_SERVOS_MASK, Easing::CircularEaseIn);

    frontHolo.assignServos(&servoDispatch, 13, 14);
    topHolo.assignServos(&servoDispatch, 15, 16);
    rearHolo.assignServos(&servoDispatch, 17, 18);

    FLD.setPeakValueProvider(mic);
    RLD.setPeakValueProvider(mic);

    SetupEvent::ready();

//    DelayCall::schedule([] { Marcduino::send(F("$73")); }, 1000);

    frontHolo.play("LEIA.BD2");
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK);
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

#ifdef USE_DEBUG
    if (DEBUG_SERIAL.available())
    {
        int ch = DEBUG_SERIAL.read();
        if (ch == 'o')
        {
            servoDispatch.setServosEasingMethod(ALL_DOME_PANELS_MASK, Easing::CircularEaseIn);
            SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelWave, ALL_DOME_PANELS_MASK, 1000);
        }
        else if (ch == '1')
        {
            frontHolo.moveHP(HoloLights::kLowerLeft, 500);
        }
        else if (ch == '2')
        {
            frontHolo.moveHP(HoloLights::kDown, 500);
        }
        else if (ch == '3')
        {
            frontHolo.moveHP(HoloLights::kLowerRight, 500);
        }
        else if (ch == '4')
        {
            frontHolo.moveHP(HoloLights::kCenter, 500);
        }
        else if (ch == 'e')
        {
            // servoDispatch.setServosEasingMethod(GROUP_RIGHTDOOR, Easing::CircularEaseIn);
            SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK, 50, 100);
        }
        else if (ch == 'f')
        {
            SEQUENCE_PLAY_ONCE_VARSPEED_EASING(servoSequencer, SeqPanelMarchingAnts, ALL_DOME_PANELS_MASK, 500, 1000, Easing::CircularEaseIn, Easing::BounceEaseOut);
        }
        else if (ch == 'c')
        {
            servoDispatch.setServosEasingMethod(ALL_DOME_PANELS_MASK, Easing::BounceEaseOut);
            SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, ALL_DOME_PANELS_MASK, 1000);
        }
        else
        {
            DEBUG_PRINTLN("STOP ALL SERVOS");
            servoDispatch.stop();
        }
    }
#endif
    sendHeartBeat("DO",1000);
}
