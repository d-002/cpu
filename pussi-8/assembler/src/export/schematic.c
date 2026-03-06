#include "schematic.h"

#include <stdlib.h>

#include "export_utils.h"
#include "logger/logger.h"
#include "nbt/nbt.h"
#include "utils/errors.h"
#include "utils/specs.h"

#define STR_LEN(s) s, strlen(s)

#define ID_AIR 0
#define ID_STONE 1
#define ID_BARREL 2

struct pos
{
    short x;
    short y;
    short z;
    short i;
};

struct pos make_pos(short x, short y, short z, short w, short l)
{
    struct pos pos = {
        .x = x,
        .y = y,
        .z = z,
        .i = (y * l + z) * w + x,
    };

    return pos;
}

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

static nbt_tag_t *fill_shovels(int n_shovels)
{
    nbt_tag_t *items = nbt_new_tag_list(NBT_TYPE_COMPOUND);
    nbt_set_tag_name(items, STR_LEN("Items"));

    for (int i = 0; i < n_shovels; i++)
    {
        nbt_tag_t *shovel = nbt_new_tag_compound();
        nbt_tag_list_append(items, shovel);

        nbt_tag_t *count = nbt_new_tag_int(1);
        nbt_tag_compound_append(shovel, count);
        nbt_set_tag_name(count, STR_LEN("Count"));

        nbt_tag_t *slot = nbt_new_tag_int(i);
        nbt_tag_compound_append(shovel, slot);
        nbt_set_tag_name(slot, STR_LEN("Slot"));

        nbt_tag_t *id = nbt_new_tag_string(STR_LEN("minecraft:wooden_shovel"));
        nbt_tag_compound_append(shovel, id);
        nbt_set_tag_name(id, STR_LEN("Id"));
    }

    return items;
}

static int set_data(short data, struct pos pos, int8_t *data_arr,
                    nbt_tag_t *block_entities)
{
    data_arr[pos.i] = data == 0 ? ID_STONE : ID_BARREL;

    nbt_tag_t *barrel = nbt_new_tag_compound();
    nbt_tag_list_append(block_entities, barrel);

    int pos_arr[3] = { pos.x, pos.y, pos.z };
    nbt_tag_t *pos_tag = nbt_new_tag_int_array(pos_arr, 3);
    nbt_set_tag_name(pos_tag, STR_LEN("Pos"));
    nbt_tag_compound_append(barrel, pos_tag);

    nbt_tag_t *id = nbt_new_tag_string(STR_LEN("minecraft:barrel"));
    nbt_set_tag_name(id, STR_LEN("Id"));
    nbt_tag_compound_append(barrel, id);

    nbt_tag_t *data_tag = nbt_new_tag_compound();
    nbt_set_tag_name(data_tag, STR_LEN("Data"));
    nbt_tag_compound_append(barrel, data_tag);

    nbt_tag_t *id_inner = nbt_new_tag_string(STR_LEN("minecraft:barrel"));
    nbt_set_tag_name(id_inner, STR_LEN("Id"));
    nbt_tag_compound_append(data_tag, id_inner);

    nbt_tag_t *items = fill_shovels(2 * data + 1);
    nbt_tag_compound_append(data_tag, items);

    return SUCCESS;
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

    short w = 4 * 16 - 3;
    short h = 16;
    short l = 4 * 8;
    nbt_tag_t *width = nbt_new_tag_short(w);
    nbt_tag_t *height = nbt_new_tag_short(h);
    nbt_tag_t *length = nbt_new_tag_short(l);
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

    nbt_tag_t *palette_air = nbt_new_tag_int(ID_AIR);
    nbt_set_tag_name(palette_air, STR_LEN("minecraft:air"));
    nbt_tag_compound_append(palette, palette_air);
    nbt_tag_t *palette_stone = nbt_new_tag_int(ID_STONE);
    nbt_set_tag_name(palette_stone, STR_LEN("minecraft:stone"));
    nbt_tag_compound_append(palette, palette_stone);
    nbt_tag_t *palette_barrel = nbt_new_tag_int(ID_BARREL);
    nbt_set_tag_name(palette_barrel, STR_LEN("minecraft:barrel"));
    nbt_tag_compound_append(palette, palette_barrel);

    nbt_tag_t *block_entities = nbt_new_tag_list(NBT_TYPE_COMPOUND);
    nbt_set_tag_name(block_entities, STR_LEN("BlockEntities"));
    nbt_tag_compound_append(blocks, block_entities);

    size_t data_size = (size_t)w * h * l;
    int8_t *data_arr = malloc(data_size * sizeof(int8_t));
    memset(data_arr, ID_AIR, data_size * sizeof(int8_t));

    // encode data in the ROM barrels pattern, meaning a 16-bits instruction is
    // split into 4 barrels
    if (content->length > ROM_SIZE)
        logwarn(NO_LINE, "Program size is too large for the ROM: %ld > %ld",
                content->length, ROM_SIZE);

    for (size_t i = 0; i < ROM_SIZE; i++)
    {
        short instruction = i >= content->length ? 0
            : i == 0 ? *(short *)queue_iter_start(content)
                     : *(short *)queue_iter_next(content);

        short layer = i / 32;
        short x = layer * 4;
        short y1 = i % 4 * 2 + i / 4 % 2;
        short y2 = y1 + 8;
        short z1 = i / 4 % 8 * 4 + layer % 2;
        short z2 = z1 + 2;

        // depending on the value of the instruction, use a block entity or a
        // normal block
        // TODO: check opcode etc inversion
        set_data(instruction & 0x000f, make_pos(x, y1, z1, w, l), data_arr,
                 block_entities);
        set_data(instruction & 0x00f0, make_pos(x, y2, z1, w, l), data_arr,
                 block_entities);
        set_data(instruction & 0x0f00, make_pos(x, y1, z2, w, l), data_arr,
                 block_entities);
        set_data(instruction & 0xf000, make_pos(x, y2, z2, w, l), data_arr,
                 block_entities);
    }

    nbt_tag_t *data = nbt_new_tag_byte_array(data_arr, data_size);
    nbt_set_tag_name(data, STR_LEN("Data"));
    nbt_tag_compound_append(blocks, data);

    write_nbt_file(export_path, root, NBT_WRITE_FLAG_USE_GZIP);
    free(data_arr);
    nbt_free_tag(root);

    free(export_path);
    return SUCCESS;
}
