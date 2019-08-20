#include "ReelTwo.h"
//#include "ReelTwoSMQ.h"	/* include first to enable SMQ support */
#include "i2c/I2CReceiver.h"
#include "dome/MagicPanel.h"

MagicPanel magicPanel;
I2CReceiver i2cReceiver(I2C_MAGIC_PANEL);

void setup()
{
	REELTWO_READY();
	SetupEvent::ready();
}

void loop()
{
	AnimatedEvent::process();
}
