#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "options_commande.h"

int end_CSM (char *c) {
	int len = strlen(c);
	if (c[len-1] == 'm' && c[len-2] == 's' && c[len-3] == 'c' && c[len-4] == '.') return 0;
	return 1;
}


FILE *commande_o (int argc, char  *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 && i < argc-1) {
			char *fichier = malloc(strlen(argv[i+1])*sizeof(char));
			strcpy(fichier, argv[i+1]);
			if (end_CSM(argv[i+1]))
				fichier = strcat(fichier, ".csm");
			if (fopen(fichier,"r") != NULL) {
				char a = 0;
				while (a != 'y' && a != 'n') {
					if (a != '\n')
						printf("%s\n", "Etes-vous sur de vouloir ecraser le fichier ? [y/n]");
					a = getchar();
				}
				if (a == 'n') return 0;
			}
			return fopen(fichier, "w");
		}
	}
	return 0;
}

char commande_s (int argc, char  *argv[]) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0 && i < argc-1) {
			return *argv[i+1];
		}
	}
	return 0;
}

void commande_h (void) {
	printf("Commandes facultatives : \n\n");
	printf("-o [fichier] : Cette option sert a enregistre" 
		"le resultat de la commande dans un fichier annexe\n"
		"Si cette commande n'est pas specifier, la commande enregistrera dans"
		"le premier fichier\n\n");

	printf("-s c : Cette option remplace le caractere separateur\n");
}