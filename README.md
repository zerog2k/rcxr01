# rcxr01
Various documentation and example code for Turning Technologies ResponseCard XR/XR2 model RCXR-01

inspired by [Travis Goodspeed's most excellent reverse engineering](http://travisgoodspeed.blogspot.com/2010/07/reversing-rf-clicker.html) of older versions of these clickers.
The RCXR-01 seems to be quite handy as a portable ATmega + nRF24 platform (i.e. handheld control/monitoring). I hope that you will find lots of nice hacks

This is not about reverse engineering the existing software, but rather reusing the hardware completely. You get a nice lcd, matrix keypad, NRF24 radio, usb-uart, and mcu with plenty of flash that can run Arduino (or pure AVR C if that's your thing ;).

(Note that the NXT [RCXR-02/03] is completely different insides - based on MSP430F5528)

<img src="docs/rcxr_01_demo.jpg" width="400">

## hardware
* ATmega 644PV mcu
  * 64k flash, 4k ram, 2k eeprom
  * internal RC 1MHz clock (CKDIV8 fuse set)
  * 32khz crystal on TOSC1/2 
    * timer2 can be setup as rtc
    * ~~may be missing some loading caps? In some experiments, I'm having a tough time getting the crystal to start. I used 6pF caps, and I think that helped. But also doing some experiments where just grounding the crystal can it seems to help it start up (without needing extra load caps).~~
* NRF24L01 radio
  * on hardware spi bus (SCK/MISO/MOSI)
  * CE -> PB0, CSN -> PC1
* CAT25128 16KB SPI EEPROM
  * CS -> SS (PB4)
  * doesn't look like TT used the external eeprom for anything.
* FTDI FT232R usb to uart
  * TXD/RXD seem to be connected through some isolator? to mcu
  * has custom PID: 0xdb14
    * flashing to default ftdi PID: `sudo ./ft232r_prog --old-pid 0xdb14 --new-pid 0x6001` so serial port is setup on connect
    * https://github.com/eswierk/ft232r_prog
  * DTR/RTS not connected. May want to connect either these to mcu RST (top pad of D5) w/ in-line 1uF cap for auto-reset, i.e. for arduino bootloader (I found 4.7uF cap works better for me.)
* when usb power is present, PB2 is pulled low (via Q3)
* testpoint pads under battery compartment (from left to right)
  * MOSI, RST, SCK, MISO, GND, VDD3v3, SS, TCK, TMS, TDO, TDI
* LCD panel
  * 96x32 matrix plus several custom symbols, possibly [ST7565 chipset](https://edeca.net/pages/the-st7565-display-controller/).  
  * U8x8 lib customized: U8X8_ST7565_RCXR01_4W_HW_SPI
  * mcu connectivity: SPI for SCLK & SI. CS -> PD5, RST -> PD6, RS(A0) -> PB3

### keypad matrix map
|       |  PA5 |  PA2 |  PD3 |  PD4 |
| ----- | ---- | ---- | ---- | ---- |
| *PA1* |  SW1 |  SW2 |  SW3 |  SW4 |
| *PA3* |  SW5 |  SW6 |  SW7 |  SW8 |
| *PA6* |  SW9 | SW10 | SW11 | SW12 |
| *PA7* | SW13 | SW14 | SW15 | SW16 |
| *PA4* | SW17 | SW18 | SW19 | SW20 |

### intial fuses
 * (E:FE, H:9D, L:62)
 * http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega644p&LOW=62&HIGH=9D&EXTENDED=FE&LOCKBIT=FF

### flashing arduino bootloader
 * use something like usbasp, ftdi, etc. Cheap usbasp adapter worked fine for me. Something similar to [this adapter from Banggood](https://www.banggood.com/USBASP-USBISP-3_3-5V-AVR-Downloader-Programmer-With-ATMEGA8-ATMEGA128-p-934425.html?p=WX0407753399201409DA)
 * https://github.com/MCUdude/MightyCore/tree/master/avr/bootloaders/optiboot_flash/atmega644p
 * choose a bootloader option:
#### bootloader @ 1MHz
  * slower option (recommend instead 8MHz@38400)
  * use `optiboot_flash_atmega644p_9600_1000000L.hex` 
  * set fuses: `avrdude -v -p atmega644p -c usbasp -P usb -e -U lock:w:0x3f:m -U efuse:w:0xfe:m -U hfuse:w:0xd6:m -U lfuse:w:0x62:m`
  * flash bootloader: `avrdude -v -p atmega644p -c usbasp -P usb -U flash:w:optiboot_flash_atmega644p_9600_1000000L.hex:i -U lock:w:0x0f:m`
  * now should be able to use arduino bootloader at 9600 for uploading (see note above about connecting RTS/DTR to RST for auto-reset.)
#### bootloader @ 8MHz
  * use `optiboot_flash_atmega644p_38400_8000000L.hex`
  * set fuses: `avrdude -v -p atmega644p -c usbasp -P usb -e -U lock:w:0x3f:m -U efuse:w:0xfe:m -U hfuse:w:0xd6:m -U lfuse:w:0xe2:m` (unsets CKDIV8)
  * flash bootloader: `avrdude -v -p atmega644p -c usbasp -P usb -U flash:w:optiboot_flash_atmega644p_38400_8000000L.hex:i -U lock:w:0x0f:m`
  * should allow you to use 38400 upload speed. Note: adjust `upload_speed` in `platformio.ini`
  * bootloader will run at 8MHz. This should be safe because with usb, vcc should always be above 2.7V. Application sketch should run at <=4MHz in order to run off of battery down to BOD voltage of 1.8V. Demo app here sets clock prescaler down to 1MHz.
   
## building
 * using MightyCore board support for Arduino framework
 * using platformio for build/upload
 * pinouts and pin number mappings: https://github.com/MCUdude/MightyCore#pinout

## images
### pcb front
<img src="docs/rcxr_01_front_pcb.jpg" width="400">

### pcb rear
<img src="docs/rcxr_01_rear_pcb.jpg" width="400">
