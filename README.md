ts100tris
=========

Tetris for TS100 soldering iron.

[![Tetris for TS100 soldering iron.](http://img.youtube.com/vi/Buzew1z1AhQ/0.jpg)](https://www.youtube.com/watch?v=Buzew1z1AhQ)

Built on top of an official minidso TS100 firmware starting from version 2.17.

Installation
------------
Download .hex file in the [releases section](https://github.com/joric/ts100tris/releases).

Connect to USB while holding button A, copy .hex to USB drive, wait it renames to .rdy, unplug USB.

There's no firmware backup, just revert to the official firmware the same way.


How to play
-----------

Hold button B to start the game. Press A or B to move. Hold A to drop, B to rotate, both buttons to quit.

Each time you lose the game the soldering iron tip temperature increases by 10 degrees.

Use 12..24V external power supply for the immersive gaming experience.


Build
-----

I used IAR Embedded Workbench for ARM EWARM v.7.50.2 10505.

Open .eww, hit Make, that's it.


Hardware
--------

* STM32F103T8U6 (ARM Cortex M3, clock frequency 72 MHz)

* MMA8652FC (3-Axis, 12-bit Digital Accelerometer)

* SSD1306 (White 96x16 OLED Display)

Disclaimer
----------

Accelerometer API doesn't determine tilt direction (left or right), so button controls only (yet).

This is very early alpha, currently works in USB mode only.

Scripting
---------

This version uses AMX (Counter Strike) scripting and Pawn virtual machine written in pure assembly.

Hold button B to run prebuilt AUTOEXEC.AMX uploaded to the USB drive (see the scripts directory).

I had to comment out settings menu despite the engine is quite small (about 5 kb of Thumb-2 code).

There should enough space on the USB drive to replace the entire menu with scripting.

