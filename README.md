# AdafruitMiniTTLCamera

## About ##

C++ library tested on PocketBeagel platform for the Adafruit Miniature TTL Serial JPEG Camera with NTSC Video PRODUCT ID: 1386
https://www.adafruit.com/product/1386#technical-details. It should work on any UNIX system that supports Termios.

This sensor communicates using Universal Asynchronous Receiver/Transmitter. A UART library was created using a combination of the structure from https://github.com/LukeNow/BBB-UART-anybaud and some of the Termios UART commands from https://github.com/AkshayPatwardhan/BBB-UART/. 

The rest of the code has been created with inspiration from Adafruit's library:
https://github.com/adafruit/Adafruit-VC0706-Serial-Camera-Library Especially the arduino example in it: https://github.com/adafruit/Adafruit-VC0706-Serial-Camera-Library/tree/master/examples/Snapshot

## Status / Known bugs ##

Can communicate with the camera although taking pictures has not been reliable so far.

## Using this library ##

Copy the all files from the source and include folders to your project and build it.

The main.cpp source file contains a terminal program used to test the camera.

The CameraMediator class contains all the business logic for the camera.

The UART class contains the logic for serial communication.

## Further reading ## 

Other projects with camera: 

https://cassiopeia.hk/wificam/

Adafruit camera tutorial: 

https://learn.adafruit.com/ttl-serial-camera/

An interesting library for Beagle Bone: 

https://github.com/yigityuce/BlackLib

Overview of Termios: 

https://en.wikibooks.org/wiki/Serial_Programming/termios
