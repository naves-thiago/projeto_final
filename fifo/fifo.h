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


struct fifo_item_t;
typedef struct fifo_item_t fifo_item_t;

typedef void (*fifo_cb_t)(fifo_item_t * item);

typedef enum {
  CB_FIFO_READ,
  CB_FIFO_QUEUED,
  CB_FIFO_DELETED
} fifo_status_t;

struct fifo_item_t {
  void * data;
  intptr_t callback_data;
  fifo_cb_t callback;
  size_t data_size;
  fifo_status_t status;
};

typedef unsigned int fifo_handle_t;

typedef struct {
  fifo_item_t * data;
  size_t size;
  size_t write;
  size_t read;
  fifo_handle_t first_handle;
  fifo_handle_t last_handle;
  semaphore_t avaliable;
  semaphore_t used;
} fifo_t;

#define HANDLE_INVALID (UINT_MAX)
#define HANDLE_MAX (UINT_MAX - 1)
//#define HANDLE_MAX 7

void fifoInit(fifo_t * fifo, fifo_item_t * data, size_t size);
fifo_handle_t fifoPushWithHandle(fifo_t * fifo, fifo_item_t * data, systime_t timeout);
fifo_handle_t fifoPushWithHandleI(fifo_t * fifo, fifo_item_t * data);
bool fifoPop(fifo_t * fifo, fifo_item_t * data, systime_t timeout);
bool fifoPopI(fifo_t * fifo, fifo_item_t * data);
bool fifoDelete(fifo_t * fifo, fifo_handle_t handle);


static inline bool fifoPushI(fifo_t * fifo, fifo_item_t * data) {
  return fifoPushWithHandleI(fifo, data) != HANDLE_INVALID;
}

static inline bool fifoPush(fifo_t * fifo, fifo_item_t * data, systime_t timeout) {
  return fifoPushWithHandle(fifo, data, timeout) != HANDLE_INVALID;
}

