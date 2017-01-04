#include <stdio.h>
#include "uid.h"

UID uid;

int main() {
  printf("Begin\n");
  uidInit(&uid, 3);
  printf("Test if all IDs are invalid\n");
  for (int i=-1; i<25; i++)
    if (uidIsValid(&uid, i)) {
      printf("Id %d is valid\n", i);
      return 0;
    }

  printf("OK\n");
  printf("New id\n");
  uint32_t id = uidNew(&uid);
  printf("> %u\n", id);
  if (uidIsValid(&uid, id))
    printf("Id is valid\n");
  else {
    printf("Id is not valid\n");
    return 0;
  }

  printf("Del %u\n", id);
  uidDel(&uid, id);
  if (uidIsValid(&uid, id)) {
    printf("Id is valid\n");
    return 0;
  }
  else
    printf("Id is not valid\n");

  uint32_t ids[50];
  for (int i=0; i<23; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=0; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%d not valid\n", ids[i]);
      return 0;
    }

  printf("23 valid ids\n");
  printf("Del 7 ids\n");
  for (int i=0; i<7; i++)
    uidDel(&uid, ids[i]);

  for (int i=0; i<7; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%d is valid\n", ids[i]);
      return 0;
    }

  for (int i=8; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%d is not valid\n", ids[i]);
      return 0;
    }

  printf("Del %d\n", ids[7]);
  uidDel(&uid, ids[7]);
  if (uidIsValid(&uid, ids[7])) {
    printf("%d is valid\n", ids[7]);
    return 0;
  }

  for (int i=8; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%d is not valid\n", ids[i]);
      return 0;
    }

  printf("Del %d\n", ids[9]);
  uidDel(&uid, ids[9]);
  if (uidIsValid(&uid, ids[9])) {
    printf("%d is valid\n", ids[9]);
    return 0;
  }

  for (int i=10; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%d is not valid\n", ids[i]);
      return 0;
    }


  for (int i=23; i<23+8; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=10; i<31; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%d is not valid\n", ids[i]);
      return 0;
    }


  printf("=== OK ===\n");
  return 0;
}

