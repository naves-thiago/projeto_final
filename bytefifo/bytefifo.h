#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "glue.h"

/**
 * @defgroup bytefifo byte FIFO
 * @brief Non thread-safe circular byte FIFO with header / footer comparison functions.
 *         Push with a full FIFO overrides the oldest byte.
 * @{
 */

/**
 * @brief byte FIFO instance
 */
typedef struct {
  uint8_t * buffer;     /**< Data area (must be length + 1 in size) */
  uint16_t  write;      /**< Next write position                    */
  uint16_t  read;       /**< Oldest data position                   */
  uint16_t  length;     /**< FIFO length                            */
} BYTEFifo;


/**
 * @brief Initializes the FIFO and sets the data area
 *
 * @param[in] fifo The FIFO instance
 * @param[in] buffer The memory area to store the data
 * @param[in] length Length of @p buffer
 */
void byteFifoInit(BYTEFifo * fifo, uint8_t * buffer, uint16_t length);

/**
 * @brief Removes all data from the FIFO
 *
 * @param[in] fifo The FIFO instance
 */
void byteFifoClear(BYTEFifo * fifo);

/**
 * @brief Counts the number of queued bytes
 *
 * @param[in] fifo The FIFO instance
 * @return The number of bytes in the FIFO
 */
uint16_t byteFifoCount(BYTEFifo * fifo);

/**
 * @brief Adds a byte to the FIFO. If the FIFO is full, the oldest byte is replaced
 *
 * @param[in] fifo The FIFO instance
 * @param[in] c The byte to add
 */
void byteFifoPush(BYTEFifo * fifo, uint8_t c);

/**
 * @brief Copy the oldest bytes in the FIFO (up to @p count) to @p out, but keep them in the FIFO
 *
 * @param[in] fifo The FIFO instance
 * @param[out] out Copy destination
 * @param[in] count Maximum amount of bytes to copy
 *
 * @return The number of bytes actually copied
 *
 * @sa byteFifoPop()
 */
uint16_t byteFifoPeek(BYTEFifo * fifo, uint8_t * out, uint16_t count);

/**
 * @brief Access the bytes in the queue without copying them.
 *        Due to internal data organization, the data will likely be divided in 2 parts, accessed via @p out1 and @p out2.
 *
 * @attention @p out1 and @p out2 point to the internal data buffer. Modifying these will modify the stored data.
 *
 * @note If only @p out1 is used, @p out2 will be set to NULL and @p len2 to 0.
 *
 * @param[in]  fifo The FIFO instance
 * @param[out] out1 Pointer to the first (oldest) part of the data
 * @param[out] len1 Length of @p out1
 * @param[out] out2 Pointer to the second (newest) part of the data
 * @param[out] len2 Length of @p out2
 *
 * @sa byteFifoPeek()
 */
void byteFifoPeekNoCopy(BYTEFifo * fifo, uint8_t ** out1, uint16_t * len1, uint8_t ** out2, uint16_t * len2);

/**
 * @brief Move the oldest bytes in the FIFO (up to @p count) to @p out
 * @note If @p out is NULL, the bytes will be discarded instead
 *
 * @param[in] fifo The FIFO instance
 * @param[out] out Copy destination or NULL
 * @param[in] count Maximum amount of bytes to move
 *
 * @return The number of bytes actually moved
 *
 * @sa byteFifoPeek()
 */
uint16_t byteFifoPop(BYTEFifo * fifo, uint8_t * out, uint16_t count);

/**
 * @brief Compares the the @p count oldest bytes in the FIFO with the ones in @p b
 *
 * @note This function returns false if the @p count is larger the the FIFO length
 *
 * @param[in] fifo The FIFO instance
 * @param[in] b The vector to compare with
 * @param[in] count The amount of bytes to compare
 *
 * @return true if all the bytes match, false otherwise
 *
 * @sa byteFifoEndsWith()
 */
bool byteFifoBeginsWith(BYTEFifo * fifo, uint8_t * b, uint16_t count);

/**
 * @brief Compares the the @p count newest bytes in the FIFO with the ones in @p b
 *
 * @note This function returns false if the @p count is larger the the FIFO length
 *
 * @param[in] fifo The FIFO instance
 * @param[in] b The vector to compare with
 * @param[in] count The amount of bytes to compare
 *
 * @return true if all the bytes match, false otherwise
 *
 * @sa byteFifoBeginsWith()
 */
bool byteFifoEndsWith(BYTEFifo * fifo, uint8_t * b, uint16_t count);

/**
 * @brief This macro declares a FIFO and the data buffer and initializes them
 *
 * @param[in] name The FIFO variable name. The data vector will be called <name>_buffer
 * @param[in] len The data buffer length
 */
#define BYTEFifoDECL(name, len) uint8_t GLUE(name, _buffer)[len];              \
                                BYTEFifo name = {.write = 0,                   \
                                                 .read = 0,                    \
                                                 .length = len,                \
                                                 .buffer = GLUE(name, _buffer) \
                                                }

/** @} */
