#include <stddef.h>
#include "uid.h"

#define BUFFER_LEN 4 // FIFO length will be 3
BYTEFifoDECL(bitmaps, BUFFER_LEN);

void uidInit(UID * uid, uint16_t len) {
  uid->bitmaps = bitmaps;
  for (int i=0; i<len; i++)
    byteFifoPush(&uid->bitmaps, 0);

  uid->idMin = 0;
  uid->idMax = 0;
}

uint32_t uidNew(UID * uid) {
  // TODO acount for all IDs being used
  uint32_t r = uid->idMax;
  uid->idMax ++;
  uid->idMax %= UID_MAX;
  return r;
}

#if 0
// Returns the bit index corresponding to id
static uint32_t uidIndex(UID * uid, uint32_t id) {
  uint32_t idMax = uid->idMax;
  uint32_t idMin = uid->idMin;

  if (uid->idMax >= uid->idMin) {
    if ((id < idMin) || (id >= idMax))
      return false;

    return id - idMin;
  }
  else {
    // idMax < idMin
    if ((id >= idMax) && (id < idMin))
      return false;

    uint32_t x;
    if (id >= idMin)
      return id - idMin;
    else
      return UID_MAX - idMin + 1 + id;
  }
}
#endif

bool uidIsValid(UID * uid, uint32_t id) {
  uint32_t idMax = uid->idMax;
  uint32_t idMin = uid->idMin;
  uint32_t x;

  if (uid->idMax >= uid->idMin) {
    if ((id < idMin) || (id >= idMax))
      return false;

    x = id - idMin;
  }
  else {
    // idMax < idMin
    if ((id >= idMax) && (id < idMin))
      return false;

    if (id >= idMin)
      x = id - idMin;
    else
      x = UID_MAX - idMin + 1 + id;
  }
  uint8_t b = byteFifoPeekNth(&uid->bitmaps, x >> 3);
  return !(b & (1 << (x & 7)));
}

void uidDel(UID * uid, uint32_t id) {
  uint32_t idMax = uid->idMax;
  uint32_t idMin = uid->idMin;
  uint32_t x;

  if (uid->idMax >= uid->idMin) {
    if ((id < idMin) || (id >= idMax))
      return;

    x = id - idMin;
  }
  else {
    // idMax < idMin
    if ((id >= idMax) && (id < idMin))
      return;

    if (id >= idMin)
      x = id - idMin;
    else
      x = UID_MAX - idMin + 1 + id;
  }

  uint8_t b = byteFifoPeekNth(&uid->bitmaps, x >> 3);
  b |= 1 << (x & 7);

  if (((x >> 3) == 0) && (b == 0xFF)) {
    byteFifoPop(&uid->bitmaps, NULL, 1);
    byteFifoPush(&uid->bitmaps, 0);
    uid->idMin += 8;
  }
  else
    byteFifoReplaceNth(&uid->bitmaps, x >> 3, b);
}

