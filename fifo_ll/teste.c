#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "llfifo.h"

typedef struct {
  LLFifoItem _i;
  int data;
  int id;
} item_t;

LLFifo fifo;
item_t * items[50];
int item_count = 0;

int push(int d) {
  item_t * i = malloc(sizeof(item_t));
  i->data = d;
  i->id = item_count;
  llFifoPush(&fifo, (LLFifoItem *)i);
  items[item_count] = i;
  item_count ++;
  return item_count -1;
}

bool pop() {
  item_t * i = llFifoPop(&fifo, 0);

  if (i) {
    if (i->_i.status != CB_FIFO_DELETED)
      printf("Pop: %d\n", i->data);
    else // Should not happen
      printf("Pop: %d (deleted) - %p - %d\n", i->data, i, i->id);

    return true;
  }
  else
    printf("Pop failed\n");

  return false;
}

int pushI(int d) {
  item_t * i = malloc(sizeof(item_t));
  i->data = d;
  i->id = item_count;
  llFifoPushI(&fifo, (LLFifoItem *)i);
  items[item_count] = i;
  item_count ++;
  return item_count -1;
}

bool popI() {
  item_t * i = llFifoPopI(&fifo);

  if (i) {
    if (i->_i.status != CB_FIFO_DELETED)
      printf("Pop: %d\n", i->data);
    else // Should not happen
      printf("Pop: %d (deleted) - %p - %d\n", i->data, i, i->id);

    return true;
  }
  else
    printf("Pop failed\n");

  return false;
}

int main() {
  llFifoInit(&fifo);

  printf("Thread functions tests\n\n");

  for (int i=0; i<6; i++) {
    int id = push(i);
    printf("Push %d - ID: %d\n", i, id);
  }

  pop();

  llFifoDelete(&fifo, items[2]);
  printf("Delete ID: 2\n");

  for (int i=0; i<6; i++)
    pop();

  for (int i=0; i<6; i++) {
    int id = push(i+10);
    printf("Push %d - ID: %d\n", i+10, id);
  }

  llFifoDelete(&fifo, items[6]);
  printf("Delete ID: 6\n");

  for (int i=0; i<5; i++)
    pop();

  for (int i=0; i<5; i++) {
    int id = push(i+20);
    printf("Push %d - ID: %d\n", i+20, id);
  }

  llFifoDelete(&fifo, items[16]);
  printf("Delete ID: 16\n");

  for (int i=0; i<5; i++)
    pop();

  printf("\n-----------------------------\n");
  printf("IRQ functions tests\n\n");

  llFifoInit(&fifo);
  for (int i=0; i<item_count; i++) {
    free(items[i]);
    items[i] = NULL;
  }

  item_count = 0;


  for (int i=0; i<6; i++) {
    int id = pushI(i);
    printf("Push %d - ID: %d\n", i, id);
  }

  popI();

  llFifoDelete(&fifo, items[2]);
  printf("Delete ID: 2\n");

  for (int i=0; i<6; i++)
    popI();

  for (int i=0; i<6; i++) {
    int id = pushI(i+10);
    printf("Push %d - ID: %d\n", i+10, id);
  }

  llFifoDelete(&fifo, items[6]);
  printf("Delete ID: 6\n");

  for (int i=0; i<5; i++)
    popI();

  for (int i=0; i<5; i++) {
    int id = pushI(i+20);
    printf("Push %d - ID: %d\n", i+20, id);
  }

  llFifoDelete(&fifo, items[16]);
  printf("Delete ID: 16\n");

  for (int i=0; i<5; i++)
    popI();


  for (int i=0; i<item_count; i++)
    free(items[i]);
#if 0
  cbFifoDelete(&fifo, items[0]);
  cbFifoDelete(&fifo, items[1]);
  cbFifoDelete(&fifo, items[2]);
  cbFifoDelete(&fifo, items[4]);
  cbFifoDelete(&fifo, items[10]);
  cbFifoDelete(&fifo, items[7]);

  printf("\n-----------------------------\n");
  printf("IRQ functions tests\n\n");

  cbFifoInit(&fifo, data, sizeof(data));

  for (int i=0; i<6; i++) {
    int id = pushI((char *)i);
    printf("PushI %d - ID: %d\n", i, id);
  }

  popI();

  cbFifoDelete(&fifo, 2);
  //printf("Delete ID: 2\n");

  for (int i=0; i<5; i++)
    popI();

  popI();

  for (int i=0; i<6; i++) {
    int id = pushI((char *)(i+10));
    printf("PushI %d - ID: %d\n", i+10, id);
  }

  cbFifoDelete(&fifo, -1);
  cbFifoDelete(&fifo, 0);
  cbFifoDelete(&fifo, 1);
  cbFifoDelete(&fifo, 2);
  cbFifoDelete(&fifo, 4);
  cbFifoDelete(&fifo, 10);
  cbFifoDelete(&fifo, 7);
  cbFifoDelete(&fifo, 5);

#endif
  return 0;
}
