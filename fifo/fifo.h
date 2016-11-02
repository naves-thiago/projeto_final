/* Generic thread safe FIFO lib */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

/////////
typedef int semaphore_t;
typedef int systime_t;
////////


struct CBFifoItem;
typedef struct CBFifoItem CBFifoItem;

typedef void (*CBFifoCb)(CBFifoItem * item);

typedef enum {
  CB_FIFO_QUEUED,
  CB_FIFO_DELETED
} CBFifoStatus;

struct CBFifoItem {
  void * data;
  intptr_t callbackData;
  CBFifoCb callback;
  size_t dataSize;
  CBFifoStatus status;
};

typedef unsigned int CBFifoHandle;

#define CB_FIFO_HANDLE_INVALID (UINT_MAX)
#define CB_FIFO_HANDLE_MAX (UINT_MAX - 1)
//#define CB_FIFO_HANDLE_MAX 7

typedef struct {
  CBFifoItem * data;
  size_t size;
  size_t write;
  size_t read;
  CBFifoHandle firstHandle;
  CBFifoHandle lastHandle;
  semaphore_t avaliable;
  semaphore_t used;
} CBFifo;

void cbFifoInit(CBFifo * fifo, CBFifoItem * data, size_t size);
CBFifoHandle cbFifoPushWithHandle(CBFifo * fifo, CBFifoItem * data, systime_t timeout);
CBFifoHandle cbFifoPushWithHandleI(CBFifo * fifo, CBFifoItem * data);
bool cbFifoPop(CBFifo * fifo, CBFifoItem * data, systime_t timeout);
bool cbFifoPopI(CBFifo * fifo, CBFifoItem * data);
bool cbFifoDelete(CBFifo * fifo, CBFifoHandle handle);


static inline bool cbFifoPushI(CBFifo * fifo, CBFifoItem * data) {
  return cbFifoPushWithHandleI(fifo, data) != CB_FIFO_HANDLE_INVALID;
}

static inline bool cbFifoPush(CBFifo * fifo, CBFifoItem * data, systime_t timeout) {
  return cbFifoPushWithHandle(fifo, data, timeout) != CB_FIFO_HANDLE_INVALID;
}

