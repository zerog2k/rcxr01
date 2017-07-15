// read 9 byte unique serial number on atmega
#include <avr/pgmspace.h>

#define SERNUM_LEN  9

uint8_t myserial[SERNUM_LEN];

uint8_t read_sig_row_byte(uint16_t addr) {
  uint8_t result;
  SPMCSR |= _BV(SIGRD) | _BV(SPMEN);
  result = pgm_read_byte(addr);
  return result;
}

enum {
  SERNUM1 = 0x0E, 
  SERNUM0,
  SERNUM3,
  SERNUM2,
  SERNUM5,
  SERNUM4,
  SERNUM6 = 0x15,
  SERNUM7,
  SERNUM8
};

const uint8_t ser_num_addrs[SERNUM_LEN] = { SERNUM0, SERNUM1, SERNUM2, SERNUM3, SERNUM4, SERNUM5, SERNUM6, SERNUM7, SERNUM8 };

void get_ser_num(uint8_t buf[SERNUM_LEN])
{
    uint8_t i; 
    for (i=0; i < SERNUM_LEN; i++)
    {
        buf[i] = read_sig_row_byte(ser_num_addrs[i]);
    }   
}

void print_ser_num()
{
    uint8_t i;
    get_ser_num(myserial);
    printf("myserial: ");
    for (i=0; i<SERNUM_LEN; i++)
    {
        printf("%02X", myserial[i]);
    }
    printf("\n");
}

void print_lot_code()
{
    uint8_t i;
    get_ser_num(myserial);
    printf("lot code: ");
    for (i=0; i<6; i++)
    {
        printf("%c", myserial[i]);
    }
    printf("\n");
}