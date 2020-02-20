#include "options_commande.c"

//Verifier si le nombre de champs est correct (verifie uniquement sur la premiere ligne)
int champs_correct (char *c, int nb) {
	FILE *f = fopen(c, "r");
	char a = 0;
	char s;
	int nbc = 0;
	a = fgetc(f);
	while (a == '#') {
		while (a != '\n' && a != EOF)
			a = fgetc(f);
		a = fgetc(f);
	}
	s = a;
	fgetc(f);
	while ((a = fgetc(f)) != '\n' && a != EOF) {
		if (a == s) nbc++;
	}
	fclose(f);
	if (nb == nbc) return 0;
	return 1;
}

//Copie le fichier c dans le fichier sortie
void copie (char *c, FILE *sortie, char sep) {
	FILE *f = fopen(c, "r");
	char a = 0;
	char s;
	a = fgetc(f);
	while (a == '#') {
		while (a != '\n' && a != EOF)
			a = fgetc(f);
		a = fgetc(f);
	}
	s = a;
	fgetc(f);
	while ((a = fgetc(f)) != EOF) {
		if (a == s) fputc(sep, sortie);
		else fputc(a, sortie);
	}
	fclose(f);
}

int commande (int argc, char *argv[]) {
	FILE *sortie = NULL;
	char separateur = -1;
	char c;
	int a = 1;
	int nb_champs = 0;

	//recuperer les options de la commande
	while ((c = getopt(argc, argv, "osh")) != -1) {
		if (c == 'o') {
			sortie = commande_o(argc, argv);
			if (sortie == 0) {
				return 0;
			}
			a += 2;
		}
		else if (c == 's'){
			separateur = commande_s(argc, argv);
			a += 2;
		}
		else if (c == 'h') {
			printf("\nunion fichier fichier [fichier...]\n\n");
			printf("Cette commande permet de concatener plusieurs fichiers (au moins deux).\n\n");
			commande_h();
			return 1;
		}
	}

	if (argc < 3) return 0;
	// caracteristique du premier fichier, compte aussi le nombre de champs
	FILE *f = fopen(argv[a], "r");
	c = fgetc(f);
	while (c == '#') {
		while (c != '\n' && a != EOF)
			c = fgetc(f);
		c = fgetc(f);
	}
	char sep = c;
	if (separateur == -1) {
		separateur = sep;
	}
	fgetc(f);
	do {
		c = fgetc(f);
		if (c == separateur || c == sep) nb_champs++;
	} while (c != '\n' && c != EOF);
	fclose(f);

	//ecriture du premier fichier dans le fichier sortie
	char t = 0;
	if (sortie == NULL) {
		t = 1;
		sortie = tmpfile();
	}
	FILE *f_tmp = fopen(argv[a], "r");
	char ab = fgetc(f_tmp);
	while (ab == '#') {
		do {
			ab = fgetc(f_tmp);
		} while(ab != '\n');
		ab = fgetc(f_tmp);
	}
	char sepa_tmp = ab;
	fputc(separateur, sortie);
	do {
		c = fgetc(f_tmp);
		if (c == sepa_tmp) fputc(separateur, sortie);
		else if (c != EOF) fputc(c, sortie);
	} while (c != EOF);
	fclose(f_tmp);
	a++;

	//copie les autres fichiers
	for (int i = a; i < argc; i++) {
		if (champs_correct (argv[i], nb_champs)){
			printf("Le nombre de champs n'est pas le meme.\n");
			return 0;
		}
		copie(argv[i], sortie, separateur);
	}

	//si le fichier d'entre est le premier fichier alors on copie tout
	if (t) {
		fseek(sortie, 0, SEEK_SET);
		FILE *fin = fopen(argv[a-1], "w");
		while ((c = fgetc(sortie)) != EOF) {
			fputc(c, fin);
		}
		fclose(fin);
	}
	fclose(sortie);
	return 1;
}

int main(int argc, char *argv[])
{
	if (commande(argc, argv) == 0)
		printf("%s\n", "Une erreur a du se produire.");
	return 0;
}