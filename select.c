#include "options_commande.c"

//Compte le nombre de char c dans le string str
int nb_occurrence (char *str, char c) {
	int n = strlen(str);
	int nb = 0;
	for (int i = 0; i < n; ++i)
	{
		if (str[i] == c)
		{
			nb++;
		}
	}
	return nb;
}

//Compte le nombre de champs maximal dans le fichier c
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

//Ecrit les champs n1,...,nk dans le fichier sortie
void write_in (char *c, FILE *sortie, int *nb, char sep, int taille) {
	FILE *f = fopen(c, "r");
	char a = fgetc(f);
	char *s = malloc(sizeof(char));
	while (a == '#') {
		while (a != '\n' && a != EOF){
			a = fgetc(f);
		}
		a = fgetc(f);
	}
	*s = a;
	fgetc(f);
	a = fgetc(f);
	int nb_c = nb_champs(c);
	while(a != EOF) {
		char *mot[nb_c];
		int n = 1;
		while (a != '\n' && a != EOF) {
			n++;
			a = fgetc(f);
		}
		char *m = malloc(n);
		fseek(f, (-1)*n, SEEK_CUR);
		fgets(m, n, f);
		int t = 0;
		char *token = strtok(m, s);
		while (token != NULL) {
			mot[t] = malloc(sizeof(char)*strlen(token));
			sscanf(token, "%s", mot[t]);
			t++;
			token = strtok(NULL, s);
		}
		for (int i = 0; i < taille; i++) {
			fputs(mot[nb[i]], sortie);
			if (i != taille-1) {
				fputc(*s, sortie);
				fputc(' ', sortie);
			}
			else fputc('\n', sortie);
		}
		a = fgetc(f);
		if (a == '\n') a = fgetc(f);
	}
	fclose(f);
}

//Recuperer les champs aui doivent etre ecrit
int *get_tableau(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-l") == 0 && i < argc-1) {
			int *entier = malloc ((nb_occurrence(argv[i+1], ',')+1)*sizeof(int));
			char *c = malloc(strlen(argv[i+1])*sizeof(char));
			strcpy(c, argv[i+1]);
			char *token = strtok(c, ",");
			int a = 0;
			while (token != NULL) {
				sscanf(token, "%d", &entier[a]);
				a++;
				token = strtok(NULL, ",");
			}
			return entier;
		}
	}
	return NULL;
}

int commande (int argc, char *argv[]) {

	FILE *sortie = NULL;
	char *entre = malloc(1000*sizeof(char));
	char separateur = -1;
	char c;
	int *tab = NULL;
	int taille;
	char ent = 0;

	//recuperer les options de la commande
	while ((c = getopt(argc, argv, "oslh")) != -1) {
		if (c == 'o') {
			if (optind == argc-1) 
				ent = 1;
			sortie = commande_o(argc, argv);
			if (sortie == 0) {
				return 0;
			}
		}
		else if (c == 's'){
			separateur = commande_s(argc, argv);
		}
		else if (c == 'l') {
			tab = get_tableau(argc, argv);
			taille = nb_occurrence(argv[optind], ',') + 1;
		}
		else if (c == 'h') {
			printf("\nselect -l n1,...,nk [fichier]\n\n");
			printf("Cette commande produit le fichier aui contient chaque ligne"
				"les champs n1,...,nk\n\n");
			printf("L'option -l est obligatoire.\n\n");
			commande_h();
			return 1;
		}
	}

	//verifie si l'option l a bien ete passer
	if(tab == NULL){
		printf("Vous avez oublie la commande -l\n");
		return 0;
	}

	//verifier si le fichier a bien ete entrer
	if (ent || (!ent && end_CSM(argv[argc-1]))) {
		printf("Veuillez donner un fichier d'entrer :\n");
		scanf("%s", entre);
		while (end_CSM(entre)) {
			printf("Le fichier doit avoir l'extension .CSM\n");
			scanf("%s", entre);
		}
	}
	else {
		entre = argv[argc-1];
	}
	
	//si le fichier sortie est le fichier d'entrer
	char t = 0;
	if (sortie == NULL) {
		t = 1;
		sortie = tmpfile();
	}

	//recupere le separateur du fichier d'origine
	FILE *f = fopen(entre, "r");
	c = fgetc(f);
	while (c == '#') {
		do {
			c = fgetc(f);
		} while(c != '\n' && c != EOF);
		c = fgetc(f);
	}
	if (separateur == -1) separateur = c;
	fputc(separateur, sortie);
	fputc(fgetc(f), sortie);
	fclose(f);

	//ecrit les champs
	write_in(entre, sortie, tab, separateur, taille);

	//si le fichier de sortie est le fichier d'origine, on reecrit
	if (t) {
		fseek(sortie, 0, SEEK_SET);
		FILE *fin = fopen(entre, "w");
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
		printf("Une erreur s'est produit\n");
	return 0;
}