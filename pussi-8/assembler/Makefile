CFLAGS=-Wall -Wextra -Werror -Wvla -std=c99 -pedantic
CPPFLAGS=-Isrc
LDLIBS=-lm

TARGET=assembli
OBJ_MAIN=src/main.o
OBJ=src/opcodes/opcodes.o src/opcodes/aliases_alu.o src/opcodes/aliases_jmp.o src/opcodes/aliases_mem.o src/opcodes/aliases_utils.o \
	src/assembler/assembler.o src/assembler/expand_vars.o src/assembler/expand_labels.o src/assembler/to_machine.o src/assembler/to_machine_utils.o \
	src/cli/cli.o \
	src/export/binary.o src/export/schematic.o src/export/export_utils.o \
	src/files/files.o \
	src/hash_map/hash_map.o \
	src/lexer/lexer.o src/lexer/t_space.o src/lexer/t_identifier.o src/lexer/t_number.o src/lexer/t_data.o src/lexer/t_comment.o src/lexer/t_opcode.o src/lexer/t_simple.o src/lexer/t_eol.o src/lexer/token.o \
	src/logger/logger.o \
	src/parser/parser.o src/parser/state.o src/parser/instruction.o src/parser/parser_utils.o src/parser/state_instruction.o src/parser/state_assignation.o \
	src/queue/queue.o \
	src/utils/numstr.o src/utils/checks.o \
	src/nbt/nbt.o src/nbt/miniz.o

TARGET_UNIT=tests/unit
TARGET_FUNC=tests/testsuite.sh
OBJ_UNIT=tests/unit_queue.o tests/unit_hashmap.o tests/unit_lexer.o tests/unit_cli.o tests/unit_parser_utils.o tests/unit_parser.o tests/unit_vars.o tests/unit_other.o tests/unit_labels.o

.PHONY: all check dev coverage clean

all: $(TARGET)

$(TARGET): $(OBJ_MAIN) $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(TARGET_UNIT): CFLAGS+=-lcriterion -g
$(TARGET_UNIT): LDLIBS+=-lcriterion
$(TARGET_UNIT): $(OBJ) $(OBJ_UNIT)
	$(CC) -o $(TARGET_UNIT) $^ $(LDFLAGS) $(LDLIBS)

check: LDLIBS+=-lcriterion
check: $(TARGET) $(TARGET_UNIT)
	./$(TARGET_UNIT)
	./$(TARGET_FUNC)

dev: CFLAGS+=-fsanitize=address -g
dev: LDLIBS+=-fsanitize=address
dev: check

coverage: CFLAGS+=--coverage -fPIC
coverage: LDLIBS+=-lgcov
coverage: LDFLAGS+=--coverage
coverage: dev
	gcovr --html --html-nested --output=report.html
	xdg-open report.html

clean:
	# main clean
	$(RM) $(TARGET) $(OBJ_MAIN) $(OBJ)
	$(RM) $(TARGET_UNIT) $(OBJ_UNIT)

	# coverage clean
	$(RM) *.html *.css *.js
	find . -type f -name *.gcno -exec $(RM) {} +
	find . -type f -name *.gcda -exec $(RM) {} +
