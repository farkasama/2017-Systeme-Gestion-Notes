#include "options_commande.c"


//Ecrit la ligne nb du fichier c dans le fichier sortie
void write_in (char *c, FILE *sortie, int nb, char sep) {
	FILE *f = fopen(c, "r");
	char a = fgetc(f);
	char s;
	while (a == '#') {
		while (a != '\n' && a != EOF)
			a = fgetc(f);
		a = fgetc(f);
	}
	s = a;
	fgetc(f);
	int b = 0;
	do {
		a = fgetc(f);
		if (b == nb && a != EOF){
			if (a == s) fputc(sep, sortie);
			else fputc(a, sortie);
		}
		if (a == '\n') b++;
	}while (b != nb+1 && a != EOF);
	fclose(f);
}

//compte le nombre de ligne dans le fichier str
int compte_ligne (char *str) {
	FILE *f = fopen(str, "r");
	int nb = 0;
	char c = fgetc(f);
	if (c == '#') {
		do {
			c = fgetc(f);
		} while(c != '\n' && c != EOF);
		c = fgetc(f);
	}
	fgetc(f);
	fgetc(f);
	while ((c = fgetc(f)) != EOF) {
		if (c == '\n') nb++;
	}
	fclose(f);
	return nb;
}

//ajoute le seprateur dans le fichier sortie
void add_debut (char *entre, FILE *sortie, char sep) {
	FILE *f = fopen(entre, "r");
	char c = fgetc(f);
	while (c == '#') {
		while (c != '\n' && c != EOF) {
			c = fgetc(f);
		}
		c = fgetc(f);
	}
	fputc(sep, sortie);
	fputc('\n', sortie);
	fclose(f);
}


//Tri le tableau t par ordre lexicographique
void tri_string(char *t[], int num[], int n){
	int i, j;
    char *en_cours = malloc(1000);
    for (i = 1; i < n; i++) {
        en_cours = t[i];
        int a = num[i];
        for (j = i; j > 0 && strcmp(en_cours, t[j-1]) < 0; j--) {
            t[j] = t[j - 1];
            num[j] = num[j-1];
        }
        t[j] = en_cours;
        num[j] = a;
    }
}

//tri le tableau t par ordre croissant
void tri_entier (int t[], int num[], int n) {
	int i, j;
    int en_cours;
    for (i = 1; i < n; i++) {
        en_cours = t[i];
        int a = num[i];
        for (j = i; j > 0 && t[j-1] > en_cours; j--) {
            t[j] = t[j - 1];
            num[j] = num[j-1];
        }
        t[j] = en_cours;
        num[j] = a;
    }
}


int caractere(char *entre, FILE *sortie, char *argv[], int num[], int n, char sep) {
	//supprime les espaces au debut et a la fin
	for (int i = 0; i < n; i++) {
		sscanf(argv[i], "%s", argv[i]);
	}

	//appelle la fonction pour trier
	tri_string(argv, num, n);

	//si le fichier de sortie est le fichier d'entre
	char tmp = 0;
	if (sortie == NULL) {
		sortie = tmpfile();
		tmp = 1;
	}

	add_debut(entre, sortie, sep);

	//ajoute les lignes num[i] au fichier de sortie
	for (int i = 0; i < n; i++) {
		write_in(entre, sortie, num[i], sep);
	}

	//si le fichier de sortie est le fichier d'entre on reecrit
	if (tmp) {
		char c;
		fseek(sortie, 0, SEEK_SET);
		FILE *fin = fopen(entre, "w");
		while ((c = fgetc(sortie)) != EOF) {
			fputc(c, fin);
		}
		fclose(fin);
	}

	fclose(sortie);
	return 0;
}


int entier(char *entre, FILE *sortie, char *argv[], int num[], int n, char sep) {
	int tab[n];
	int taille = n;
	//verifie si la note est abs et si oui le met a la fi n du tableau sinon ajoute l'entier au tableau
	for (int i = 0; i < taille; i++) {
		sscanf(argv[i], "%s", argv[i]);
		if (strcmp(argv[i], "abs") == 0) {
			taille--;
			tab[taille] = 0;
			int a = num[taille];
			num[taille] = num[i];
			num[i] = a;
			char *b = malloc(strlen(argv[taille])*sizeof(char));
			strcpy(b, argv[taille]);
			strcpy(argv[taille], argv[i]);
			strcpy(argv[i], b);
			i--;
		}
		else {
			tab[i] = atoi(argv[i]);
		}
	}

	//si le fichier d'entre est le fichier de sortie
	char tmp = 0;
	if (sortie == NULL) {
		sortie = tmpfile();
		tmp = 1;
	}

	add_debut(entre, sortie, sep);

	tri_entier(tab, num, taille);

	//ecrit les ligne num[i] dqns le fichier sortie
	for (int i = 0; i < n; i++) {
		write_in(entre, sortie, num[i], sep);
	}

	//si le fichier d'entre est le fichier de sortie
	if (tmp) {
		char c;
		fseek(sortie, 0, SEEK_SET);
		FILE *fin = fopen(entre, "w");
		while ((c = fgetc(sortie)) != EOF) {
			fputc(c, fin);
		}
		fclose(fin);
	}

	fclose(sortie);
	return 0;
}


int commande (int argc, char* argv[]) {
	FILE *sortie = NULL;
	char separateur = -1;
	char c;
	char num = 0;
	int champs = -1;
	char ent = 0;
	char *entre = malloc(1000*sizeof(int));

	//recupere les options
	extern char *optarg;
	while ((c = getopt(argc, argv, "oshn1::2::3::4::5::6::7::8::9::0::")) != -1) {
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
		else if (c == 'n') {
			num = 1;
		}
		else if (c >= '0' && c <= '9') {
			//si le champs passe en argument est superieur a 9
			if (optarg) {
				char *str = malloc(strlen(optarg) +2);
				*str = c;
				*(str+1) = '\0';
				strcat(str, optarg);
				champs = atoi(str);
			}
			else champs = c - '0';
		}
		else if (c == 'h') {
			printf("tri -c [-n] [fichier] \n\n");
			printf("Effectue le tri du fichier par ligne en fonction du champs c\n"
				"L'option -c (ou c est un entier) est obligatoire."
				"-n est facultatif. Elle permet de faire le tri numerique. Sinon le tri"
				" lexicographique s'applique \n\n");
			commande_h();
			return 0;
		}
	}

	//verifie sur le fichier d'entre est donne
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

	int nb_ligne = compte_ligne(entre);
	char *mot[nb_ligne];
	int num_mot[nb_ligne];

	//recupere le seprateur
	FILE *f = fopen(entre, "r");
	c = fgetc(f);
	while (c == '#') {
		do {
			c = fgetc(f);
		} while(c != '\n' && c != EOF);
		c = fgetc(f);
	}
	char sep = c;
	if (separateur == -1) {
		separateur = sep;
	}
	fgetc(f);
	c = fgetc(f);
	//recupere les champs de chaque ligne avec optimisation de l'espace
	int i = 0;
	while(c != EOF) {
		int nombre = 1;
		while (c != EOF && nombre != champs){
			if (c == sep) nombre++;
			c = fgetc(f);
		}
		int a = 1;
		while (c != EOF && c != sep && c != '\n'){
			a++;
			c = fgetc(f);
		}
		fseek(f, a*(-1), SEEK_CUR);
		mot[i] = malloc((a+1)*sizeof(char));
		fgets(mot[i], a, f);
		i++;
		c = fgetc(f);
		while (c != EOF && c != '\n'){
			c = fgetc(f);
		}
		if (c == '\n') c = fgetc(f);
	}
	fclose(f);

	//initialise le tableau du numero de la ligne des champs
	for (int i = 0; i < nb_ligne; i++) {
		num_mot[i] = i;
	}

	if (num == 1) return entier(entre, sortie, mot, num_mot, nb_ligne, separateur);
	return caractere(entre, sortie, mot, num_mot, nb_ligne, separateur);
}


int main(int argc, char *argv[])
{
	if (commande(argc, argv))
		printf("Une erreur a du se produire\n");
	return 0;
}