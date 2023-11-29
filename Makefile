stand: stand.c
	$(CC) $(CFLAGS) -O2 -Wall -std=gnu11 $^ -o $@

.PHONY: install
install: stand
	cp stand /usr/local/bin/stand

.PHONY: clean
clean:
	rm stand

.PHONY: release
release: stand
	rm -rf dist/
	mkdir dist
	tar -czf dist/stand-linux-amd64.tar.gz $^