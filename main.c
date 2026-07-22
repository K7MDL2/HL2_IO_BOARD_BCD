// This is firmware for the Hermes Lite 2 IO board designed by Jim Ahlstrom, N2ADR. It is
//   Copyright (c) 2022-2023 James C. Ahlstrom <jahlstr@gmail.com>.
//   It is licensed under the MIT license. See MIT.txt.

// This firmware outputs the FT817 band voltage on J4 pin 8 and sets the band.

#include "../hl2ioboard.h"
#include "../i2c_registers.h"

#define DUAL_BAND_XVTR  // define this for the K7MDL custom 6M+2M dual band transverter box.  
// Uses GPIO12 (switched 5V), GPIO10_out5 for PTT, Out1 for Band_6M enable, Out2 for Band_2M enable. 
// Switched 5V is from a 74L05 so 100ma max. The xvtr box has 2x 5V relays at 33ma each.  They use TTL input control.
// All other bands the GPIOs are set to 0.
// When not defined, there are 4 band outputs and ptt.  Switched 5V is off.

// These are the major and minor version numbers for firmware. You must set these.
uint8_t firmware_version_major=1;
uint8_t firmware_version_minor=3;

void Xvtr_PTT(bool tx)
{
	// Operate PTT for transverter
	if (tx)
		gpio_put(GPIO10_Out5, 0);   // In RX, release PTT to Transverter
	else
		gpio_put(GPIO10_Out5, 1);   // Apply PTT to transverter
}

void Split_IF(void)  //  REG_RF_INPUTS = 1 for Split IF
{
	Registers[REG_RF_INPUTS] = 1;
	gpio_put(GPIO02_RF3, 1);
	gpio_put(GPIO03_INTTR, 1);
}

void Common_IF(void)   //  REG_RF_INPUTS = 0 for Common IF
{
	Registers[REG_RF_INPUTS] = 0;  
	gpio_put(GPIO02_RF3, 0);
	gpio_put(GPIO03_INTTR, 0);
}

int main()
{
	static uint8_t current_tx_fcode = 0;
	static uint8_t current_is_rx = 1;
	static uint8_t tx_band = 0;
	static uint8_t rx_band = 0;
	uint8_t band, fcode;
	uint8_t is_rx;
	uint8_t change_band;
	uint8_t i;

	stdio_init_all();
	configure_pins(0, 1);
	configure_led_flasher();

	while (1) {	// Wait for something to happen
		sleep_ms(1);	// This sets the polling frequency.
		// Control the Icom AH-4 antenna tuner.
		// Assume the START line is on J4 pin 6 and the KEY line is on J8 pin 2.
		IcomAh4(GPIO22_Out6, GPIO18_In2);
		// Poll for a changed Tx band, Rx band and T/R change
		change_band = 0;
		is_rx = gpio_get(GPIO13_EXTTR);		// true for receive, false for transmit
		if (current_is_rx != is_rx) {
			current_is_rx = is_rx;
			change_band = 1;
		}
		// Poll for a changed Tx frequency. The new_tx_fcode is set in the I2C handler.
		if (current_tx_fcode != new_tx_fcode) {
			current_tx_fcode = new_tx_fcode;
			change_band = 1;
			tx_band = fcode2band(current_tx_fcode);		// Convert the frequency code to a band code.
			ft817_band_volts(tx_band);			// Put the band voltage on J4 pin 8.
		}
		// Poll for a change in one of the twelve Rx frequencies. The rx_freq_changed is set in the I2C handler.
		if (rx_freq_changed) {
			rx_freq_changed = 0;
			change_band = 1;
			if (rx_freq_high == 0)
				rx_band = tx_band;
			else
				rx_band = fcode2band(rx_freq_high);	// Convert the frequency code to a band code.
		}

// ToDo:  Delete lines below once SDR apps catch up
tx_band = fcode2band(current_tx_fcode);
rx_band = tx_band;   // temp until SDR apps write to one or more o fhte 12possible RX freqyency registers

		if (change_band) {
			change_band = 0;
			if (tx_band == 0)	// Tx band zero is a reset
				band = 0;
			else if (current_is_rx)
				band = rx_band;
			else
				band = tx_band;

			switch (band) {		// Set some GPIO pins according to the band.
			#ifndef DUAL_BAND_XVTR   // normal K3 emulation for VHF bands  Sw5 is not used or wired
				case BAND_6:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 0);
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 0);
					Xvtr_PTT(current_is_rx);
					break;
				case BAND_2:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 1);
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 1);
					gpio_put(GPIO11_Out4, 1);
					Xvtr_PTT(current_is_rx);
					break;				
				case BAND_125cm:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 0);
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 1);
					gpio_put(GPIO11_Out4, 1);
					Xvtr_PTT(current_is_rx);
					break;
				case BAND_70cm:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 1);
					gpio_put(GPIO19_Out2, 1);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 1);
					Xvtr_PTT(current_is_rx);
					break;
				case BAND_33cm:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 0);
					gpio_put(GPIO19_Out2, 1);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 1);
					Xvtr_PTT(current_is_rx);
					break;
				case BAND_23cm:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO16_Out1, 1);
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 1);
					Xvtr_PTT(current_is_rx);
					break;
				default: 
					;
					// This includes band zero (reset)
				// for now do not mess with HF bands. Permits control form Quisk and Thetis.  
					//Common_IF();  //  REG_RF_INPUTS = 0 for Common IF
			#else  // For my dual band Xvtr.  Sw5 is used to provide relay power. Out1 and 2 control the relays.
				case BAND_6:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO12_Sw5,  1);  // Power for xvtr relays.  About 33ma max required, 100ma available.
					gpio_put(GPIO16_Out1, 1);  // turn on 6M relay (33ma)
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 0);
					Xvtr_PTT(current_is_rx);   // pass though PTT
					break;
				case BAND_2:
					Split_IF();  //  REG_RF_INPUTS = 1 for Split IF
					gpio_put(GPIO12_Sw5,  1);  // Power for xvtr relays.  About 33ma max required, 100ma available.
					gpio_put(GPIO16_Out1, 0);
					gpio_put(GPIO19_Out2, 1);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 0);
					Xvtr_PTT(current_is_rx);
					break;	
				default: 
					Common_IF();  //  REG_RF_INPUTS = 0 for Common  IF - for non-VHF bands
					gpio_put(GPIO12_Sw5,  0);  // Power OFF xvtr relays.  About 33ma max required, 100ma available.
					gpio_put(GPIO16_Out1, 0);
					gpio_put(GPIO19_Out2, 0);
					gpio_put(GPIO20_Out3, 0);
					gpio_put(GPIO11_Out4, 0);
					Xvtr_PTT(current_is_rx);
					break;							
			#endif
			}
		}
	}
}

