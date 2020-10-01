// BODY PINOUT
// =======================================
// D20 - SDA (Blue)
// D21 - SCL (Purble)
// D3  - OUTPUT_ENABLED_PIN
// D8  - CBI_DATAIN_PIN
// D9  - CBI_CLOCK_PIN
// D10 - CBI_LOAD_PIN

#define USE_DEBUG
// #define USE_SERVO_DEBUG
// #define USE_VERBOSE_SERVO_DEBUG
// #define USE_SMQDEBUG
#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"    /* include first to disable SMQ support */
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "body/DataPanel.h"
#include "body/ChargeBayIndicator.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"

#include "core/Marcduino.h"
#include "core/JawaCommander.h"

#define OUTPUT_ENABLED_PIN  3
#define CBI_DATAIN_PIN      8
#define CBI_CLOCK_PIN       9
#define CBI_LOAD_PIN        10

#define GROUP_DOORS      0x000F
#define GROUP_DATAPANEL  0x0001
#define GROUP_CHARGEBAY  0x0002
#define GROUP_LEFTDOOR   0x0004
#define GROUP_RIGHTDOOR  0x0008
#define GROUP_UPPERARM   0x0010
#define GROUP_LOWERARM   0x0020

#define DOOR_LEFT       0
#define DOOR_DATAPANEL  1
#define XCHGARM_ROTATE  2
#define UPPER_ARM       3
#define XCHGARM_LIFT    4

#define DOOR_CHARGEBAY  5
#define CPUARM_EXTEND   6
#define CPUARM_LIFT     7
#define LOWER_ARM       8
#define DOOR_RIGHT      9

const ServoSettings servoSettings[] PROGMEM = {
    // Servo #1 used for volt meter for now
    { 1,  870,  1690, GROUP_LEFTDOOR },    /* 0: left body door */
    { 2,  950,  1250, GROUP_DATAPANEL },   /* 1: data panel */
    { 3,  1000, 1500, 0 },                 /* 2: xchg arm rotate */
    { 4,  1000, 1920, GROUP_UPPERARM },    /* 3: upper utility arm */
    { 5,   900, 1425, 0 },                 /* 4: xchg arm lift */
    { 12, 1000, 1790, GROUP_CHARGEBAY },   /* 5: charge bay door */
    { 13, 1000, 1790, 0 },                 /* 6: cpu arm extend */
    { 14, 1000, 1790, 0 },                 /* 7: cpu arm lift */
    { 15, 1000, 1890, GROUP_LOWERARM },    /* 8: lower utlity arm */
    { 16, 1000, 1790, GROUP_RIGHTDOOR },   /* 9: right body door */
};
ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);

AnimationPlayer player(servoSequencer);
JawaCommander<> jawaCommander;

LedControlMAX7221<3> ledChain1(CBI_DATAIN_PIN, CBI_CLOCK_PIN, CBI_LOAD_PIN);
ChargeBayIndicator chargeBayIndicator(ledChain1, A1);
DataPanel dataPanel(ledChain1);

void openUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 300, 1000);
}

void closeUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 300, 1920);
}

void openLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 300, 1000);
}

void closeLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 300, 1890);
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
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 100, 1300);
    dataPanel.setSequence(DataPanel::kNormal);
}

void closeDataPanelDoor()
{
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 100, 950);
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

////////////////

MARCDUINO_ACTION(StopSequence, :SE00, ({
}))

MARCDUINO_ANIMATION(OpenUtilityArms, :SE10)
{
    DO_START()
    DO_ONCE({
        openUpperArm();
        openLowerArm();
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseUtilityArms, :SE11)
{
    DO_START()
    DO_ONCE({
        closeUpperArm();
        closeLowerArm();
    })
    DO_END()
}

MARCDUINO_ANIMATION(OpenChargeBayDoor, :SE12)
{
    DO_START()
    DO_ONCE({
        openChargeBayDoor();
    })
    DO_END()
}

MARCDUINO_ANIMATION(OpenDataPanelDoor, :SE13)
{
    DO_START()
    DO_ONCE({
        openDataPanelDoor();
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseChargeBayDoor, :SE14)
{
    DO_START()
    DO_ONCE({
        closeChargeBayDoor();
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseDataPanelDoor, :SE15)
{
    DO_START()
    DO_ONCE({
        closeDataPanelDoor();
    })
    DO_END()
}

MARCDUINO_ANIMATION(OpenCPUARM, :SE16)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        openRightBodyDoor();
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 100, 1000);
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1000);
    }, 3000)
    DO_ONCE({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1750);
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseCPUARM, :SE17)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1000);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1750);
    }, 2000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1000);
    }, 1000)
    DO_ONCE({
        closeRightBodyDoor();
    })
    DO_END()
}

MARCDUINO_ANIMATION(OpenXCHGARM, :SE18)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        openLeftBodyDoor();
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 900);
    }, 3000)
    DO_ONCE({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1500);
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseXCHGARM, :SE19)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1000);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1500);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 1750);
    }, 2000)
    DO_ONCE({
        closeLeftBodyDoor();
    })
    DO_END()
}

// #define GROUP_DATAPANEL  0x0001
// #define GROUP_CHARGEBAY  0x0002
// #define GROUP_LEFTDOOR   0x0004
// #define GROUP_RIGHTDOOR  0x0008
// #define GROUP_UPPERARM   0x0010
// #define GROUP_LOWERARM   0x0020

static const ServoSequence SeqBodyPanelAllOpen PROGMEM =
{
    { 20,   B00010000, B00000000, B00000000, B00000000 },
};

static const ServoSequence SeqBodyPanelAllClose PROGMEM =
{
    { 20,   B11101111, B00000000, B00000000, B00000000 },
};

static const ServoSequence SeqBodyPanelAllFlutter PROGMEM =
{
    SEQUENCE_RANGE_LIMIT(200, 200)
    { 10,   B00010000, B00000000, B00000000, B00000000 },
    { 10,   B11101111, B00000000, B00000000, B00000000 },
    { 10,   B00010000, B00000000, B00000000, B00000000 },
    { 10,   B11101111, B00000000, B00000000, B00000000 },
    { 10,   B00010000, B00000000, B00000000, B00000000 },
    { 10,   B11101111, B00000000, B00000000, B00000000 },
    { 10,   B00010000, B00000000, B00000000, B00000000 },
    { 10,   B11101111, B00000000, B00000000, B00000000 },
    { 10,   B00010000, B00000000, B00000000, B00000000 },
    { 10,   B11101111, B00000000, B00000000, B00000000 },
};

MARCDUINO_ACTION(FlutterAllPanels, :OF00, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqBodyPanelAllFlutter, GROUP_DOORS, 10, 50);
}))

MARCDUINO_ACTION(OpenAllPanels, :OP00, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqBodyPanelAllOpen, GROUP_DOORS);
}))

MARCDUINO_ACTION(CloseAllPanels, :CL00, ({
    SEQUENCE_PLAY_ONCE(servoSequencer, SeqBodyPanelAllClose, GROUP_DOORS);
}))

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

    servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });

    SetupEvent::ready();

    dataPanel.setSequence(DataPanel::kDisabled);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);

    // closeCPUARM();

    DelayCall::schedule([] { Marcduino::send(F("$84")); }, 2000);
    DelayCall::schedule([] { openLowerArm(); }, 4000);
    DelayCall::schedule([] { closeLowerArm(); }, 8000);
}

void loop()
{
    AnimatedEvent::process();
}
