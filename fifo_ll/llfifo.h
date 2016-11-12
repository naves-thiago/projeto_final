#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

/////////
// ChibiOS provided types
typedef int semaphore_t;
typedef int systime_t;
////////

/**
 * @defgroup cbfifo Callback FIFO
 * @detail Thread safe FIFO with pop callback and item delete support.
 * @{
 */

typedef struct LLFifoItem LLFifoItem;

/**
 * @brief Queued item state options.
 */
typedef enum {
  CB_FIFO_READ,    /**< Item is no longer queued. */
  CB_FIFO_QUEUED,  /**< Item is queued and valid. Do not modify items while they are in this state. */
  CB_FIFO_DELETED  /**< Item has been deleted using @p llFifoDelete(). */
} LLFifoStatus;

/**
 * @brief This struct represents the linked list part of an item that can be put in the queue.
 * Add this struct as the first member of a new struct that also contains the data and add that to the queue using a typecast.
 *
 * @note Do not modify the data in this struct.
 */
struct LLFifoItem {
  LLFifoItem * next;      /**< Next item in queue (queued after this one) */
  LLFifoItem * prev;      /**< Previous item in queue (queues before this one) */
  LLFifoStatus status;    /**< Item status flag. */
  int data[];             /**< The outter struct starts here */
};

/**
 * @brief LLFifo instance data. The data in this struct is not meant to be accessed directly.
 */
typedef struct {
  LLFifoItem * head;   /**< Oldest item in the queue. Next to be dequeued. */
  LLFifoItem * tail;   /**< Newest item in the queue. */
  semaphore_t count;   /**< Queued items count semaphore. Adds the option to block threads trying to pop items. */
} LLFifo;

/**
 * @brief Setups the fifo struct as an empty queue.
 *
 * @param[in] fifo Pointer to the fifo struct.
 */
void llFifoInit(LLFifo * fifo);

/**
 * @brief Returns a pointer to the oldest item in the queue or NULL.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] timeout Maximum amount of time to block the thread waiting for a item.
 *
 * @return item pointer on success, NULL otherwise.
 *
 * @sa llFifoPopI()
 */
LLFifoItem * llFifoPop(LLFifo * fifo, systime_t timeout);

/**
 * @brief Returns a pointer to the oldest item in the queue or NULL from an ISR.
 *
 * @param[in] fifo Pointer to the fifo struct.
 *
 * @return item pointer on success, NULL otherwise.
 *
 * @sa llFifoPop()
 */
LLFifoItem * llFifoPopI(LLFifo * fifo);

/**
 * @brief Flags a queued item as deleted (@p CB_FIFO_DELETED) and removes it from the queue.
 * @note It's safe to reuse or free the item on success.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] item Pointer to the item struct.
 *
 * @return true on success, false otherwise.
 */
bool llFifoDelete(LLFifo * fifo, LLFifoItem * item);

/**
 * @brief Adds an item to the queue from an ISR.
 * @note The item is not copied to the queue, only a pointer is added.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] item Item to be queued.
 *
 * @sa llFifoPush() llFifoPop() llFifoPopI()
 */
void llFifoPushI(LLFifo * fifo, LLFifoItem * item);

/**
 * @brief Adds an item to the queue.
 * @note The item is not copied to the queue, only a pointer is added.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] item Item to be queued.
 *
 * @sa llFifoPushI() llFifoPop() llFifoPopI()
 */
void llFifoPush(LLFifo * fifo, LLFifoItem * item);

/** @} */
