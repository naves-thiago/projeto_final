/* Pointer FIFO lib */
#include "fifo.h"
#include <string.h>
#include <string.h>

////////////
// These functions simulate the ChibiOS services
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

void cbFifoInit(CBFifo * fifo, CBFifoItem * data, size_t size) {
  fifo->data = data;
  fifo->size = size / sizeof(CBFifoItem);
  fifo->write = 0;
  fifo->read = 0;
  fifo->firstHandle = 0;
  fifo->lastHandle = 0;
  chSemObjectInit(&fifo->avaliable, fifo->size);
  chSemObjectInit(&fifo->used, 0);
}

static inline void incHandle(CBFifoHandle *h) {
  if (*h == CB_FIFO_HANDLE_MAX)
    (*h) = 0;
  else
    (*h) ++;
}

CBFifoHandle cbFifoPushWithHandle(CBFifo * fifo, CBFifoItem * data, systime_t timeout) {
  chDbgCheck(fifo != NULL);
  chDbgCheck(data != NULL);

  if (chSemWaitTimeout(&fifo->avaliable, timeout) != MSG_OK)
    return CB_FIFO_HANDLE_INVALID;

  CBFifoHandle ret;
  chSysLock();
  ret = fifo->lastHandle;

  // Create a new handle for the next push
  incHandle(&fifo->lastHandle);

  CBFifoItem * q_data = &fifo->data[fifo->write];
  fifo->write = (fifo->write + 1) % fifo->size;
  chSysUnlock();

  memcpy(q_data, data, sizeof(CBFifoItem));
  q_data->status = CB_FIFO_QUEUED;

  // Allow pop only after copying the data
  chSemSignal(&fifo->used);
  return ret;
}

bool cbFifoPop(CBFifo * fifo, CBFifoItem * data, systime_t timeout) {
  chDbgCheck(fifo != NULL);

  if (chSemWaitTimeout(&fifo->used, timeout) != MSG_OK)
    return false;

  CBFifoItem * q_data;

  chSysLock();
  q_data = &fifo->data[fifo->read];
  fifo->read = (fifo->read + 1) % fifo->size;

  // Invalidade the read element handle
  incHandle(&fifo->firstHandle);

  chSysUnlock();

  if (q_data->status != CB_FIFO_DELETED) {
    if (data)
      memcpy(data, q_data, sizeof(CBFifoItem));

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

CBFifoHandle cbFifoPushWithHandleI(CBFifo * fifo, CBFifoItem * data) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);
  chDbgCheck(data != NULL);

  if (chSemGetCounterI(&fifo->avaliable) == 0)
    return CB_FIFO_HANDLE_INVALID;

  // Create a new handle for the next push
  CBFifoHandle ret = fifo->lastHandle;
  incHandle(&fifo->lastHandle);

  chSemFastWaitI(&fifo->avaliable);
  memcpy(&fifo->data[fifo->write], data, sizeof(CBFifoItem));
  fifo->data[fifo->write].status = CB_FIFO_QUEUED;
  fifo->write = (fifo->write + 1) % fifo->size;
  chSemSignalI(&fifo->used);
  return ret;
}

bool cbFifoPopI(CBFifo * fifo, CBFifoItem * data) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);

  if (chSemGetCounterI(&fifo->used) == 0)
    return false;

  chSemFastWaitI(&fifo->used);

  // Invalidade the read element handle
  incHandle(&fifo->firstHandle);

  CBFifoItem * q_data = &fifo->data[fifo->read];
  fifo->read = (fifo->read + 1) % fifo->size;

  if (q_data->status != CB_FIFO_DELETED) {
    if (data)
      memcpy(data, q_data, sizeof(CBFifoItem));

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

bool cbFifoDelete(CBFifo * fifo, CBFifoHandle handle) {
  chDbgCheck(fifo != NULL);

  if (handle == CB_FIFO_HANDLE_INVALID)
    return false;

  chSysLock();
  CBFifoItem * elem = NULL;

  if ((fifo->firstHandle > fifo->lastHandle) && (handle <= CB_FIFO_HANDLE_MAX)) {
    if (handle >= fifo->firstHandle) {
      elem = &fifo->data[handle % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }

    if ((handle >= 0) && (handle < fifo->lastHandle)) {
      elem = &fifo->data[(handle + CB_FIFO_HANDLE_MAX + 1) % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }
  }
  else {
    if ((handle >= fifo->firstHandle) && (handle < fifo->lastHandle)) {
      elem = &fifo->data[handle % fifo->size];
      elem->status = CB_FIFO_DELETED;
    }
  }

  // DEBUG
  if (elem) {
    printf("Delete %u: %d\n", handle, (int)elem->data);
  }
  else
    printf("Delete %u: Invalido\n", handle);

  chSysUnlock();

  return elem != NULL;
}

