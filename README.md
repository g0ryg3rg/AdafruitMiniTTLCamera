# AdafruitMiniTTLCamera

## About ##

C++ library tested on PocketBeagel platform for the Adafruit Miniature TTL Serial JPEG Camera with NTSC Video PRODUCT ID: 1386
https://www.adafruit.com/product/1386#technical-details. It should work on any UNIX system that supports Termios.

This sensor communicates using Universal Asynchronous Receiver/Transmitter. A UART library was created usingng a combination of the structure from https://github.com/LukeNow/BBB-UART-anybaud and some of the Termios UART commands from https://github.com/AkshayPatwardhan/BBB-UART/. 

## Using this library ##

Copy the all files from the source and include folders to your project and build it.

The CameraMediator class contains all the business logic for the camera.






