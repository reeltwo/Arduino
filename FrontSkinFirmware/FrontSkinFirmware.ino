// BODY PINOUT
// =======================================
// D20 - SDA (Blue)
// D21 - SCL (Purble)
// D3  - OUTPUT_ENABLED_PIN
// D8  - CBI_DATAIN_PIN
// D9  - CBI_CLOCK_PIN
// D10 - CBI_LOAD_PIN

//#define USE_DEBUG
// #define USE_SERVO_DEBUG
// #define USE_VERBOSE_SERVO_DEBUG
// #define USE_SMQDEBUG
#include "ReelTwoSMQ.h"  /* include first to enable SMQ support */
#include "ReelTwo.h"    /* include first to disable SMQ support */
#include "core/Animation.h"
#include "core/DelayCall.h"
#include "body/DataPanel.h"
#include "body/ChargeBayIndicator.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "body/Gripper.h"
#include "body/Welder.h"
#include "body/Zapper.h"

#include "core/Marcduino.h"
#include "core/JawaCommander.h"
#include "core/HeartBeat.h"

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
#define GROUP_ARMS       (GROUP_UPPERARM|GROUP_LOWERARM)

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

#define WELDER_ID1_PIN   38
#define WELDER_RELAY_PIN 46
#define ZAPPER_RELAY_PIN 36
#define ZAPPER_ID4_PIN   37

#define GRIP_AIN1_PIN    47
#define GRIP_AIN2_PIN    44
#define GRIP_PWMA_PIN    45
#define GRIP_STBY_PIN    42
#define GRIP_OPEN_PIN    41
#define GRIP_CLOSE_PIN   43
#define GRIPPER_ID2_PIN  39
#define UNUSED_ID3_PIN   40

const ServoSettings servoSettings[] PROGMEM = {
    // Servo #1 used for volt meter for now
    { 1,  870,  1690, GROUP_LEFTDOOR },    /* 0: left body door */
    { 2,  950,  1250, GROUP_DATAPANEL },   /* 1: data panel */
    { 3,  1500, 1000, 0 },                 /* 2: xchg arm rotate - servo reversed */
    { 4,  1920, 1000, GROUP_UPPERARM },    /* 3: upper utility arm - servo reversed */
    { 5,  1425,  900, 0 },                 /* 4: xchg arm lift - servo reversed */
    { 12, 1000, 1790, GROUP_CHARGEBAY },   /* 5: charge bay door */
    { 13, 1000, 1790, 0 },                 /* 6: cpu arm extend */
    { 14, 1790, 1000, 0 },                 /* 7: cpu arm lift - servo reversed */
    { 15, 1890, 1000, GROUP_LOWERARM },    /* 8: lower utlity arm - servo reversed */
    { 16, 1890, 1000, GROUP_RIGHTDOOR },   /* 9: right body door - servo reversed*/
};
ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);

AnimationPlayer player(servoSequencer);
JawaCommander<> jawaCommander;

LedControlMAX7221<3> ledChain1(CBI_DATAIN_PIN, CBI_CLOCK_PIN, CBI_LOAD_PIN);
ChargeBayIndicator chargeBayIndicator(ledChain1, A1);
DataPanel dataPanel(ledChain1);

Welder welder(WELDER_ID1_PIN, WELDER_RELAY_PIN);
Zapper zapper(ZAPPER_ID4_PIN, ZAPPER_RELAY_PIN);
Gripper gripper(GRIPPER_ID2_PIN, GRIP_AIN1_PIN, GRIP_AIN2_PIN, GRIP_PWMA_PIN, GRIP_STBY_PIN, GRIP_OPEN_PIN, GRIP_CLOSE_PIN);

void openUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 500, 1.0);
}

void closeUpperArm()
{
    servoDispatch.moveTo(UPPER_ARM, 150, 500, 0.0);
}

void openLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 500, 1.0);
}

void closeLowerArm()
{
    servoDispatch.moveTo(LOWER_ARM, 150, 500, 0.0);
}

void openLeftBodyDoor()
{
    servoDispatch.moveTo(DOOR_LEFT, 150, 500, 1.0);
}

void closeLeftBodyDoor()
{
    servoDispatch.moveTo(DOOR_LEFT, 150, 500, 0.0);
}

void openRightBodyDoor()
{
    servoDispatch.moveTo(DOOR_RIGHT, 150, 500, 1.0);
}

void closeRightBodyDoor()
{
    servoDispatch.moveTo(DOOR_RIGHT, 150, 500, 0.0);
}

void openDataPanelDoor()
{
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 500, 1.0);
    dataPanel.setSequence(DataPanel::kNormal);
}

void closeDataPanelDoor()
{
    servoDispatch.moveTo(DOOR_DATAPANEL, 150, 500, 0.0);
    dataPanel.setSequence(DataPanel::kDisabled);
}

void openChargeBayDoor()
{
    servoDispatch.moveTo(DOOR_CHARGEBAY, 150, 500, 1.0);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kNormal);
}

void closeChargeBayDoor()
{
    servoDispatch.moveTo(DOOR_CHARGEBAY, 150, 500, 0.0);
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
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 100, 0.0);
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1.0);
    }, 3000)
    DO_ONCE({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1.0);
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseCPUARM, :SE17)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 0.0);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 0.0);
    }, 2000)
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
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 1.0);
    }, 3000)
    DO_ONCE({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    })
    DO_END()
}

MARCDUINO_ANIMATION(CloseXCHGARM, :SE19)
{
    DO_START()
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1.0);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    }, 1000)
    DO_ONCE_AND_WAIT({
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 0.0);
    }, 2000)
    DO_ONCE({
        closeLeftBodyDoor();
    })
    DO_END()
}

MARCDUINO_ACTION(FlutterAllPanels, :OF00, ({
    SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelAllFlutter, GROUP_DOORS, 10, 50);
}))

MARCDUINO_ACTION(OpenAllPanels, :OP00, ({
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllOpen, GROUP_DOORS, 500);
}))

MARCDUINO_ACTION(CloseAllPanels, :CL00, ({
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, GROUP_DOORS, 500);
}))

void openCPUARM()
{
    openRightBodyDoor();
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 100, 0.0);
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 1.0);
    }, 1000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 1.0);
    }, 3000);
}

void closeCPUARM()
{
    servoDispatch.moveTo(CPUARM_EXTEND, 150, 1000, 0.0);
    DelayCall::schedule([] {
        servoDispatch.moveTo(CPUARM_LIFT, 150, 2000, 0.0);
    }, 1000);
    DelayCall::schedule([] {
        closeRightBodyDoor();
    }, 4000);
}

void openXCHGARM()
{
    servoDispatch.setServoEasingMethod(XCHGARM_ROTATE, Easing::BounceEaseOut);
    openLeftBodyDoor();
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 1.0);
    }, 1000);
    DelayCall::schedule([] {
        if (gripper.isAttached())
        {
            gripper.gripperArm();
        }
        if (zapper.isAttached())
        {
            zapper.zapperArm();
        }
        if (welder.isAttached())
        {
            welder.welderArm();
        }
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1.0);
        if (welder.isAttached())
        {
            welder.welderOn();
        }
        if (gripper.isAttached())
        {
            gripper.gripperOn();
        }
        if (zapper.isAttached())
        {
            zapper.zapperOn();
        }
    }, 3000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    }, 4000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1.0);
    }, 5000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    }, 6000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1.0);
    }, 7000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    }, 8000);
}

void closeXCHGARM()
{
    servoDispatch.setServoEasingMethod(XCHGARM_ROTATE, Easing::CircularEaseIn);
    gripper.gripperDisarm();
    zapper.zapperDisarm();
    welder.welderDisarm();
    servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 1.0);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_ROTATE, 150, 1000, 0.0);
    }, 1000);
    DelayCall::schedule([] {
        servoDispatch.moveTo(XCHGARM_LIFT, 150, 2000, 0.0);
    }, 2000);
    DelayCall::schedule([] {
        closeLeftBodyDoor();
    }, 5000);
}

void openAllPanels()
{
    dataPanel.setSequence(DataPanel::kNormal);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kNormal);
    servoDispatch.setServosEasingMethod(GROUP_RIGHTDOOR, Easing::CircularEaseIn);
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllOpen, GROUP_DOORS, 4000);
}

void closeAllPanels()
{
    servoDispatch.setServosEasingMethod(GROUP_RIGHTDOOR, Easing::BounceEaseOut);
    SEQUENCE_PLAY_ONCE_SPEED(servoSequencer, SeqPanelAllClose, GROUP_DOORS, 1000);
    DelayCall::schedule([] {
        dataPanel.setSequence(DataPanel::kDisabled);
        chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);
    }, 2000);
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

SMQMESSAGE(BodyPanelGroupEasing, {
    uint32_t groupMask = msg.get_integer(MSGID("mask"));
    uint32_t easing = msg.get_integer(MSGID("easing"));
    if (groupMask != 0)
    {
        servoDispatch.setServosEasingMethod(groupMask, Easing::getEasingMethod(easing));
    }
})

SMQMESSAGE(BodyPanelGroupMove, {
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

void zapZap()
{
    DEBUG_PRINTLN("ZAP ZAP");
    zapper.zapperOn();
}

void weldWeld()
{
    DEBUG_PRINTLN("WELD WELD");
    welder.welderOn();
}

void gripGrip()
{
    DEBUG_PRINTLN("GRIP GRIP");
    gripper.gripperOn();
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
    REELTWO_READY();

    Wire.begin();
    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    //servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });
    // Default all servos to bounce
    // Change arms to quartic

    SetupEvent::ready();

    dataPanel.setSequence(DataPanel::kDisabled);
    chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);

    servoDispatch.setServosEasingMethod(GROUP_DOORS|GROUP_ARMS, Easing::QuarticEaseInOut);
    // closeCPUARM();

    DelayCall::schedule([] { Marcduino::send(F("$84")); }, 2000);
    // DelayCall::schedule([] { openLowerArm(); }, 4000);
    // DelayCall::schedule([] { closeLowerArm(); }, 8000);

    // DelayCall::schedule([] { openAllPanels(); }, 4000);
    // DelayCall::schedule([] { closeAllPanels(); }, 10000);

    if (gripper.isAttached())
        DEBUG_PRINTLN("GRIPPER ATTACHED");
    else
        DEBUG_PRINTLN("NO GRIPPER");

    if (zapper.isAttached())
        DEBUG_PRINTLN("ZAPPER ATTACHED");
    else
        DEBUG_PRINTLN("NO ZAPPER");

    if (welder.isAttached())
        DEBUG_PRINTLN("WELDER ATTACHED");
    else
        DEBUG_PRINTLN("NO WELDER");

    if (gripper.isGripperOpen())
        DEBUG_PRINTLN("GRIPPER OPEN");
    if (gripper.isGripperClosed())
        DEBUG_PRINTLN("GRIPPER CLOSED");
}

void closeArms()
{
    servoDispatch.setServoEasingMethod(UPPER_ARM, Easing::LinearInterpolation);
    servoDispatch.setServoEasingMethod(LOWER_ARM, Easing::LinearInterpolation);
    servoDispatch.moveTo(LOWER_ARM, 150, 2000, 0.0);
    servoDispatch.moveTo(UPPER_ARM, 150, 2000, 0.0);
    DelayCall::schedule([] { openArmTest(); }, 4000);
}

void openArmTest()
{
    static uint8_t currentEasingMethod = 0;
    Easing::Method method = Easing::getEasingMethod(currentEasingMethod);
    if (method != NULL)
    {
        DEBUG_PRINT("EASING METHOD: "); DEBUG_PRINTLN(currentEasingMethod);
        servoDispatch.setServoEasingMethod(UPPER_ARM, method);
        servoDispatch.setServoEasingMethod(LOWER_ARM, method);
        servoDispatch.moveTo(UPPER_ARM, 150, 2000, 1.0);
        servoDispatch.moveTo(LOWER_ARM, 150, 2000, 1.0);
        currentEasingMethod++;
        DelayCall::schedule([] { closeArms(); }, 4000);
    }
}

void loop()
{
    AnimatedEvent::process();

#ifdef USE_DEBUG
    if (DEBUG_SERIAL.available())
    {
        int ch = DEBUG_SERIAL.read();
        if (ch == 'o')
        {
            openAllPanels();
            // openXCHGARM();
        }
        else if (ch == '1')
        {
            openXCHGARM();
            //openArmTest();
        }
        else if (ch == '2')
        {
            closeXCHGARM();
            // servoDispatch.setServoEasingMethod(UPPER_ARM, Easing::LinearInterpolation);
            // servoDispatch.setServoEasingMethod(LOWER_ARM, Easing::LinearInterpolation);
            // servoDispatch.moveTo(LOWER_ARM, 150, 2000, 0.0);
            // servoDispatch.moveTo(UPPER_ARM, 150, 2000, 0.0);
        }
        else if (ch == 'e')
        {
            servoDispatch.setServosEasingMethod(GROUP_RIGHTDOOR, Easing::CircularEaseIn);
            SEQUENCE_PLAY_ONCE_VARSPEED(servoSequencer, SeqPanelMarchingAnts, GROUP_DOORS, 50, 100);
        }
        else if (ch == 'f')
        {
            SEQUENCE_PLAY_ONCE_VARSPEED_EASING(servoSequencer, SeqPanelMarchingAnts, GROUP_DOORS, 500, 1000, Easing::CircularEaseIn, Easing::BounceEaseOut);
        }
        else if (ch == 'c')
        {
            closeAllPanels();
            // closeXCHGARM();
        }
        else
        {
            DEBUG_PRINTLN("STOP ALL SERVOS");
            servoDispatch.stop();
        }
    }
#endif
    sendHeartBeat("FS", 1000);
}
