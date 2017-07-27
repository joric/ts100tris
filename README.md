ts100tris
=========

This is an experimental branch, for the original project see https://github.com/joric/ts100tris

Scripting
---------

What I've tried (in order of appearance):

* eLUA, apparently memory footprint is "[at least 256 kbytes of flash memory, and at least 64 kbytes of RAM](http://www.electronicdesign.com/dev-tools/running-embedded-lua-microcontrollers)" and it simply doesn't fit into 64kb ROM and 20 kb RAM of STM32F103T8U6.

* Micropython is "[80K of ARM Thumb2 code and 8KB is minimal amount to run simple scripts](https://github.com/micropython/micropython/wiki/FAQ)". Way too large, especially considering that existing firmware with USB drive and FAT12 support occupies most of available 64KB (uploading scripts via UART is way less convenient).

* PyMite, I've tried [older versions of PyMite](https://code.google.com/archive/p/python-on-a-chip/downloads) but they still occupy too much space (and scripts need precompilation). So, I guess, we stuck with native C (or smaller interpreters).

This branch uses AMX (Counter Strike) scripting and Pawn virtual machine written in pure assembly.

I had to cut stock settings menu, despite the engine is quite small (about 5 kb of Thumb-2 code).

Hold button B to run precompiled AUTOEXEC.AMX uploaded to the USB drive (see the scripts directory).

There should be enough space on the USB drive to replace the entire menu with scripting.

