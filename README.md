# HL2_IO_BOARD_BCD
BCD and PTT outputs for the HL2 IO board designed by N2ADR.  VHF band focused, modify to suit your needs.

This code example I use to produce 4 line BCD patterns derived from the TX frequency sent to the IO board via i2c form a compatible SDR app. It also monitors EXTTR on the HL2 IO header and repeats it through.  All 5 signals I woire tot he DB9. 

My code example is a list of VHF frequencies combined with forcing Aux RF for RX on VHF bands and ANT for RX on the HF bands.  You can change the frequency list very easily as well as the specific patern for each band you desire.

In this version the BCD will only match the TX frequency as I found most SDR apps when I wrote this did not send RX_Freq info out yet.
