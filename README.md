# Simple Makefile based build and deploy environment for Arduino based Projects #

Uses arduino-builder. Presently only the Arduino Mega and Arduino ProMini have been tested.

### Directory structure

Directory structure of a sample project

	Arduino.mk
	Makefile
	MySketch
		[<hostname>.mk] optional
		Makefile
		MySketch.ino
	MyOtherSketch
		[<hostname>.mk] optional
		Makefile
		MySketch.ino

The Sketch/Makefile could look like this:

	PORT=/dev/ttyUSB0
	TARGET=ProMini
	GITHUB_REPOS=adafruit/Adafruit_Neopixel adafruit/Adafruit_Sensor
	include ../Arduino.mk

The <hostname>.mk file can be used to specify build host specific options such as PORT

	PORT=/dev/ttyACM0


### Clean build and upload

	$ make clean build upload && tio -b 115200 /dev/ttyUSB0

### Verbose mode

	$ make clean build upload VERBOSE=1

### Silent mode

	$ make clean build upload SILENT=1

### Refresh github libraries

	$ make github_pull

### What is tio?

tio is a simple TTY terminal emulator. You can install tio by doing:

sudo apt install tio

Or build it from source https://github.com/tio/tio 
