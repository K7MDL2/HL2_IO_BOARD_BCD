# HL2_IO_BOARD_BCD
BCD and PTT outputs for the HL2 IO board designed by N2ADR.  Derived from the N2ADR basic example, this is VHF band focused, modify to suit your needs.

Original source at https://github.com/jimahlstrom/HL2IOBoard

In my main.c here I produce 4-line BCD patterns derived from the TX frequency sent to the IO board via i2c from a compatible SDR app. It also monitors EXTTR on the HL2 IO header and repeats it through, buffered by one of the transistors on board. All 5 signals I wire to the DB9 to my Q5 5-band transverter (144, 222, 432, 903, 1296), or external band decoder (see my other projects).

I have the compiled UF2 file so you do not need to compile this.  Just drop it into the Boot file manager window as normal for Pico uploads.

The code example has a list of VHF frequencies combined with forcing Alt-RF use for RX (for split IF) on VHF bands and ANT for RX on the HF bands. This way you can do HF as normal and then switch a VHF band which will use Alt-RX and RF1 low power output (+17dBm) for a spif IF transvter which has its own antenna.  

You can change the frequency list very easily as well as the specific pattern for each band you desire.  

Thetis for HL2 is pretty flexible with per band settings so you may not need to force the VHF band behavior.  PiHPSDR also.   I wrote a tutorial for setting up Thetis for the HL2 and transverters using split IF.  If this is done correctly, it will match what may firmware will do and won't fight for control of the relays and work properly.  For other apps without such IO board per-band knowledge, the firmare will do the right thing for you.

In this version the BCD will only match the TX frequency as I found most SDR apps when I wrote this did not send RX_Freq info out yet.

The Thetis tutorial is here:
https://github.com/K7MDL2/HL2_IO_BOARD_BCD/wiki/Thetis-Configuration-for-Transverters-with-Split-IF


## Arduino IDE option

On the original repo there are several user solutions added since I built this C version.  

One of then is ZL2TE_Arduino.  As of Feb 2026 I was able to compile and run with 1 change to a header file to fix a path to an include file.  I used Arduino IDE v2.3.7 and the current ARduino-Pico board package which includes Pico-SDK 2.2.0.

In the file ioboard.h replace this line 
#include <pico/i2c_slave.h>
With this line
#include <../../pico_i2c_slave/include/pico/i2c_slave.h>

It should now compile adn produce a UF2 image file.

I added my VHF band code to ZL2TE_Arduino.ino.  There is now a #define to choose the original HF band decoder and my VHF bands decoder.

#define VHF_BANDS   // use VHF bands instead of HF.

I have my edited versions of the 2 files in a zip.  Replace your files with these and set teh #define as desired.
