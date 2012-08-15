#include <ChibiOS.h>
/**
 * determine unused stack bytes for a thread
 *
 * param[in] wsp pointer to working space for thread
 * param[in] size working space size
 *
 * \return number of unused stack locations
 */
size_t chUnusedStack(void *wsp, size_t size) {
  size_t n = 0;
#if CH_DBG_FILL_THREADS
  uint8_t *startp = (uint8_t *)wsp + sizeof(Thread);
  uint8_t *endp = (uint8_t *)wsp + size;
  while (startp < endp)
    if(*startp++ == CH_STACK_FILL_VALUE) ++n;
#endif
  return n;
}