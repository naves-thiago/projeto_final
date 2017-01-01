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

  printf("=== OK ===\n");
  return 0;
}

