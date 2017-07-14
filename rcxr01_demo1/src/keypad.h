#include <Keypad.h>

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

uint8_t rowPins[ROWS] = {25, 27, 30, 31, 28}; //connect to the row pinouts of the keypad
uint8_t colPins[COLS] = {29, 26, 11, 12}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// 
#define VARIANT_XR
// #define VARIANT_XR2

#ifdef VARIANT_XR

#define K_YES       02
#define K_PREV      K_YES
#define K_MENU      03
#define K_NO        04
#define K_NEXT      K_NO
#define K_SEL       08
#define K_Q         09
#define K_QUESTION  K_Q
#define K_PEN       13

#elif   VARIANT_XR2

#define K_PREV      02
#define K_SEL       03
#define K_NEXT      04
#define K_CHAN      13
#define K_abc       K_SEL

#endif

#define K_ENTER     01
#define K_1A        05
#define K_2B        06
#define K_3C        07

#define K_4D        10
#define K_5E        11
#define K_CLR       12
#define K_DEL       K_CLR
#define K_8H        14
#define K_9I        15
#define K_6F        16
#define K_7G        17
#define K_PLUS      18
#define K_PLUSMINUS K_PLUS
#define K_0J        19
#define K_DOT       20

