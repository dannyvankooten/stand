CFLAGS_COMMON =  -O2 -Wall -Wextra -Wpedantic -Wconversion -Wundef -Wformat=2 -std=c11 -D_GNU_SOURCE
CFLAGS_DEBUG += -fsanitize=address,undefined -fstack-protector-strong
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG)

stand: stand.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: install
install: stand
	cp stand /usr/local/bin/stand

.PHONY: clean
clean:
	rm stand

.PHONY: release
release: CFLAGS=$(CFLAGS_COMMON)
release: stand
	rm -rf dist/
	mkdir dist
	tar -czf dist/stand-linux-amd64.tar.gz $^
