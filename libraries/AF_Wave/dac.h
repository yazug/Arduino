

#define DAC_CS_PORT PORTD
#define DAC_CS_DDR DDRD
#define DAC_CS PIND2

#define DAC_CLK_PORT PORTD
#define DAC_CLK_DDR DDRD
#define DAC_CLK PIND3

#define DAC_DI_PORT PORTD
#define DAC_DI_DDR DDRD
#define DAC_DI PIND4

#define DAC_LATCH_PORT PORTD
#define DAC_LATCH_DDR DDRD
#define DAC_LATCH PIND5

#define DAC_FLAG_B 0x8000
#define DAC_FLAG_A 0
#define DAC_FLAG_BUFFERED 0x4000
#define DAC_FLAG_UNBUFFERED 0
#define DAC_FLAG_GAIN1X 0x2000
#define DAC_FLAG_GAIN2X 0
#define DAC_FLAG_ENABLED 0x1000
#define DAC_FLAG_SHUTDOWN 0

#define select_dac() DAC_CS_PORT &= ~_BV(DAC_CS)
#define unselect_dac() DAC_CS_PORT |= _BV(DAC_CS)
#define dac_clock_up() DAC_CLK_PORT |= _BV(DAC_CLK)
#define dac_clock_down() DAC_CLK_PORT &= ~_BV(DAC_CLK)
#define dac_latch_up() DAC_LATCH_PORT |= _BV(DAC_LATCH)
#define dac_latch_down() DAC_LATCH_PORT &= ~_BV(DAC_LATCH)
#define dac_data_high() DAC_DI_PORT |= _BV(DAC_DI)
#define dac_data_low() DAC_DI_PORT &= ~_BV(DAC_DI)

void dac_init(void);
void dac_send_val(uint16_t v);
