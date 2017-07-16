// demo for RCXR-01

#include <Arduino.h>

#include "lcd.h"
#include "vcc.h"
#include <printf.h>
#include "RF24.h"
#include "sleep.h"
#include "rtc.h"
#include "keypad.h"
#include "serno.h"

/// radio stuff
#define   RF_CE   0  //PB0
#define   RF_CSN  17 //PC1

RF24 radio(RF_CE, RF_CSN);
const uint8_t txaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x00 };
const uint8_t rxaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x01 };
uint8_t payload[32];
#define MSG_TYPE_KEYPRESS 0xA1

uint32_t last_keypress_time = 0;
enum {
  M_SLEEP,
  M_WAKE
} display_mode;

uint8_t mode = M_WAKE;
#define SLEEP_DELAY_TIME 10

/// usb voltage detection stuff
#define USB_DETECT_PIN  2   // PB2
uint8_t on_battery;
#define BATT_LEVEL_HIGH 2500
#define BATT_LEVEL_MED  2200
#define BATT_LEVEL_LOW  1900

uint8_t i;

void setup()
{
  // change clock from 8 to 1MHz to ensure safe operating range down to
  // brown-out battery voltage of 1.8V
  CLKPR = _BV(CLKPCE); // Clock Prescaler Change Enable
  CLKPR = _BV(CLKPS1) | _BV(CLKPS0); // Clock Prescaler = fosc/8

  delay(100);

  Serial.begin(9600);
  u8x8.begin();
  u8x8_c = u8x8.getU8x8(); // low-level c access for custom lcd funcs
  lcd_clear_all_symbols();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_victoriamedium8_r);
  u8x8.drawString(0,0,"rxcr-01 demo");
  u8x8.setCursor(0,1);

  // usb detect
  pinMode(USB_DETECT_PIN, INPUT_PULLUP);

  // radio
  radio.begin();
  radio.openWritingPipe(txaddr);
  radio.openReadingPipe(1, rxaddr);
  //radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  printf_begin();
  Serial.println("starting...");
  radio.printDetails();
  //radio.startListening();  // dont use this unless we need to - consumes about 13mA
  lcd_set_chan(radio.getChannel());
  // set up wdt as wake-up timer
  setupWatchDogTimer();
  Serial.print("rtc_init start.");
  rtc_init();
  Serial.println("rtc_init done.");
  print_ser_num();
  print_lot_code();  
  sei();
}
  
void loop()
{
  on_battery = digitalRead(USB_DETECT_PIN);
  if ( mode != M_SLEEP && on_battery && 
      ( seconds - last_keypress_time ) > SLEEP_DELAY_TIME)
  {
    mode = M_SLEEP;
    customKeypad.setDebounceTime(1);
    u8x8.setPowerSave(1);
    radio.powerDown();
  }
  uint8_t mykey;
  if (mykey = customKeypad.getKey()) {
    last_keypress_time = seconds;
    if (mode == M_SLEEP) {
      u8x8.setPowerSave(0);
      radio.powerUp();
      customKeypad.setDebounceTime(10);
      mode = M_WAKE;
      return;
    }
    // alternate yes/no between presses
    if (i%2==0) {
          lcd_set_yes();
    } else {
          lcd_set_no();
    }
    Serial.println(mykey);
    //u8x8.clearLine(1);
    u8x8.setCursor(0,1);
    u8x8.print("k:   ");
    u8x8.setCursor(2,1);
    u8x8.print(mykey);
    payload[0] = MSG_TYPE_KEYPRESS;
    payload[1] = mykey;
    radio.stopListening();
    radio.write(payload, 2);
    lcd_animate_sig(6);
    i++;
    return;
  }

  if (mode == M_WAKE)
  {
    u8x8.setCursor(5,1);
    u8x8.print("t:");
    u8x8.print(seconds);

    u8x8.setCursor(0,2);
    u8x8.print("on_bat:");
    u8x8.print(on_battery);

    uint16_t batt_mv = readVcc();
    u8x8.setCursor(0,3);
    u8x8.print("v:");
    u8x8.print(batt_mv);
    lcd_set_bat_shell(BAT_SHELL_ON); 
    if (batt_mv > BATT_LEVEL_HIGH)
      lcd_set_bat(3);
    else if (batt_mv > BATT_LEVEL_MED)
      lcd_set_bat(2);
    else if (batt_mv > BATT_LEVEL_LOW)
      lcd_set_bat(1);
    else
    {
      lcd_set_bat(0);
      lcd_set_bat_shell(BAT_SHELL_BLINK_SLOW);
    }
  }

  enterSleep(); // wake on wdt and rtc interrupts
}
