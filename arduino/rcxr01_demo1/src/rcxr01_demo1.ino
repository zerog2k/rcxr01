// demo for RCXR-01

#include <Arduino.h>
#include <Keypad.h>
#include <SPI.h>
#include <U8x8lib.h>
#include "lcd_symbols.h"

#include "vcc.h"
#include <printf.h>
#include "RF24.h"

/// radio stuff
#define   RF_CE   0  //PB0
#define   RF_CSN  17 //PC1

RF24 radio(RF_CE, RF_CSN);
const uint8_t txaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x00 };
const uint8_t rxaddr[] = { 0xAD, 0xAC, 0xAB, 0xAA, 0x01 };
uint8_t payload[32];

/// LCD stuff

#define CS 13 //PD5
#define RST 14 //PD6
#define RS 3 //PB3

U8X8_ST7565_RCXR01_4W_HW_SPI u8x8(/* cs=*/ CS, /* dc=*/ RS, /* reset=*/ RST);

/// keypad stuff
const uint8_t ROWS = 5; //four rows
const uint8_t COLS = 4; //four columns

uint8_t hexaKeys[ROWS][COLS] = {
  { 1,  2,  3,  4},
  { 5,  6,  7,  8},
  { 9, 10, 11, 12},
  {13, 14, 15, 16},
  {17, 18, 19, 20}
};

byte rowPins[ROWS] = {25, 27, 30, 31, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {29, 26, 11, 12}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

/// usb voltage detection stuff
#define USB_DETECT_PIN  2   // PB2
uint8_t portb_state;

void setup(){
  Serial.begin(9600);

  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"rxcr-01 demo");
  u8x8.setCursor(0,1);

  // usb detect
  pinMode(USB_DETECT_PIN, INPUT_PULLUP);

  // radio
  radio.begin();
  radio.openWritingPipe(txaddr);
  radio.openReadingPipe(1, rxaddr);
  radio.enableDynamicPayloads();
  //radio.setAutoAck(false);
  printf_begin();
  Serial.println("starting...");
  radio.printDetails();
  radio.startListening();
  
}
  
void loop(){
  
  uint8_t mykey = customKeypad.getKey();
  if (mykey){
    lcd_set_yes(u8x8.getU8x8());
    Serial.println(mykey);
    u8x8.clearLine(1);
    u8x8.setCursor(0,1);
    u8x8.print(mykey,HEX);
    payload[0] = mykey;
    radio.stopListening();
    radio.write(payload, sizeof(payload));
    
  }
  
  // check port state
  if (portb_state != PINB) 
  {
    portb_state = PINB;
    u8x8.clearLine(2);
    u8x8.setCursor(0,2);
    u8x8.print("portb: ");
    u8x8.print(portb_state, HEX);
  }
  
  u8x8.setCursor(0,3);
  u8x8.print("vcc: ");
  u8x8.print(readVcc());
  
  delay(200);
}
