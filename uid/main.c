#include <stdio.h>
#include "uid.h"

#define ERROR() printf("Line: %d\n", __LINE__); return 0
UID uid;

int main() {
  uint32_t ids[100];

  printf("Begin\n");
  uidInit(&uid, 3);
  printf("Test if all IDs are invalid\n");
  for (int i=-1; i<25; i++)
    if (uidIsValid(&uid, i)) {
      printf("Id %u is valid\n", i);
      ERROR();
    }

  printf("OK\n");
  printf("New id\n");
  uint32_t id = uidNew(&uid);
  printf("> %u\n", id);
  if (uidIsValid(&uid, id))
    printf("Id is valid\n");
  else {
    printf("Id is not valid\n");
    ERROR();
  }

  printf("Del %u\n", id);
  uidDel(&uid, id);
  if (uidIsValid(&uid, id)) {
    printf("Id is valid\n");
    ERROR();
  }
  else
    printf("Id is not valid\n");

  for (int i=0; i<23; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=0; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u not valid\n", ids[i]);
      ERROR();
    }

  printf("23 valid ids\n");
  printf("Del 7 ids\n");
  for (int i=0; i<7; i++)
    uidDel(&uid, ids[i]);

  for (int i=0; i<7; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  for (int i=8; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  printf("Del %d\n", ids[7]);
  uidDel(&uid, ids[7]);
  if (uidIsValid(&uid, ids[7])) {
    printf("%u is valid\n", ids[7]);
    ERROR();
  }

  for (int i=8; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  printf("Del %d\n", ids[8]);
  uidDel(&uid, ids[8]);
  if (uidIsValid(&uid, ids[8])) {
    printf("%u is valid\n", ids[8]);
    ERROR();
  }

  for (int i=9; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  printf("Del %d\n", ids[9]);
  uidDel(&uid, ids[9]);
  if (uidIsValid(&uid, ids[9])) {
    printf("%u is valid\n", ids[9]);
    ERROR();
  }

  for (int i=10; i<23; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  // 8 new ids
  for (int i=23; i<31; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=10; i<31; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  for (int i=0; i<10; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  printf("Del all but the last 8 ids\n");
  for (int i=10; i<=23; i++)
    uidDel(&uid, ids[i]);

  for (int i=10; i<=23; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  for (int i=24; i<31; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  for (int i=24; i<28; i++) {
    printf("Del %u\n", ids[i]);
    uidDel(&uid, ids[i]);
  }

  for (int i=0; i<24; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  for (int i=28; i<31; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  for (int i=31; i<36; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=28; i<36; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  for (int i=0; i<28; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  printf("Del all ids\n");
  for (int i=28; i<36; i++) {
    printf("Del %u\n", ids[i]);
    uidDel(&uid, ids[i]);
  }

  for (int i=0; i<36; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  printf("16 new ids\n");
  for (int i=36; i<52; i++) {
    uint32_t tmp = uidNew(&uid);
    printf("New: %u\n", tmp);
    ids[i] = tmp;
  }

  for (int i=0; i<36; i++)
    if (uidIsValid(&uid, ids[i])) {
      printf("%u is valid\n", ids[i]);
      ERROR();
    }

  for (int i=36; i<52; i++)
    if (!uidIsValid(&uid, ids[i])) {
      printf("%u is not valid\n", ids[i]);
      ERROR();
    }

  

  /*
  int new_index = 52;
  int del_index = 
  for (int k=0; k<5; k++) {
    for (int i=0; i<7; i++)
  }
  */

  printf("=== OK ===\n");
  return 0;
}

