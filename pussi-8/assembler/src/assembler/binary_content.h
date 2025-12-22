#ifndef BINARY_CONTENT_H
#define BINARY_CONTENT_H

#include "queue.h"

/**
 * Wrapper around a queue of uint16_t
 * (8 bits opcode) (8 bits arguments)
 */

struct binary_content
{
    struct queue *instructions;
    size_t length;
};

struct binary_content *binary_content_create(void);
void binary_content_destroy(struct binary_content *content);

#endif /* ! BINARY_CONTENT_H */
