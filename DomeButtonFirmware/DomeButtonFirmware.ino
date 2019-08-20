#undef USE_NEOPSI
#define USE_DEBUG
#define DBC_TEECES_L_PIN 8
#include "ReelTwo.h"	/* include first to enable SMQ support */
#ifndef USE_NEOPSI
#include "dome/TeecesPSI.h"
#else
#define DBC_TEECES_L_PIN 8
#include "dome/NeoPSI.h"
#endif
#include "dome/ButtonController.h"

#ifndef USE_NEOPSI
LedControlMAX7221<1> teecesChain(6, 7, 8);
TeecesPSI rearPSI(teecesChain);
#else
NeoRearPSI<> rearPSI(DBC_TEECES_L_PIN, LogicEngineRearPSIDefault);
#endif

ButtonController controller([] (byte color, int rate) {
	if (!color)
	{
		// LEDs OFF
		DEBUG_PRINTLN("LED OFF");
	#ifndef USE_NEOPSI
		rearPSI.setSolidState(0);
	#else
    		rearPSI.selectSequence(LogicEngineDefaults::LIGHTSOUT);
	#endif
	}    
	else if (rate == ON)
	{
		// ALWAYS ON
		if (color == PSI_YELLOW)
		{
			// ALWAYS ON YELLOW
			DEBUG_PRINTLN("PSI_YELLOW");
		#ifndef USE_NEOPSI
			rearPSI.setState(6);
		#else
			rearPSI.selectSequence(LogicEngineDefaults::SOLIDCOLOR, LogicEngineRenderer::kYellow);
		#endif
		}
		else if (color == PSI_GREEN)
		{
			// ALWAYS ON GREEN
			DEBUG_PRINTLN("PSI_GREEN");
		#ifndef USE_NEOPSI
			rearPSI.setState(0);
		#else
                        rearPSI.selectSequence(LogicEngineDefaults::SOLIDCOLOR, LogicEngineRenderer::kGreen);
		#endif
		}
		else if (color == PSI_BOTH)
		{
			DEBUG_PRINTLN("PSI_BOTH");
		#ifndef USE_NEOPSI
			rearPSI.setSolidState(~0);
		#else
    			rearPSI.selectSequence(LogicEngineDefaults::NORMAL);
		#endif
		}
	}
	else
	{
	#ifndef USE_NEOPSI
		rearPSI.setAnimate(ON);
	#else
		//rearPSI.selectSequence(LogicEngineDefaults::PSICOLORWIPE, LogicEngineRenderer::kYellow);
	#endif
	}
});

void setup()
{
        REELTWO_READY();
	SetupEvent::ready();
#ifdef USE_NEOPSI
        rearPSI.selectSequence(LogicEngineDefaults::NORMAL);
#endif
}

void loop()
{
	AnimatedEvent::process();
}
