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

Hold button B to start the game.

Press A or B to move, A+B or hold B to rotate, hold A to drop, hold both buttons to quit.

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

This is very early alpha, currently works in USB mode only.

There's also an experimental [accelerometer](https://github.com/joric/ts100tris/tree/accelerometer) branch.

Press
-----

* [Hackaday: Tetris on a soldering iron](http://hackaday.com/2017/07/07/tetris-on-a-soldering-iron/)
* [Lenta.Ru: Программист запустил "Тетрис" на паяльнике](https://lenta.ru/news/2017/07/05/tetris/)
* [Vice: Play the Tiniest Version of 'Tetris' on a Soldering Iron](https://motherboard.vice.com/en_us/article/59pdpd/play-the-tiniest-version-of-tetris-on-a-soldering-iron)
* [Hacker News: Tetris for TS100 soldering iron](https://news.ycombinator.com/item?id=15172345)



