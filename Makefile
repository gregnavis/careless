CC?=gcc
OUTPUT=libcareless.so

BIN_PATH?=/usr/bin
LIB_PATH?=/usr/lib

.PHONY: clean install
.IGNORE: clean

all: $(OUTPUT)

$(OUTPUT): libcareless.c
	$(CC) -shared -fPIC $^ -ldl -o $@

install: all
	install -o root -g root -m 0755 $(OUTPUT) $(LIB_PATH)
	install -o root -g root -m 0755 netredirect $(BIN_PATH)

clean:
	rm $(OUTPUT)
