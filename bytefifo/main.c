#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "bytefifo.h"

//#define _GLUE(a,b) a ## b
//#define GLUE(a,b) _GLUE(a,b)

#define BUFFER_LEN 11 // FIFO length will be 10

//BYTEFifo fifo;
//uint8_t buffer[BUFFER_LEN];

BYTEFifoDECL(fifo, BUFFER_LEN);

void print_vec(char * v, int len) {
  for (int i=0; i<len; i++)
    putchar(v[i]);
}

#if 0
int main() {
#define b2s(x) (x) ? "True " : "False"
//  byteFifoInit(&fifo, buffer, BUFFER_LEN);
  printf("<char> <enter> -> Push char\n"
         "+ <enter> -> Pop char\n"
         "* <enter> -> Peek no copy\n");
  while (1) {
    char c = (char)getchar();
    if (c == '\n')
      continue;

    if (c == '+')
      byteFifoPop(&fifo, &c, 1);
    else if (c == '*') {
      uint8_t * d1, *d2;
      uint16_t l1, l2;
      byteFifoPeekNoCopy(&fifo, &d1, &l1, &d2, &l2);
      printf("Data1 (len = %d): ", l1);
      print_vec((char *)d1, l1);
      printf("\nData2 (len = %d): ", l2);
      print_vec((char *)d2, l2);
      putchar('\n');
    }
    else
      byteFifoPush(&fifo, c);

    uint16_t count = byteFifoCount(&fifo);
    bool b = byteFifoBeginsWith(&fifo, "123", 3);
    bool t = byteFifoEndsWith(&fifo, "abcd", 4);
    printf("Count: %2u   Beg: %s   Term: %s\n", count, b2s(b), b2s(t));
  }

  return 0;
}
#else
int main() {
  printf("Push 0~9\n");
  for (int i=0; i<10; i++)
    byteFifoPush(&fifo, i);

  printf("Pop 0~9\n");
  for (int i=0; i<10; i++) {
    uint8_t c;
    byteFifoPop(&fifo, &c, 1);
    printf("-> %d\n", c);
  }

  printf("Push 0~9\n");
  for (int i=0; i<10; i++)
    byteFifoPush(&fifo, i);

  printf("Peek 0~9\n");
  for (int i=0; i<10; i++) {
    uint8_t c;
    c = byteFifoPeekNth(&fifo, i);
    printf("-> %d\n", c);
  }

  printf("Replace 5 -> 11\n");
  byteFifoReplaceNth(&fifo, 5, 11);

  printf("Peek 0~9\n");
  for (int i=0; i<10; i++) {
    uint8_t c;
    c = byteFifoPeekNth(&fifo, i);
    printf("-> %d\n", c);
  }

  printf("Replace 0~9\n");
  for (int i=0; i<10; i++)
    byteFifoReplaceNth(&fifo, i, i+20);

  printf("Pop 0~9\n");
  for (int i=0; i<10; i++) {
    uint8_t c;
    byteFifoPop(&fifo, &c, 1);
    printf("-> %d\n", c);
  }
}
#endif
