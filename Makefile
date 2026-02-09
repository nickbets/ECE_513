
# ---- Config ----
CC       ?= gcc
TARGET   ?= app

SRC      := main.c structs.c parser.c DC_analysis.c iterative.c csparse.c transient.c 
OBJ      := $(SRC:.c=.o)
DEPS     := $(OBJ:.o=.d)

CSTD     ?= c17
CFLAGS   ?= -std=$(CSTD) -Wall -Wextra -Wpedantic -O2 -g -MMD -MP -D_POSIX_C_SOURCE=200809L -fsanitize=address 
LDFLAGS  ?= -fsanitize=address -lgsl -lgslcblas -lm

# ---- Default ----
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# ---- Pattern rules ----
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

# ---- Convenience targets ----
run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -O0 -g3 -fsanitize=address,undefined -fno-omit-frame-pointer
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

format:
	@command -v clang-format >/dev/null 2>&1 && \
		clang-format -i $(SRC) structs.h || \
		echo "clang-format not found; skipping format."

clean:
	$(RM) $(OBJ) $(DEPS) $(TARGET)

.PHONY: all run debug valgrind format clean
