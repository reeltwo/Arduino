#define USE_DEBUG
#define USE_SERVO_DEBUG

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


// D53 - SDCS (SC) (Gray Wire)
// D47 - OCS (OC) (Orange Wire)
// D49 - RST (R) (Green Wire)
// D48 - D/C (DC) (Brown Wire)
// D52 - SCK (CK) (White Wire)
// D51 - MOSI (SI)(Blue Wire)
// D50 - MISO (SO) (Yellow Wire)

//#include "ReelTwoSMQ.h"    /* include first to enable SMQ support */
#include "ReelTwo.h"
#include "dome/HoloLights.h"
#include "dome/HoloDisplay.h"
#include "dome/Logics.h"
#include "MicrophoneAmplitude.h"
#include "i2c/PeriscopeI2C.h"
#include "i2c/MagicPanelI2C.h"
#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "Orientation.h"
//#include "ChargeBayIndicator.h"
#include "core/DelayCall.h"

#define HP_FRONT_LED_PIN    45
#define HP_REAR_LED_PIN     2
#define HP_TOP_LED_PIN      3
#define RADAR_EYE_LED_PIN   4
// D5  - FRONT_LOGIC_PIN
// D6  - REAR_LOGIC
#define OUTPUT_ENABLED_PIN  7
#define SMOKE_PIN           8

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
#define HOLO_SERVOS_MASK        (HOLO_HSERVO|HOLO_VSERVO)

const ServoSettings servoSettings[] PROGMEM = {
    { 1,  SMALL_PANEL,   1000, 1650 },  /* 0: door 4 */
    { 2,  SMALL_PANEL,   1500, 2300 },  /* 1: door 3 */
    { 4,  SMALL_PANEL,    900, 1650 },  /* 2: door 2 */
    { 6,  SMALL_PANEL,   1200, 1900 },  /* 3: door 1 */
    { 17, MEDIUM_PANEL,  1300, 1900 },  /* 4: door 5 */
    { 9,  BIG_PANEL,     1200, 2000 },  /* 5: door 9 */
    { 8,  MINI_PANEL,    1275, 1975 },  /* 6: mini door 2 */
    { 7,  MINI_PANEL,    1550, 1900 },  /* 7: mini front psi door */
    { 3,  PIE_PANEL,     1250, 1900 },  /* 8: pie panel 1 */
    { 10, PIE_PANEL,     1075, 1700 },  /* 9: pie panel 2 */
    { 11, PIE_PANEL,     1200, 2000 },  /* 10: pie panel 3 */
    { 12, PIE_PANEL,      750, 1450 },  /* 11: pie panel 4 */
    { 5,  TOP_PIE_PANEL, 1250, 1850 },  /* 12: dome top panel */
    { 13, HOLO_HSERVO,    800, 1600 },  /* 13: horizontal front holo */
    { 14, HOLO_VSERVO,    800, 1800 },  /* 14: vertical front holo */
    { 15, HOLO_HSERVO,    800, 1600 },  /* 15: horizontal top holo */
    { 16, HOLO_VSERVO,    800, 1325 },  /* 16: vertical top holo */
    { 25, HOLO_VSERVO,    900, 1000 },  /* 17: vertical rear holo */
    { 26, HOLO_HSERVO,   1300, 1600 },  /* 18: horizontal rear holo */
};
ServoDispatchFuzzyNoodlePCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
HoloDisplay frontHolo(HoloLights::kRGBW, HoloLights::kFrontHolo);
HoloLights rearHolo(HP_REAR_LED_PIN, HoloLights::kRGBW, HoloLights::kRearHolo);
HoloLights topHolo(HP_TOP_LED_PIN, HoloLights::kRGBW, HoloLights::kTopHolo, 12);
HoloLights radarEye(RADAR_EYE_LED_PIN, HoloLights::kRGBW, HoloLights::kRadarEye, 24);
MicrophoneAmplitude mic;
// Front Logic Device (Jawa ID#1)
LogicEngineDeathStarFLD<> FLD(1, LogicEngineFLDDefault);
// Rear Logic Device (Jawa ID#2)
LogicEngineDeathStarRLDInverted<> RLD(2, LogicEngineRLDDefault);
PeriscopeI2C periscope;
MagicPanelI2C magicPanel;
Orientation orientation;
//ChargeBayIndicator cbi;

#if 0
Teeces<1> teecesChain(6, 7, 8);
PSI frontPSI = teecesChain[0];
PSI rearPSI = teecesChain[1];
PSI rearPSI2 = teecesChain[2];
#endif 

// TODO RAISE DOME WHEEL SLIGHTLY
#if 0
class CustomCommand : public CommandEvent
{
public:
    virtual void handleCommand(const char* cmd) override
    {
        
    }
void openAllDoors()
{
    for (int i = 0; i < 13; i++)
        servoDispatch.moveTo(i, random(150,400), 700);
}

void closeAllDoors()
{
    for (int i = 0; i < 13; i++)
       servoDispatch.moveTo(i, random(150,400), 2400);
}

void openPieDoors()
{
    for (int i = 8; i < 12; i++)
       servoDispatch.moveTo(i, random(150,400), 700);
}

void closePieDoors()
{
    for (int i = 8; i < 12; i++)
       servoDispatch.moveTo(i, random(150,400), 2400);
}

void openWaveDoors()
{
    for (int i = 0; i < 13; i++)
       servoDispatch.moveTo(i, i*100, 50, 700);
    DelayCall::schedule(closeWaveDoors, 13*100+50);
}

void closeWaveDoors()
{
    for (int i = 0; i < 13; i++)
       servoDispatch.moveTo(i, i*100, 50, 2400);
}
#endif

void setup()
{
    REELTWO_READY();

    Wire.begin();
    Wire.setClock(400000); //Set i2c frequency to 400 kHz.

    randomSeed(analogRead(3));

    servoDispatch.setOutputEnablePin(OUTPUT_ENABLED_PIN);
    servoDispatch.setClockCalibration((const uint32_t[]) { 27570000, 27190000 });

    frontHolo.assignServos(&servoDispatch, 13, 14);
    topHolo.assignServos(&servoDispatch, 15, 16);
    rearHolo.assignServos(&servoDispatch, 17, 18);

    FLD.setPeakValueProvider(mic);
    RLD.setPeakValueProvider(mic);

    SetupEvent::ready();

    frontHolo.selectSequence(1, 45);

    // servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 700);
    //servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 2400);

    // servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 150, 400, 700);
    // Open all servos in 4 seconds
    DelayCall::schedule([] { DEBUG_PRINTLN("OPEN"); servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 150, 100, 700); }, 4000);
    // Close all servos in 8 seconds
    DelayCall::schedule([] { DEBUG_PRINTLN("CLOSE"); servoDispatch.moveServosTo(ALL_DOME_PANELS_MASK, 150, 100, 2400); }, 8000);

    // DelayCall::schedule([] { servoDispatch.moveTo(1, 20, 1000); }, 4000);
    // DelayCall::schedule([] { servoSequencer.play(sSeqPanelDance, SizeOfArray(sSeqPanelDance)); }, 4000);

    // Close all servos in 4 seconds
//    DelayCall::schedule([] { servoDispatch.moveServosVarSpeed(DOME_PANEL_MASK, 150, 400, 2400); }, 4000);
}

void loop()
{
    static float heading = 0;
    heading += 0.1;
    if (orientation.getHeadingChanged(heading))
    {
//        servoDispatch.moveTo(12, 150, 1250 + int((1850.0 - 1250.0) * (heading / 360.0)));
    }
    AnimatedEvent::process();
}
