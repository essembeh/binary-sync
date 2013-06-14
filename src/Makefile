all: bs-info bs-checksum


bs-checksum: bs_checksum.o block.o adler32.o bsheader.o
	gcc bs_checksum.o block.o adler32.o bsheader.o -o bs-checksum

bs-info: bs_info.o block.o adler32.o bsheader.o
	gcc bs_info.o block.o adler32.o bsheader.o -o bs-info


common.o: common.c common.h
	gcc -c common.c
	
adler32.o: adler32.c zutil.h zutil.h
	gcc -c adler32.c

bsheader.o: bsheader.c bsheader.h common.h
	gcc -c bsheader.c
	
block.o: block.c bsheader.h block.h adler32.h
	gcc -c block.c


bs_checksum.o: bs_checksum.c common.h bsheader.h adler32.h block.h
	gcc -c bs_checksum.c

bs_info.o: bs_info.c common.h bsheader.h
	gcc -c bs_info.c

bs_request.o: bs_request.c common.h bsheader.h adler32.h block.h
	gcc -c bs_request.c



clean:
	rm -f *.o