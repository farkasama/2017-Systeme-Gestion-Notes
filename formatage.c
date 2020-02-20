#include "options_commande.c"

//compte le nombre de champs maximal du fichier c
int nb_champs (char *c) {
	FILE *f = fopen(c, "r");
	char a = 0;
	char s;
	int nbc = 0;
	int n = 0;
	a = fgetc(f);
	if (a == '#') {
		while (a != '\n' && a != EOF)
			a = fgetc(f);
		s = fgetc(f);
	}
	else s = a;
	a = fgetc(f);
	while (a != EOF) {
		if (a == s) nbc++;
		else if (a == '\n') {
			if (nbc > n) {
				n = nbc;
			}
			nbc = 0;
		}
		a = fgetc(f);
	}
	fclose(f);
	return n;
}

//ajoute tous les caractere et ' ' + le(s) separateur(s) si il(s) en manque(nt )
void formater (char *c, FILE *sortie, int n, char separateur) {
	FILE *f = fopen(c, "r");
	char a = 0;
	char s;
	int nbc = 0;
	a = fgetc(f);

	char t = 0;
	if (sortie == NULL) {
		t = 1;
		sortie = tmpfile();
	}

	while (a == '#') {
		while (a != '\n' && a != EOF)
			a = fgetc(f);
		a = fgetc(f);
	} 
	s = a;
	if (separateur == -1) separateur = s;
	fputc(separateur, sortie);
	fputc(fgetc(f), sortie);
	//a = fgetc(f);
	while (a != EOF) {
		a = fgetc(f);
		if (a == s){ 
			nbc++;
			fputc(separateur, sortie);
		}
		else if (a == '\n') {
			if (nbc < n) {
				for (int i = 0; i < n-nbc; ++i)
				{
					if (i != 0) fputc(' ', sortie);
					fputc(separateur, sortie);
				}
			}
			nbc = 0;
			fputc(a, sortie);
		}
		else if (a != EOF)
			fputc(a, sortie);
	}
	fclose(f);

	if (t) {
		fseek(sortie, 0, SEEK_SET);
		FILE *fin = fopen(c, "w");
		while ((a = fgetc(sortie)) != EOF) {
			fputc(a, fin);
		}
		fclose(fin);
	}
	fclose(sortie);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Erreur\n");
		return 0;
	}
	
	char c;
	FILE *sortie = NULL;
	char separateur = -1;

	//recupere les options
	while ((c = getopt(argc, argv, "osh")) != -1) {
		if (c == 'o') {
			sortie = commande_o(argc, argv);
			if (sortie == 0) {
				printf("Erreur\n");
				return 0;
			}
		}
		else if (c == 's'){
			separateur = commande_s(argc, argv);
		}
		else if(c == 'h') {
			printf("fornatage fichier\n\n"
				"La commande repère la ligne de données avec le plus grand "
				"nombre de champs n et ajoute à la fin de chaque ligne les "
				"champs vides pour obtenir toutes les lignes avec le même "
				"nombre n de champs\n\n");
			commande_h();
		}
	}

	int n = nb_champs(argv[argc-1]);
	formater(argv[argc-1], sortie, n, separateur);
	return 0;
}