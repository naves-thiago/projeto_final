#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "bytefifo.h"

#define UID_MAX (UINT32_MAX -1)
#define UID_INVALID (UINT32_MAX)

typedef struct {
  BYTEFifo bitmaps;
  uint32_t idMin;
  uint32_t idMax;
} UID;

void uidInit(UID * uid, uint16_t len);
uint32_t uidNew(UID * uid);
bool uidIsValid(UID * uid, uint32_t id);
void uidDel(UID * uid, uint32_t id);

