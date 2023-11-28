stand: stand.c
	$(CC) -O3 $^ -o $@

.PHONY: install
install: stand
	cp stand /usr/local/bin/stand

.PHONY: clean
clean:
	rm stand

