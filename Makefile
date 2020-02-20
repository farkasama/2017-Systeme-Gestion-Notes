all: union select tri moyenne formatage

union: union.c options_commande.h
	gcc -g -Wall union.c -o union.o

select: select.c options_commande.h
	gcc -g -Wall select.c -o select.o

tri: tri.c options_commande.h
	gcc -g -Wall tri.c -o tri.o

moyenne: moyenne.c options_commande.h
	gcc -g -Wall moyenne.c -o moyenne.o

formatage: formatage.c options_commande.h
	gcc -g -Wall formatage.c -o formatage.o

clean:
	rm -rf *.o
	rm -rf *~