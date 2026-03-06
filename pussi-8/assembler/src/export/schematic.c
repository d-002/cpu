#include "schematic.h"

#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "nbt/nbt.h"
#include "utils/errors.h"

#define STR_LEN(s) s, strlen(s)

static size_t writer_write(void *userdata, uint8_t *data, size_t size)
{
    return fwrite(data, 1, size, userdata);
}

void write_nbt_file(const char *name, nbt_tag_t *tag, int flags)
{
    FILE *file = fopen(name, "wb");

    nbt_writer_t writer;

    writer.write = writer_write;
    writer.userdata = file;

    nbt_write(writer, tag, flags);

    fclose(file);
}

// ok like I'm not checking memory here sorry
int to_schematic(struct cli_args *args, char *path, struct queue *content)
{
    char *export_path = change_extension(path, "schem");
    if (export_path == NULL)
        return ALLOC_ERROR;

    verbose(args, NO_LINE, "Exporting worldedit schematic to '%s'.",
            export_path);

    nbt_tag_t *root = nbt_new_tag_compound();

    nbt_tag_t *schematic = nbt_new_tag_compound();
    nbt_set_tag_name(schematic, STR_LEN("Schematic"));
    nbt_tag_compound_append(root, schematic);

    nbt_tag_t *version = nbt_new_tag_int(3);
    nbt_set_tag_name(version, STR_LEN("Version"));
    nbt_tag_compound_append(schematic, version);

    nbt_tag_t *data_version = nbt_new_tag_int(4189);
    nbt_set_tag_name(data_version, STR_LEN("DataVersion"));
    nbt_tag_compound_append(schematic, data_version);

    nbt_tag_t *metadata = nbt_new_tag_compound();
    nbt_set_tag_name(metadata, STR_LEN("Metadata"));
    nbt_tag_compound_append(schematic, metadata);

    bool found;
    size_t i = get_extension_dot_index(path, &found);
    if (found)
        path[i] = '\0';
    nbt_tag_t *name = nbt_new_tag_string(STR_LEN(path));
    if (found)
        path[i] = '.';
    nbt_set_tag_name(name, STR_LEN("Name"));
    nbt_tag_compound_append(metadata, name);

    // TODO: rest of metadata

    // nbt_tag_t *width = nbt_new_tag_short(4 * 8);
    // nbt_tag_t *height = nbt_new_tag_short(1 * 16);
    // nbt_tag_t *length = nbt_new_tag_short(4 * 16);
    nbt_tag_t *width = nbt_new_tag_short(4);
    nbt_tag_t *height = nbt_new_tag_short(1);
    nbt_tag_t *length = nbt_new_tag_short(4);
    nbt_set_tag_name(width, STR_LEN("Width"));
    nbt_set_tag_name(height, STR_LEN("Height"));
    nbt_set_tag_name(length, STR_LEN("Length"));
    nbt_tag_compound_append(schematic, width);
    nbt_tag_compound_append(schematic, height);
    nbt_tag_compound_append(schematic, length);

    int offset_arr[3] = { 0, 0, 0 };
    nbt_tag_t *offset = nbt_new_tag_int_array(offset_arr, 3);
    nbt_set_tag_name(offset, STR_LEN("Offset"));
    nbt_tag_compound_append(schematic, offset);

    nbt_tag_t *blocks = nbt_new_tag_compound();
    nbt_set_tag_name(blocks, STR_LEN("Blocks"));
    nbt_tag_compound_append(schematic, blocks);

    nbt_tag_t *palette = nbt_new_tag_compound();
    nbt_set_tag_name(palette, STR_LEN("Palette"));
    nbt_tag_compound_append(blocks, palette);

    nbt_tag_t *temp_palette_elt = nbt_new_tag_int(1);
    nbt_set_tag_name(temp_palette_elt, STR_LEN("minecraft:rail"));
    nbt_tag_compound_append(palette, temp_palette_elt);

    int8_t *arr_temp = malloc(64 * sizeof(int8_t));
    for (size_t i = 0; i < 64; i++)
        arr_temp[i] = 1;

    nbt_tag_t *blocks_data = nbt_new_tag_byte_array(arr_temp, 64);
    nbt_set_tag_name(blocks_data, STR_LEN("Data"));
    nbt_tag_compound_append(blocks, blocks_data);

    nbt_tag_t *block_entities = nbt_new_tag_list(NBT_TYPE_COMPOUND);
    nbt_set_tag_name(block_entities, STR_LEN("BlockEntities"));
    nbt_tag_compound_append(blocks, block_entities);

    write_nbt_file(export_path, root, NBT_WRITE_FLAG_USE_GZIP);
    free(arr_temp);
    nbt_free_tag(root);

    free(export_path);
    content++; /////
    return SUCCESS;
}
