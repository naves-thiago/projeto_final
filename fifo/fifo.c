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
  chMtxObjectInit(&fifo->mutex);
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
  if (!fifo) {
    return HANDLE_INVALID;
  }

  if (chSemWaitTimeout(&fifo->avaliable, timeout) != MSG_OK) {
    return HANDLE_INVALID;
  }

  fifo_handle_t ret;
  chMtxLock(&fifo->mutex);
  ret = fifo->last_handle;

  // Create a new handle for the next push
  inc_handle(&fifo->last_handle);

  // TODO mover o memcpy para fora do mutex. semaforo used garante que os dados nao serao sobrescritos
  // TODO regiao critica para concorrencia com IRQ
  memcpy(&fifo->data[fifo->write], data, sizeof(fifo_item_t));
  fifo->data[fifo->write].status = CB_FIFO_QUEUED;
  fifo->write = (fifo->write + 1) % fifo->size;
  chMtxUnlock(&fifo->mutex);
  chSemSignal(&fifo->used);
  return ret;
}

bool fifoPop(fifo_t * fifo, fifo_item_t * data, systime_t timeout) {
  if (!fifo) {
    return false;
  }

  if (chSemWaitTimeout(&fifo->used, timeout) != MSG_OK) {
    return false;
  }

  // TODO mover o memcpy para fora do mutex. semaforo avaliable garante que os dados nao serao sobrescritos
  // TODO regiao critica para concorrencia com IRQ
  chMtxLock(&fifo->mutex);
  if (data) {
    memcpy(data, &fifo->data[fifo->read], sizeof(fifo_item_t));
  }
  fifo->read = (fifo->read + 1) % fifo->size;

  // Invalidade the read element handle
  inc_handle(&fifo->first_handle);

  chMtxUnlock(&fifo->mutex);
  chSemSignal(&fifo->avaliable);

  if ((data->callback) && (data->status != CB_FIFO_DELETED))
    data->callback(data);

  return true;
}

fifo_handle_t fifoPushWithHandleI(fifo_t * fifo, fifo_item_t * data) {
  if (!fifo) {
    return HANDLE_INVALID;
  }

//  chSysLockFromISR();
  if (chSemGetCounterI(&fifo->avaliable) == 0) {
//    chSysUnlockFromISR();
    return HANDLE_INVALID;
  }

  // Create a new handle for the next push
  fifo_handle_t ret = fifo->last_handle;
  inc_handle(&fifo->last_handle);

  chSemFastWaitI(&fifo->avaliable);
  memcpy(&fifo->data[fifo->write], data, sizeof(fifo_item_t));
  fifo->data[fifo->write].status = CB_FIFO_QUEUED;
  fifo->write = (fifo->write + 1) % fifo->size;
  chSemSignalI(&fifo->used);
//  chSysUnlockFromISR();
  return ret;
}

bool fifoPopI(fifo_t * fifo, fifo_item_t * data) {
  if (!fifo) {
    return false;
  }

//  chSysLockFromISR();
  if (chSemGetCounterI(&fifo->used) == 0) {
//    chSysUnlockFromISR();
    return false;
  }

  // Invalidade the read element handle
  inc_handle(&fifo->first_handle);

  if (data) {
    memcpy(data, &fifo->data[fifo->read], sizeof(fifo_item_t));
  }
  fifo->read = (fifo->read + 1) % fifo->size;
  chSemSignalI(&fifo->avaliable);
//  chSysUnlockFromISR();

  if ((data->callback) && (data->status != CB_FIFO_DELETED))
    data->callback(data);

  return true;
}

bool fifoDelete(fifo_t * fifo, fifo_handle_t handle) {
  if (handle == HANDLE_INVALID)
    return false;

  chMtxLock(&fifo->mutex);
  fifo_item_t * elem = NULL;
  size_t index = handle % fifo->size;

  if (fifo->first_handle > fifo->last_handle) {
    if ((handle >= fifo->first_handle) || ((handle >= 0) && (handle < fifo->last_handle)))
      elem = &fifo->data[index];
  }
  else {
    if ((handle >= fifo->first_handle) && (handle < fifo->last_handle))
      elem = &fifo->data[index];
  }
  chMtxUnlock(&fifo->mutex);

  // DEBUG
  if (elem) {
    elem->status = CB_FIFO_DELETED;
    printf("Delete %d: %d\n", handle, (int)elem->data);
  }
  else
    printf("Delete %d: Invalido\n", handle);

  return elem != NULL;
}

