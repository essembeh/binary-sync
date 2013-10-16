CC=gcc
CFLAGS=-W -Wall
LDFLAGS=-lz

all: binaries

binaries: bs-info bs-checksum bs-apply bs-data

bs-info: bsheader.o common.o bs_info.o
	$(CC) -o $@ $^ $(LDFLAGS)

bs-checksum: bsheader.o common.o bs_checksum.o
	$(CC) -o $@ $^ $(LDFLAGS)

bs-data: bsheader.o common.o bs_data.o
	$(CC) -o $@ $^ $(LDFLAGS)

bs-apply: bsheader.o common.o bs_apply.o
	$(CC) -o $@ $^ $(LDFLAGS)

bsheader.o: src/bsheader.c src/bsheader.h src/error.h
	${CC} -c $< ${CFLAGS}

common.o: src/common.c src/common.h src/error.h
	${CC} -c $< ${CFLAGS}

bs_info.o: src/bs_info.c src/error.h
	${CC} -c $< ${CFLAGS}

bs_checksum.o: src/bs_checksum.c src/error.h
	${CC} -c $< ${CFLAGS}

bs_data.o: src/bs_data.c src/error.h
	${CC} -c $< ${CFLAGS}

bs_apply.o: src/bs_apply.c src/error.h
	${CC} -c $< ${CFLAGS}

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm -rf bs-info bs-checksum bs-apply bs-data

archive: binaries
	rm -rf binary-sync
	mkdir binary-sync
	cp bs-info bs-checksum bs-apply bs-data binary-sync
	tar fac binary-sync.tar.gz binary-sync
