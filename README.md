# HL2_IO_BOARD_BCD
BCD and PTT outputs for the HL2 IO board designed by N2ADR.  Derived from the N2ADR basic example, this is VHF band focused, modify to suit your needs.

This code example I use to produce 4 line BCD patterns derived from the TX frequency sent to the IO board via i2c from a compatible SDR app. It also monitors EXTTR on the HL2 IO header and repeats it through.  All 5 signals I wire to the DB9 to my Q5 5-band transverter (144, 222, 432, 903, 1296).

The code example has a list of VHF frequencies combined with forcing Aux RF for RX on VHF bands and ANT for RX on the HF bands.  You can change the frequency list very easily as well as the specific pattern for each band you desire.  Thetis for HL2 is pretty flexible with per band settings so you may not need to force the VHF band behavior.  

In this version the BCD will only match the TX frequency as I found most SDR apps when I wrote this did not send RX_Freq info out yet.
