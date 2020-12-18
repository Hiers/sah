CC = cc 

DMACROS = -D_GNU_SOURCE -DROOTDIR=\"/\" -DDBPATH=\"/var/lib/pacman/\" -DAUR_BASE_URL=\"https://aur.archlinux.org\" -DCONFFILE=\"/etc/pacman.conf\"
LIBRARIES = -lalpm -lcurl -lyajl -lgit2

sah: sah.c aur.c util.c alpm-query.c color.c
	$(CC) *.c -Wall ${DMACROS} ${LIBRARIES} -o sah

install: sah
	mkdir -p $(DESTDIR)/usr/bin
	cp sah $(DESTDIR)/usr/bin
	strip $(DESTDIR)/usr/bin/sah

uninstall:
	rm $(DESTDIR)/usr/bin/sah

.PHONY: install uninstall
