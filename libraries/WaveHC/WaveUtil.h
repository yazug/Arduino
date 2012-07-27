#ifndef WaveUtil_h
#define WaveUtil_h
void ROM_putstring(const char *str);
void ROM_putstringnl(const char *str);
#define UINT16_MAX 65535U
#define putstring(x) ROM_putstring(PSTR(x))
#define putstring_nl(x) ROM_putstringnl(PSTR(x))
#define nop asm volatile ("nop\n\t")
#endif //WaveUtil_h
