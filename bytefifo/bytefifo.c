#include <stddef.h>
#include "bytefifo.h"

void byteFifoInit(BYTEFifo * fifo, uint8_t * buffer, uint16_t length) {
  fifo->write = 0;
  fifo->read = 0;
  fifo->length = length;
  fifo->buffer = buffer;
}

void byteFifoClear(BYTEFifo * fifo) {
  fifo->write = 0;
  fifo->read = 0;
}

uint16_t byteFifoCount(BYTEFifo * fifo) {
  return (fifo->length - fifo->read + fifo->write) % fifo->length;
}

void byteFifoPush(BYTEFifo * fifo, uint8_t c) {
  fifo->buffer[fifo->write] = c;
  fifo->write = (fifo->write + 1) % fifo->length;
  if (fifo->write == fifo->read)
    fifo->read = (fifo->read + 1) % fifo->length;
}

uint16_t byteFifoPeek(BYTEFifo * fifo, uint8_t * out, uint16_t count) {
  uint16_t c = byteFifoCount(fifo);
  if (count > c)
    count = c;

  uint16_t r = fifo->read;
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    out[i] = fifo->buffer[(r + i) % l];

  return count;
}

void byteFifoPeekNoCopy(BYTEFifo * fifo, uint8_t ** out1, uint16_t * len1, uint8_t ** out2, uint16_t * len2) {
  *out1 = &fifo->buffer[fifo->read];
  if (fifo->read <= fifo->write) {
    *len1 = fifo->write - fifo->read;
    *out2 = NULL;
    *len2 = 0;
  }
  else {
    *len1 = fifo->length - fifo->read;
    *out2 = fifo->buffer;
    *len2 = fifo->write;
  }
}

uint16_t byteFifoPop(BYTEFifo * fifo, uint8_t * out, uint16_t count) {
  uint16_t res;
  if (out) {
    res = byteFifoPeek(fifo, out, count);
  }
  else {
    res = byteFifoCount(fifo);
    if (res > count)
      res = count;
  }

  fifo->read = (fifo->read + res) % fifo->length;
  return res;
}

bool byteFifoBeginsWith(BYTEFifo * fifo, uint8_t * b, uint16_t count) {
  uint16_t c = byteFifoCount(fifo);
  if (count > c)
    return false;

  uint16_t r = fifo->read;
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    if (fifo->buffer[(r + i) % l] != b[i])
      return false;

  return true;
}

bool byteFifoEndsWith(BYTEFifo * fifo, uint8_t * b, uint16_t count) {
  uint16_t c = byteFifoCount(fifo);
  if (count > c)
    return false;

  uint16_t r = fifo->read + c - count; // Read start position
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    if (fifo->buffer[(r + i) % l] != b[i])
      return false;

  return true;
}

uint8_t byteFifoPeekNth(BYTEFifo * fifo, uint16_t n) {
  return fifo->buffer[(fifo->read + n) % fifo->length];
}

void byteFifoReplaceNth(BYTEFifo * fifo, uint16_t n, uint8_t b) {
  fifo->buffer[(fifo->read + n) % fifo->length] = b;
}
