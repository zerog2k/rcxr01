// custom lcd symbols on RCXR-01

u8x8_t * u8x8_c; // low-level c access

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

#define NUM_SEGS  7
const uint8_t ch_tens_map[NUM_SEGS] = { CH_TENS_a, CH_TENS_b, CH_TENS_c, CH_TENS_d, CH_TENS_e, CH_TENS_f, CH_TENS_g };
const uint8_t ch_ones_map[NUM_SEGS] = { CH_ONES_a, CH_ONES_b, CH_ONES_c, CH_ONES_d, CH_ONES_e, CH_ONES_f, CH_ONES_g };

// digit to segment map: a,b,c,d,e,f
const uint8_t digits[][NUM_SEGS] = {
//  a, b, c, d, e, f, g
  { 1, 1, 1, 1, 1, 1, 0 }, // 0
  { 0, 1, 1, 0, 0, 0, 0 }, // 1
  { 1, 1, 0, 1, 1, 0, 1 }, // 2
  { 1, 1, 1, 1, 0, 0, 1 }, // 3
  { 0, 1, 1, 0, 0, 1, 1 }, // 4
  { 1, 0, 1, 1, 0, 1, 1 }, // 5
  { 1, 0, 1, 1, 1, 1, 1 }, // 6
  { 1, 1, 1, 0, 0, 0, 0 }, // 7
  { 1, 1, 1, 1, 1, 1, 1 }, // 8
  { 1, 1, 1, 1, 0, 1, 1 }  // 9
 };

// some other symbols
#define SYM_1       0x27 
#define SYM_23      0x2d
#define SYM_A       0x37
#define SYM_BC      0x3b

// signal bar graph
#define SIG_1       0x55
#define SIG_2       0x56
#define SIG_3       0x57
#define SIG_4       0x58
#define SIG_5       0x59
#define SIG_6       0x5f
#define MAX_SIGNAL    6
const uint8_t signal_map[MAX_SIGNAL] = { SIG_1, SIG_2, SIG_3, SIG_4, SIG_5, SIG_6 };

// battery bar graph
#define BAT_3       0x5a
#define BAT_2       0x5b
#define BAT_1       0x5c

// no (crossed circle)
#define SYM_NO      0x5d
// yes (check mark)
#define SYM_YES     0x5e

// function prototypes
void lcd_set_bat(uint8_t level);
void lcd_set_sig(uint8_t level);
void lcd_animate_sig(uint8_t level, uint8_t delay_time);
void lcd_clear_all_symbols();
void lcd_set_yes();
void lcd_set_no();
void lcd_set_chan(uint8_t chan);
void lcd_write(uint8_t addr, uint8_t data);

// function definitions
void lcd_set_bat(uint8_t level)
{
  switch (level)
  {
    case 0:
      lcd_write(BAT_1, 0);
      lcd_write(BAT_2, 0);
      lcd_write(BAT_3, 0);
      break;
    case 1:
      lcd_write(BAT_1, 1);
      lcd_write(BAT_2, 0);
      lcd_write(BAT_3, 0);
      break;
    case 2:
      lcd_write(BAT_1, 1);
      lcd_write(BAT_2, 1);
      lcd_write(BAT_3, 0);
      break;
    case 3:
    default:
      lcd_write(BAT_1, 1);
      lcd_write(BAT_2, 1);
      lcd_write(BAT_3, 1);
      break;                    
  }
}

void lcd_set_sig(uint8_t level)
{
  uint8_t i, val;

  for (i=0; i < MAX_SIGNAL; i++)
  {
    if (i < level)
      val = 1;
    else
      val = 0;
    lcd_write(signal_map[i], val);
  }
}

void lcd_animate_sig(uint8_t level, uint8_t delay_time, uint8_t repeat)
{
  uint8_t i,j;
  if (level > MAX_SIGNAL)
    level = MAX_SIGNAL;  
  for (i=0; i < repeat; i++)
  {
    for (j=0; j <= level; j++)
    {
      lcd_set_sig(j);
      delay(delay_time);
    }
    delay(delay_time);
  }
}

void lcd_clear_all_symbols()
{
  uint8_t i, c = 0;
  u8x8_cad_StartTransfer(u8x8_c);
  u8x8_cad_SendCmd(u8x8_c, 0xb0 | PAGENUM);    
  u8x8_cad_SendCmd(u8x8_c, 0x10 | 0);  
  u8x8_cad_SendCmd(u8x8_c, 0x00 | 0);
  for (i=0; i<255; i++)
  {
    u8x8_cad_SendData(u8x8_c, 1, &c);
  }
  u8x8_cad_EndTransfer(u8x8_c);      
}

void lcd_set_yes()
{
  lcd_write(SYM_YES, 1);
  lcd_write(SYM_NO, 0);
}

void lcd_set_no()
{
  lcd_write(SYM_NO, 1);
  lcd_write(SYM_YES, 0);
}

void lcd_set_chan(uint8_t chan)
{
  uint8_t i, n;
  lcd_write(SYM_CH, 1);
  // process tens
  n = (chan / 10) % 10;
  for (i=0; i < NUM_SEGS; i++)
  {
    lcd_write(ch_tens_map[i], digits[n][i]);
  }
  // process ones
  n = chan % 10;
  for (i=0; i < NUM_SEGS; i++)
  {
    lcd_write(ch_ones_map[i], digits[n][i]);
  }
}

void lcd_write(uint8_t addr, uint8_t data)
{
  u8x8_cad_StartTransfer(u8x8_c);
  u8x8_cad_SendCmd(u8x8_c, 0xb0 | PAGENUM);    
  u8x8_cad_SendCmd(u8x8_c, 0x10 | (addr >> 4));  
  u8x8_cad_SendCmd(u8x8_c, 0x00 | (addr & 0xF));
  u8x8_cad_SendData(u8x8_c, 1, &data);
  u8x8_cad_EndTransfer(u8x8_c);
}





