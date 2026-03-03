#include "schematic.h"

#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "nbt/nbt.h"
#include "utils/errors.h"

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

int to_schematic(struct cli_args *args, char *path, struct queue *content)
{
    char *export_path = change_extension(path, "schem");
    if (export_path == NULL)
        return ALLOC_ERROR;

    verbose(args, NO_LINE, "Exporting worldedit schematic to '%s'.",
            export_path);
    content++;

    // ok like I'm not checking memory here sorry
    nbt_tag_t *root = nbt_new_tag_compound();
    nbt_set_tag_name(root, "Schematic", 9);
    // nbt_tag_t *width = nbt_new_tag_short(4 * 8);
    // nbt_tag_t *height = nbt_new_tag_short(1 * 16);
    // nbt_tag_t *length = nbt_new_tag_short(4 * 16);
    nbt_tag_t *width = nbt_new_tag_short(4);
    nbt_tag_t *height = nbt_new_tag_short(4);
    nbt_tag_t *length = nbt_new_tag_short(4);
    nbt_tag_t *materials = nbt_new_tag_string("Alpha", 5);
    nbt_set_tag_name(width, "Width", 5);
    nbt_set_tag_name(height, "Height", 6);
    nbt_set_tag_name(length, "Length", 6);
    nbt_set_tag_name(materials, "Materials", 9);
    nbt_tag_compound_append(root, width);
    nbt_tag_compound_append(root, height);
    nbt_tag_compound_append(root, length);
    nbt_tag_compound_append(root, materials);

    int8_t *arr_temp = malloc(64 * sizeof(int8_t));
    for (size_t i = 0; i < 64; i++)
        arr_temp[i] = 1;

    nbt_tag_t *blocks_temp = nbt_new_tag_byte_array(arr_temp, 64);
    nbt_set_tag_name(blocks_temp, "Blocks", 6);
    nbt_tag_compound_append(root, blocks_temp);

    nbt_tag_t *data = nbt_new_tag_byte_array(NULL, 0);
    nbt_set_tag_name(data, "Data", 4);
    nbt_tag_compound_append(root, data);

    nbt_tag_t *icon = nbt_new_tag_compound();
    nbt_set_tag_name(icon, "Icon", 4);
    nbt_tag_compound_append(root, icon);

    nbt_tag_t *icon_id = nbt_new_tag_string("minecraft:stone", 15);
    nbt_set_tag_name(icon_id, "id", 2);
    nbt_tag_compound_append(icon, icon_id);

    nbt_tag_t *mappings = nbt_new_tag_compound();
    nbt_set_tag_name(mappings, "SchematicaMapping", 17);
    nbt_tag_compound_append(root, mappings);

    nbt_tag_t *mapping_1 = nbt_new_tag_short(1);
    nbt_set_tag_name(mapping_1, "minecraft:stone", 15);
    nbt_tag_compound_append(mappings, mapping_1);

    write_nbt_file(export_path, root, NBT_WRITE_FLAG_USE_ZLIB);
    nbt_free_tag(root);
    free(arr_temp);

    free(export_path);
    return SUCCESS;
}
