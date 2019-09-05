//#include "ReelTwoSMQ.h" /* include first to enable SMQ support */
#include "ReelTwo.h"
#include "i2c/I2CReceiver.h"
#include "dome/MagicPanel.h"

MagicPanel magicPanel;
I2CReceiver i2cReceiver(I2C_MAGIC_PANEL);

void setup()
{
	REELTWO_READY();

	SetupEvent::ready();

    // Game of Life
    magicPanel.setSequence(MagicPanel::kLife);
//    CommandEvent::process(F("MP70000"));
}

void loop()
{
	AnimatedEvent::process();
}
