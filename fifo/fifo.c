/* Pointer FIFO lib */
#include "fifo.h"
#include <string.h>
#include <string.h>

////////////
#include <stdio.h>
#define chMtxObjectInit(...)
#define chMtxLock(...)
#define chMtxUnlock(...)
#define MSG_OK 1
#define chSysUnlockFromISR()
#define chSysLockFromISR()
#define chSysLock()
#define chSysUnlock()
#define chDbgCheck(...)
#define chDbgCheckClassI()

void chSemObjectInit(semaphore_t *s, int val) {
  *s = val;
}

void chSemSignal(semaphore_t *s) {
  (*s) ++;
}

int chSemWaitTimeout(semaphore_t *s, systime_t t) {
  (void) t;
  if (*s) {
    (*s) --;
    return MSG_OK;
  }

  return 0;
}

void chSemFastWaitI(semaphore_t *s) {
  (*s) --;
}

void chSemSignalI(semaphore_t *s) {
  (*s) ++;
}

int chSemGetCounterI(semaphore_t *s) {
  return (*s);
}
////////////

void fifoInit(fifo_t * fifo, fifo_item_t * data, size_t size) {
  fifo->data = data;
  fifo->size = size / sizeof(fifo_item_t);
  fifo->write = 0;
  fifo->read = 0;
  fifo->first_handle = 0;
  fifo->last_handle = 0;
  chSemObjectInit(&fifo->avaliable, fifo->size);
  chSemObjectInit(&fifo->used, 0);
}

static inline void inc_handle(fifo_handle_t *h) {
  if (*h == HANDLE_MAX)
    (*h) = 0;
  else
    (*h) ++;
}

fifo_handle_t fifoPushWithHandle(fifo_t * fifo, fifo_item_t * data, systime_t timeout) {
  chDbgCheck(fifo != NULL);

  if (chSemWaitTimeout(&fifo->avaliable, timeout) != MSG_OK)
    return HANDLE_INVALID;

  fifo_handle_t ret;
  chSysLock();
  ret = fifo->last_handle;

  // Create a new handle for the next push
  inc_handle(&fifo->last_handle);

  fifo_item_t * q_data = &fifo->data[fifo->write];
  fifo->write = (fifo->write + 1) % fifo->size;
  chSysUnlock();

  memcpy(q_data, data, sizeof(fifo_item_t));
  q_data->status = CB_FIFO_QUEUED;

  // Allow pop only after copying the data
  chSemSignal(&fifo->used);
  return ret;
}

bool fifoPop(fifo_t * fifo, fifo_item_t * data, systime_t timeout) {
  chDbgCheck(fifo != NULL);

  if (chSemWaitTimeout(&fifo->used, timeout) != MSG_OK)
    return false;

  fifo_item_t * q_data;

  chSysLock();
  q_data = &fifo->data[fifo->read];
  fifo->read = (fifo->read + 1) % fifo->size;

  // Invalidade the read element handle
  inc_handle(&fifo->first_handle);

  chSysUnlock();

  if (q_data->status != CB_FIFO_DELETED) {
    if (data)
      memcpy(data, q_data, sizeof(fifo_item_t));

    if (q_data->callback)
      q_data->callback(q_data);
  }
  else {
    // Item is deleted, make the position avaliable and return failure
    chSemSignal(&fifo->avaliable);
    return false;
  }

  // Make the fifo position avaliable only after copying the data
  chSemSignal(&fifo->avaliable);

  return true;
}

fifo_handle_t fifoPushWithHandleI(fifo_t * fifo, fifo_item_t * data) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);
  chDbgCheck(data != NULL);

  if (chSemGetCounterI(&fifo->avaliable) == 0)
    return HANDLE_INVALID;

  // Create a new handle for the next push
  fifo_handle_t ret = fifo->last_handle;
  inc_handle(&fifo->last_handle);

  chSemFastWaitI(&fifo->avaliable);
  memcpy(&fifo->data[fifo->write], data, sizeof(fifo_item_t));
  fifo->data[fifo->write].status = CB_FIFO_QUEUED;
  fifo->write = (fifo->write + 1) % fifo->size;
  chSemSignalI(&fifo->used);
  return ret;
}

bool fifoPopI(fifo_t * fifo, fifo_item_t * data) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);

  if (chSemGetCounterI(&fifo->used) == 0)
    return false;

  chSemFastWaitI(&fifo->used);

  // Invalidade the read element handle
  inc_handle(&fifo->first_handle);

  fifo_item_t * q_data = &fifo->data[fifo->read];
  fifo->read = (fifo->read + 1) % fifo->size;

  if (q_data->status != CB_FIFO_DELETED) {
    if (data)
      memcpy(data, q_data, sizeof(fifo_item_t));

    if (q_data->callback)
      q_data->callback(q_data);
  }
  else {
    chSemSignalI(&fifo->avaliable);
    return false;
  }

  chSemSignalI(&fifo->avaliable);
  return true;
}

bool fifoDelete(fifo_t * fifo, fifo_handle_t handle) {
  chDbgCheck(fifo != NULL);

  if (handle == HANDLE_INVALID)
    return false;

  chSysLock();
  fifo_item_t * elem = NULL;

  if ((fifo->first_handle > fifo->last_handle) && (handle <= HANDLE_MAX)) {
    if (handle >= fifo->first_handle) {
      elem = &fifo->data[handle % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }

    if ((handle >= 0) && (handle < fifo->last_handle)) {
      elem = &fifo->data[(handle + HANDLE_MAX + 1) % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }
  }
  else {
    if ((handle >= fifo->first_handle) && (handle < fifo->last_handle)) {
      elem = &fifo->data[handle % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }
  }

  // DEBUG
  if (elem) {
    printf("Delete %d: %d\n", handle, (int)elem->data);
  }
  else
    printf("Delete %d: Invalido\n", handle);

  chSysUnlock();

  return elem != NULL;
}

