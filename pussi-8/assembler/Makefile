CFLAGS=-Wall -Wextra -Werror -Wvla -std=c99 -pedantic
CPPFLAGS=-Isrc

TARGET=assembli
OBJ_MAIN=src/main.o
OBJ=src/cli.o src/logger.o src/queue.o src/files.o src/hash_map.o \
	src/lexer/lexer.o src/lexer/t_space.o src/lexer/t_identifier.o src/lexer/t_number.o src/lexer/t_data.o src/lexer/t_comment.o src/lexer/t_opcode.o src/lexer/t_simple.o src/lexer/t_eol.o src/lexer/token.o \
	src/parser/parser.o src/parser/state.o src/parser/parser_utils.o src/parser/state_instruction.o src/parser/state_assignation.o \
	src/assembler/assembler.o src/assembler/expand_vars.o src/assembler/list_instructions.o src/assembler/binary_content.o src/assembler/aliases.o src/assembler/opcodes.o \
	src/export/binary.o src/export/schematic.o src/export/export_utils.o

TARGET_UNIT=tests/unit
TARGET_FUNC=tests/testsuite.sh
OBJ_UNIT=tests/unit_queue.o tests/unit_hashmap.o tests/unit_lexer.o tests/unit_cli.o tests/unit_parser_utils.o tests/unit_parser.o tests/unit_vars.o

.PHONY: all check debug coverage clean

all: $(TARGET)

$(TARGET): $(OBJ_MAIN) $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(TARGET_UNIT): CFLAGS+=-lcriterion -g
$(TARGET_UNIT): LDLIBS+=-lcriterion
$(TARGET_UNIT): $(OBJ) $(OBJ_UNIT)
	$(CC) -o $(TARGET_UNIT) $^ $(LDFLAGS) $(LDLIBS)

check: LDLIBS+=-lcriterion
check: $(TARGET) $(TARGET_UNIT)
	./$(TARGET_UNIT) -j 1 # run criterion on 1 thread to avoid asan clutter
	exec ./$(TARGET_FUNC)

debug: CFLAGS+=-fsanitize=address -g
debug: LDLIBS+=-fsanitize=address
debug: check

coverage: CFLAGS+=--coverage -fPIC
coverage: LDLIBS+=-lgcov
coverage: LDFLAGS+=--coverage
coverage: debug
	gcovr --html --html-nested --output=report.html
	xdg-open report.html

clean:
	# main clean
	$(RM) $(TARGET) $(OBJ_MAIN) $(OBJ)
	$(RM) $(TARGET_UNIT) $(OBJ_UNIT)

	# coverage clean
	$(RM) *.html *.css
	find . -type f -name *.gcno -exec $(RM) {} +
	find . -type f -name *.gcda -exec $(RM) {} +
