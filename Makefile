all: mytar clean

mytar: reconstruction.o extraction.o archivage.o utiles.o mytar.o
	gcc -o mytar reconstruction.o extraction.o archivage.o utiles.o mytar.o

utiles.o: utiles.c extraction.h reconstruction.h archivage.h utiles.h variables.h
	gcc -o utiles.o -c utiles.c -std=c99 -D_BSD_SOURCE -Wall -pedantic -g

extraction.o: extraction.c extraction.h reconstruction.h utiles.h variables.h
	gcc -o extraction.o -c extraction.c -std=c99 -D_BSD_SOURCE -Wall -pedantic -g

archivage.o: archivage.c archivage.h utiles.h variables.h
	gcc -o archivage.o -c archivage.c -std=c99 -D_BSD_SOURCE -Wall -pedantic -g

reconstruction.o: reconstruction.c reconstruction.h utiles.h variables.h
	gcc -o reconstruction.o -c reconstruction.c -std=c99 -D_BSD_SOURCE -Wall -pedantic -g

mytar.o: mytar.c archivage.h extraction.h reconstruction.h utiles.h variables.h
	gcc -o mytar.o -c mytar.c -std=c99 -D_BSD_SOURCE -Wall -pedantic -g

clean:
	rm -f *.o core

cleanall: clean
	rm -f mytar
