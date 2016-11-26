#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFFER_LEN 10

typedef struct {
  uint16_t write;  // Next write position
  uint16_t read;   // Oldest data position
  uint16_t length;     // FIFO length
  char buffer[BUFFER_LEN + 1];
} CHARFifo;

void charFifoInit(CHARFifo * fifo, uint16_t length) {
  fifo->write = 0;
  fifo->read = 0;
  fifo->length = length + 1;
}

uint16_t charFifoCount(CHARFifo * fifo) {
  return (fifo->length - fifo->read + fifo->write) % fifo->length;
}

void charFifoPush(CHARFifo * fifo, char c) {
  fifo->buffer[fifo->write] = c;
  fifo->write = (fifo->write + 1) % fifo->length;
  if (fifo->write == fifo->read)
    fifo->read = (fifo->read + 1) % fifo->length;
}

uint16_t charFifoPeek(CHARFifo * fifo, char * out, uint16_t count) {
  uint16_t c = charFifoCount(fifo);
  if (count > c)
    count = c;

  uint16_t r = fifo->read;
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    out[i] = fifo->buffer[(r + i) % l];

  return count;
}

uint16_t charFifoPop(CHARFifo * fifo, char * out, uint16_t count) {
  uint16_t res = charFifoPeek(fifo, out, count);
  fifo->read = (fifo->read + res) % fifo->length;
  return res;
}

bool charFifoBeginsWith(CHARFifo * fifo, char * b, uint16_t count) {
  uint16_t c = charFifoCount(fifo);
  if (count > c)
    return false;

  uint16_t r = fifo->read;
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    if (fifo->buffer[(r + i) % l] != b[i])
      return false;

  return true;
}

bool charFifoEndsWith(CHARFifo * fifo, char * b, uint16_t count) {
  uint16_t c = charFifoCount(fifo);
  if (count > c)
    return false;

  uint16_t r = fifo->read + c - count; // Read start position
  uint16_t l = fifo->length;
  for (uint16_t i=0; i<count; i++)
    if (fifo->buffer[(r + i) % l] != b[i])
      return false;

  return true;
}

CHARFifo fifo;

int main() {
#define b2s(x) (x) ? "True " : "False"
  charFifoInit(&fifo, BUFFER_LEN);

  while (1) {
    char c = (char)getchar();
    if (c == '\n')
      continue;

    if (c != '+')
      charFifoPush(&fifo, c);
    else
      charFifoPop(&fifo, &c, 1);

    uint16_t count = charFifoCount(&fifo);
    bool b = charFifoBeginsWith(&fifo, "123", 3);
    bool t = charFifoEndsWith(&fifo, "abcd", 4);
    printf("Count: %2u   Beg: %s   Term: %s\n", count, b2s(b), b2s(t));
  }

  return 0;
}
