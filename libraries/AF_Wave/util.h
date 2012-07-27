void ROM_putstring(const char *str, uint8_t nl);

#define UINT16_MAX 65535U
#define putstring(x) ROM_putstring(PSTR(x), 0)
#define putstring_nl(x) ROM_putstring(PSTR(x), 1)
#define nop asm volatile ("nop\n\t")
