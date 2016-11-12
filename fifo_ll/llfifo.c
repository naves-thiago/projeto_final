/* Linked list FIFO lib */
#include "llfifo.h"
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

void chDbgAssert(bool v, char * msg) {
  if (!v) {
    printf("Error: %s\n", msg);
    while(1);
  }
}

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

void llFifoInit(LLFifo * fifo) {
  fifo->head = NULL;
  fifo->tail = NULL;
  chSemObjectInit(&fifo->count, 0);
}

LLFifoItem * llFifoPop(LLFifo * fifo, systime_t timeout) {
  chDbgCheck(fifo != NULL);

  if (chSemWaitTimeout(&fifo->count, timeout) != MSG_OK)
    return false;

  chSysLock();
  LLFifoItem * ret = fifo->head;
  if (fifo->head == fifo->tail) {
    fifo->head = NULL;
    fifo->tail = NULL;
  }
  else
    fifo->head = fifo->head->next;
  chSysUnlock();

  return ret;
}

LLFifoItem * llFifoPopI(LLFifo * fifo) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);

  if (chSemGetCounterI(&fifo->count) == 0)
    return false;

  chSemFastWaitI(&fifo->count);

  LLFifoItem * ret = fifo->head;
  if (fifo->head == fifo->tail) {
    fifo->head = NULL;
    fifo->tail = NULL;
  }
  else
    fifo->head = fifo->head->next;

  return ret;
}

void llFifoPushI(LLFifo * fifo, LLFifoItem * item) {
  chDbgCheckClassI();
  chDbgCheck(fifo != NULL);
  chDbgCheck(item != NULL);

  if (fifo->tail == NULL) {
    fifo->tail = item;
    fifo->head = item;
    item->next = NULL;
    item->prev = NULL;
  }
  else {
    fifo->tail->next = item;
    item->prev = fifo->tail;
    fifo->tail = item;
  }
  item->status = CB_FIFO_QUEUED;

  chSemSignalI(&fifo->count);
}

void llFifoPush(LLFifo * fifo, LLFifoItem * item) {
  chDbgCheck(fifo != NULL);
  chDbgCheck(item != NULL);

  chSysLock();
  if (fifo->tail == NULL) {
    fifo->tail = item;
    fifo->head = item;
    item->next = NULL;
    item->prev = NULL;
  }
  else {
    fifo->tail->next = item;
    item->prev = fifo->tail;
    fifo->tail = item;
  }
  item->status = CB_FIFO_QUEUED;
  chSysUnlock();

  chSemSignal(&fifo->count);
}

bool llFifoDelete(LLFifo * fifo, LLFifoItem * item) {
  chDbgCheck(fifo != NULL);
  chDbgCheck(item != NULL);

  if (item->status == CB_FIFO_DELETED)
    return true;

  if (item->status == CB_FIFO_READ)
    return false;

  chSysLock();
  if (item->status == CB_FIFO_QUEUED) {
    item->status = CB_FIFO_DELETED;
    if (fifo->head == item)
      fifo->head = item->next;

    if (fifo->tail == item)
      fifo->tail = item->prev;

    if (item->prev)
      item->prev->next = item->next;

    if (item->next)
      item->next->prev = item->prev;

    chSysUnlock();
    return true;
  }

  /* Unexpected / invalid status code. should never get here */
  chDbgAssert(false, "Invalid item status");
  chSysUnlock();
  return false;
}

