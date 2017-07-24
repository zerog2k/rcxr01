// demo for RCXR-01

#include <Arduino.h>

#include "lcd.h"
#include "vcc.h"
#include <printf.h>
#include "RF24.h"
#include "sleep.h"
#include "rtc.h"
#include "serno.h"

/// radio stuff
#define   RF_CE   0  //PB0
#define   RF_CSN  17 //PC1

RF24 radio(RF_CE, RF_CSN);
const uint8_t txaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x00 };
const uint8_t rxaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x01 };
uint8_t payload[32];
#define MSG_TYPE_KEYPRESS 0xA1

// display/sleep mode stuff
uint32_t sleep_timer = 0;
typedef enum {
  M_SLEEP,
  M_WAKEUP,
  M_ACTIVE
} display_modes;

volatile uint8_t mode = M_ACTIVE;
#define SLEEP_DELAY_TIME 8
#include "keypad.h"

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
  clock_prescale_set(clock_div_8);
  
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
  keypad_enter_sleep();
  sei();
}
  
void loop()
{
  if (mode == M_WAKEUP)
  {
    // transition from WAKE to ACTIVE
    keypad_exit_sleep();
    sleep_timer = seconds;
    u8x8.setPowerSave(0);
    radio.powerUp();
    mode = M_ACTIVE;
    delay(10);
    return;
  }

  on_battery = digitalRead(USB_DETECT_PIN);
  if (! on_battery)
  {
    // on usb power, reset sleep timer or wake
    sleep_timer = seconds;
    if (mode == M_SLEEP)
      mode = M_WAKEUP;
  }

  if ( mode == M_ACTIVE && 
      ( seconds - sleep_timer ) > SLEEP_DELAY_TIME)
  {
    // transition from ACTIVE to SLEEP
    mode = M_SLEEP;
    u8x8.setPowerSave(1);
    radio.powerDown();
    keypad_enter_sleep();

    //testing
    /*
    pinMode(RF_CSN, INPUT);
    pinMode(CS, INPUT);
    pinMode(RST, INPUT);
    pinMode(RS, INPUT);
    */
  }

  if (mode == M_ACTIVE)
  {
    uint8_t mykey = customKeypad.getKey();
    if (mykey) {
      sleep_timer = seconds;
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
    }
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

  do_sleep(); // wake on wdt and rtc interrupts
}
