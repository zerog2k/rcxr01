// custom lcd symbols on RCXR-01

// custom symbols on page 8
#define PAGENUM     0x08

// symbol addresses
// channel number ("ch#")
#define SYM_CH      0x04
// channel number lcd segments
#define CH_TENS_a   0x05
#define CH_TENS_f   0x06
#define CH_TENS_g   0x07
#define CH_TENS_e   0x08
#define CH_TENS_d   0x09
#define CH_TENS_c   0x0a
#define CH_TENS_b   0x0b

#define CH_ONES_f   0x0c
#define CH_ONES_e   0x0d
#define CH_ONES_d   0x0e
#define CH_ONES_c   0x0f
#define CH_ONES_g   0x10
#define CH_ONES_b   0x11
#define CH_ONES_a   0x12

// some other symbols
#define SYM_1       0x27 
#define SYM_23      0x2d
#define SYM_A       0x37
#define SYM_BC      0x3b

// signal bar graph
#define SIG_0       0x55
#define SIG_1       0x56
#define SIG_2       0x57
#define SIG_3       0x58
#define SIG_4       0x59
#define SIG_5       0x5f

// battery bar graph
#define BAT_0       0x5a
#define BAT_1       0x5b
#define BAT_2       0x5c

// no (crossed circle)
#define SYM_NO      0x5d
// yes (check mark)
#define SYM_YES     0x5e

void lcd_write(u8x8_t * u8x8, uint8_t addr, uint8_t data)
{
  //uint8_t buf[1];
  //buf[0] = data;
  u8x8_cad_StartTransfer(u8x8);
  u8x8_cad_SendCmd(u8x8, 0x10 | (addr >> 4));  
  u8x8_cad_SendCmd(u8x8, 0x00 | (addr & 0xF));
  u8x8_cad_SendCmd(u8x8, 0xb0 | PAGENUM);  
  u8x8_cad_SendData(u8x8, 1, &data); //not working fix ptr
  u8x8_cad_EndTransfer(u8x8);
}
void lcd_set_yes(u8x8_t * u8x8)
{
  lcd_write(u8x8, SYM_YES, 1);
}

void lcd_set_chan(uint8_t chan);




