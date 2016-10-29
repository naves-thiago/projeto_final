#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "fifo.h"

fifo_t fifo;
fifo_item_t data[5];

void callback(fifo_item_t *i) {
  printf("Callback: %d\n", (int)i->data);
}

int push(char * d) {
  fifo_item_t i;
  i.data = d;
  i.data_size = 1;
  i.callback_data = 0;
  i.callback =  callback;
  return fifoPushWithHandle(&fifo, &i, 0);
}

bool pop() {
  fifo_item_t i;
  if (fifoPop(&fifo, &i, 0)) {
    if (i.status != CB_FIFO_DELETED)
      printf("Pop: %d\n", (int)i.data);
    else
      printf("Pop: %d (deleted)\n", (int)i.data);

    return true;
  }

  return false;
}

int main() {
  fifoInit(&fifo, data, sizeof(data));

  for (int i=0; i<6; i++) {
    int id = push((char *)i);
    printf("Push %d - ID: %d\n", i, id);
  }

  pop();

  fifoDelete(&fifo, 2);
  //printf("Delete ID: 2\n");

  while(pop());

  for (int i=0; i<6; i++) {
    int id = push((char *)(i+10));
    printf("Push %d - ID: %d\n", i+10, id);
  }

  fifoDelete(&fifo, -1);
  fifoDelete(&fifo, 1);
  fifoDelete(&fifo, 4);
  fifoDelete(&fifo, 10);
  fifoDelete(&fifo, 7);

  return 0;
}
