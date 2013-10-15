CC?=gcc
OUTPUT=libcareless.so

BIN_PATH=/usr/local/bin
LIB_PATH=/usr/local/lib

.PHONY: clean install
.IGNORE: clean

all: $(OUTPUT)

$(OUTPUT): libcareless.c
	$(CC) -shared -fPIC $^ -ldl -o $@

install: all
	install -o root -g root -m 0755 $(OUTPUT) $(LIB_PATH)
	install -o root -g root -m 0755 careless $(BIN_PATH)

clean:
	rm $(OUTPUT)
