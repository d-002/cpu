CFLAGS=-Wall -Wextra -Werror -Wvla -std=c99 -pedantic
CPPFLAGS=-Isrc

TARGET=assembli
OBJ_MAIN=src/main.o
OBJ=src/cli.o src/logger.o src/queue.o src/files.o src/lexer.o src/parser.o

TARGET_UNIT=tests/unit
TARGET_FUNC=tests/testsuite.sh
OBJ_UNIT=tests/unit.o

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
