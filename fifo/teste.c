#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "fifo.h"

CBFifo fifo;
CBFifoItem data[5];

void callback(CBFifoItem *i) {
  printf("Callback: %d\n", (int)i->data);
}

int push(char * d) {
  CBFifoItem i;
  i.data = d;
  i.dataSize = 1;
  i.callbackData = 0;
  i.callback =  callback;
  return cbFifoPushWithHandle(&fifo, &i, 0);
}

bool pop() {
  CBFifoItem i;
  if (cbFifoPop(&fifo, &i, 0)) {
    if (i.status != CB_FIFO_DELETED)
      printf("Pop: %d\n", (int)i.data);
    else // Should not happen
      printf("Pop: %d (deleted)\n", (int)i.data);

    return true;
  }
  else
    printf("Pop failed\n");

  return false;
}

int pushI(char * d) {
  CBFifoItem i;
  i.data = d;
  i.dataSize = 1;
  i.callbackData = 0;
  i.callback =  callback;
  return cbFifoPushWithHandleI(&fifo, &i);
}

bool popI() {
  CBFifoItem i;
  if (cbFifoPopI(&fifo, &i)) {
    if (i.status != CB_FIFO_DELETED)
      printf("PopI: %d\n", (int)i.data);
    else // Should not happen
      printf("PopI: %d (deleted)\n", (int)i.data);

    return true;
  }
  else
    printf("PopI failed\n");

  return false;
}


int main() {
  cbFifoInit(&fifo, data, sizeof(data));

  printf("Thread functions tests\n\n");

  for (int i=0; i<6; i++) {
    int id = push((char *)i);
    printf("Push %d - ID: %d\n", i, id);
  }

  pop();

  cbFifoDelete(&fifo, 2);
  //printf("Delete ID: 2\n");

  for (int i=0; i<6; i++)
    pop();

  for (int i=0; i<6; i++) {
    int id = push((char *)(i+10));
    printf("Push %d - ID: %d\n", i+10, id);
  }

  cbFifoDelete(&fifo, -1);
  cbFifoDelete(&fifo, 0);
  cbFifoDelete(&fifo, 1);
  cbFifoDelete(&fifo, 2);
  cbFifoDelete(&fifo, 4);
  cbFifoDelete(&fifo, 10);
  cbFifoDelete(&fifo, 7);

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


  return 0;
}
