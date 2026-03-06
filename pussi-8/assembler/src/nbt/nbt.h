/*
  libnbt
  Written in 2019 by IDidMakeThat

  To the extent possible under law, the author(s) have dedicated all copyright
  and related and neighboring rights to this software to the public domain
  worldwide. This software is distributed without any warranty.

  You should have received a copy of the CC0 Public Domain Dedication along
  with this software. If not, see
  <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef NBT_H
#define NBT_H

#ifdef __cplusplus
extern "C"
{
#endif

#define LIBNBT_VERSION 10
#define LIBNBT_NBT_VERSION 19133

#include <stddef.h>

#ifndef NBT_NO_STDLIB
#    include <stdlib.h>
#    include <string.h>
#    define NBT_MALLOC malloc
#    define NBT_REALLOC realloc
#    define NBT_FREE free
#    define NBT_MEMCPY memcpy
#    define NBT_MEMCMP memcmp
#endif

#ifndef NBT_NO_STDINT
#    include <stdint.h>
#endif

#ifdef NBT_OWN_ZLIB
#    include NBT_OWN_ZLIB
#else
#    include <zlib.h>
#endif

#ifndef Z_DEFAULT_WINDOW_BITS
#    define Z_DEFAULT_WINDOW_BITS 15
#endif

#ifndef NBT_BUFFER_SIZE
#    define NBT_BUFFER_SIZE 32768
#endif

#define NBT_COMPRESSION_LEVEL 9

    typedef enum
    {
        NBT_TYPE_END,
        NBT_TYPE_BYTE,
        NBT_TYPE_SHORT,
        NBT_TYPE_INT,
        NBT_TYPE_LONG,
        NBT_TYPE_FLOAT,
        NBT_TYPE_DOUBLE,
        NBT_TYPE_BYTE_ARRAY,
        NBT_TYPE_STRING,
        NBT_TYPE_LIST,
        NBT_TYPE_COMPOUND,
        NBT_TYPE_INT_ARRAY,
        NBT_TYPE_LONG_ARRAY,
        NBT_NO_OVERRIDE // Only used internally.
    } nbt_tag_type_t;

    typedef struct nbt_tag_t nbt_tag_t;

    struct nbt_tag_t
    {
        nbt_tag_type_t type;

        char *name;
        size_t name_size;

        union
        {
            struct
            {
                int8_t value;
            } tag_byte;
            struct
            {
                int16_t value;
            } tag_short;
            struct
            {
                int32_t value;
            } tag_int;
            struct
            {
                int64_t value;
            } tag_long;
            struct
            {
                float value;
            } tag_float;
            struct
            {
                double value;
            } tag_double;
            struct
            {
                int8_t *value;
                size_t size;
            } tag_byte_array;
            struct
            {
                char *value;
                size_t size;
            } tag_string;
            struct
            {
                nbt_tag_t **value;
                nbt_tag_type_t type;
                size_t size;
            } tag_list;
            struct
            {
                nbt_tag_t **value;
                size_t size;
            } tag_compound;
            struct
            {
                int32_t *value;
                size_t size;
            } tag_int_array;
            struct
            {
                int64_t *value;
                size_t size;
            } tag_long_array;
        } payload;
    };

    typedef struct
    {
        size_t (*read)(void *userdata, uint8_t *data, size_t size);
        void *userdata;
    } nbt_reader_t;

    typedef struct
    {
        size_t (*write)(void *userdata, uint8_t *data, size_t size);
        void *userdata;
    } nbt_writer_t;

    typedef enum
    {
        NBT_PARSE_FLAG_USE_GZIP = 1,
        NBT_PARSE_FLAG_USE_ZLIB = 2,
        NBT_PARSE_FLAG_USE_RAW = 3,
    } nbt_parse_flags_t;

    typedef enum
    {
        NBT_WRITE_FLAG_USE_GZIP = 1,
        NBT_WRITE_FLAG_USE_ZLIB = 2,
        NBT_WRITE_FLAG_USE_RAW = 3
    } nbt_write_flags_t;

    nbt_tag_t *nbt_parse(nbt_reader_t reader, int parse_flags);
    void nbt_write(nbt_writer_t writer, nbt_tag_t *tag, int write_flags);

    nbt_tag_t *nbt_new_tag_byte(int8_t value);
    nbt_tag_t *nbt_new_tag_short(int16_t value);
    nbt_tag_t *nbt_new_tag_int(int32_t value);
    nbt_tag_t *nbt_new_tag_long(int64_t value);
    nbt_tag_t *nbt_new_tag_float(float value);
    nbt_tag_t *nbt_new_tag_double(double value);
    nbt_tag_t *nbt_new_tag_byte_array(int8_t *value, size_t size);
    nbt_tag_t *nbt_new_tag_string(const char *value, size_t size);
    nbt_tag_t *nbt_new_tag_list(nbt_tag_type_t type);
    nbt_tag_t *nbt_new_tag_compound(void);
    nbt_tag_t *nbt_new_tag_int_array(int32_t *value, size_t size);
    nbt_tag_t *nbt_new_tag_long_array(int64_t *value, size_t size);

    void nbt_set_tag_name(nbt_tag_t *tag, const char *name, size_t size);

    void nbt_tag_list_append(nbt_tag_t *list, nbt_tag_t *value);
    nbt_tag_t *nbt_tag_list_get(nbt_tag_t *tag, size_t index);
    void nbt_tag_compound_append(nbt_tag_t *compound, nbt_tag_t *value);
    nbt_tag_t *nbt_tag_compound_get(nbt_tag_t *tag, const char *key);

    void nbt_free_tag(nbt_tag_t *tag);

#ifdef __cplusplus
}
#endif

#endif
