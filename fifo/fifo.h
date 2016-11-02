/* Generic thread safe FIFO lib */
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


struct CBFifoItem;
typedef struct CBFifoItem CBFifoItem;

/**
 * @brief Callback function pointer.
 */
typedef void (*CBFifoCb)(CBFifoItem * item);

/**
 * @brief Queued item state options.
 */
typedef enum {
  CB_FIFO_QUEUED,  /**< Item is queued and valid. */
  CB_FIFO_DELETED  /**< Item has been deleted using @p cbFifoDelete(). */
} CBFifoStatus;

/**
 * @brief This struct represents an item that can be put in the queue.
 */
struct CBFifoItem {
  void * data;            /**< Optional pointer to some data to be used by whoever pops the item (not used by this module). */
  intptr_t callbackData;  /**< This will be passed as a parameter to the callback function. */
  CBFifoCb callback;      /**< Pointer to a callback function called when this item is popped or NULL. */
  size_t dataSize;        /**< Optional size information about this item's data field (not used by this module). */
  CBFifoStatus status;    /**< Item status flag. */
};

typedef unsigned int CBFifoHandle; /**< Type that holds an unique identifier of an item. */

#define CB_FIFO_HANDLE_INVALID (UINT_MAX) /**< Unique value representing an invalid item handle, used as an error flag. */
#define CB_FIFO_HANDLE_MAX (UINT_MAX - 1) /**< Highest value allowed as an item handle. */
//#define CB_FIFO_HANDLE_MAX 7

/**
 * @brief CBFifo instance data. The data in this struct is not meant to be accessed directly.
 */
typedef struct {
  CBFifoItem * data;          /**< Queue data area. Holds the queued items and avaliable item slots. */
  size_t size;                /**< Queue length (maximum number of items). */
  size_t write;               /**< Index of the next write position. */
  size_t read;                /**< Index of the current read position. */
  CBFifoHandle firstHandle;   /**< Handle of the oldest item in the queue. */
  CBFifoHandle lastHandle;    /**< Handle of the newest item in the queue. */
  semaphore_t avaliable;      /**< Avaliable slots count semaphore. Adds the option to block threads trying to push items. */
  semaphore_t used;           /**< Used slots count semaphore. Adds the option to block threads trying to pop items. */
} CBFifo;

/**
 * @brief Setups the fifo struct as an empty queue.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] data Memory area to store the queue items.
 * @param[in] size data size in bytes.
 */
void cbFifoInit(CBFifo * fifo, CBFifoItem * data, size_t size);

/**
 * @brief Copies an item to the queue.
 * @note The item's data and callbackData contents are not copied, only the pointers are.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] data Item to be queued.
 * @param[in] timeout Maximum time to block the thread waiting for a slot in the queue.
 *
 * @return An item handle if the item was queued.
 * @retval @p CB_FIFO_HANDLE_INVALID in case of a timeout.
 */
CBFifoHandle cbFifoPushWithHandle(CBFifo * fifo, CBFifoItem * data, systime_t timeout);

/**
 * @brief Copies an item to the queue from an ISR.
 * @note The item's data and callbackData contents are not copied, only the pointers are.
 *
 * @param[in] fifo Pointer to the fifo struct.
 * @param[in] data Item to be queued.
 * @param[in] timeout Maximum time to block the thread waiting for a slot in the queue.
 *
 * @return An item handle if the item was queued.
 * @retval @p CB_FIFO_HANDLE_INVALID in case of a timeout.
 */
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

