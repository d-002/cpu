#include "binary_content.h"

#include <stdlib.h>

struct binary_content *binary_content_create(void)
{
    struct binary_content *content = malloc(sizeof(struct binary_content));
    if (content == NULL)
        return NULL;

    content->instructions = queue_create();
    content->length = 0;

    if (content->instructions == NULL)
    {
        free(content);
        return NULL;
    }

    return content;
}

void binary_content_destroy(struct binary_content *content)
{
    if (content == NULL)
        return;

    queue_destroy(content->instructions);
    free(content);
}
