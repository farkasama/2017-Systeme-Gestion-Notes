#include "options_commande.c"
#include <ctype.h>

//compte le nombre de caractere c dans le string str
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

//recupere les coefficient passer en argument
void get_coeff (char *c, float tab[], int champs[]) {
	int i = 0;
	char *a = malloc(strlen(c));
	strcpy(a, c);
	char *token = strtok_r(a, "+", &a);
	while (token != NULL) {
		char *b = malloc(strlen(token));
		strcpy(b, token);
		char *token1 = strtok_r(b, "*", &b);
		sscanf(token1, "%d", &champs[i]);
		token1 = strtok_r(NULL, "*", &b);
		sscanf(token1, "%f", &tab[i]);
		i++;
		token = strtok_r(NULL, "+", &a);
	}
}

//tri les cases passe en argument
void tri (int t[], float num[], int n) {
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

//retourne la sormme des coefficient
int somme (float t[], int n) {
	int a = 0;
	for (int i = 0; i < n; ++i)
	{
		a += t[i];
	}
	return a;
}

//verifie si le string ne contient pas que des espaces
int only_space(char *c) {
	for (int i = 0; i < strlen(c); ++i)
	{
		if(!isspace(c[i]))
			return 0;
	}
	return 1;
}

void f_moyenne(FILE *entre, FILE *sortie, char sep_ori, char sep_rem, int t1[], float t2[], int n) {
	int denominateur = somme(t2, n);
	char c = fgetc(entre);
	while (c != EOF) {
		int curseur = 1;
		int i = 0;
		float somme = 0;
		while (c != '\n') {
			//verifie si le champs appartient au valeurs passe en argument
			if (curseur == t1[i]) {
				int a = 1;
				c = fgetc(entre);
				//recupere la valeur du champs de maniere optimise en espace
				while (c != sep_ori && c != EOF && c != '\n') {
					a++;
					c= fgetc(entre);
				}
				fseek(entre, a*(-1), SEEK_CUR);
				char *mot = malloc(a+1);
				fgets(mot, a, entre);

				if (only_space(mot)) {
					somme += 0;
				}
				else if (strstr(mot, "abs") != NULL) {
					denominateur -= t2[i];
				}
				else {
					float b;
					sscanf(mot, " %f", &b);
					somme += b*t2[i];
				}
				fputs(mot, sortie);
				fputc(sep_rem, sortie);
				i++;
				curseur++;
				fgetc(entre);
			}
			//sinon passe le champs
			else {
				c = fgetc(entre);
				if (c == sep_ori){
					fputc(sep_rem, sortie); 
					curseur++;
				}
				else if (c != '\n') 
					fputc(c, sortie);
			}
		}
		//ajoute la moyenne
		fputc(sep_rem, sortie);
		fputc(' ', sortie);
		if (denominateur == 0)
			fputs("abs", sortie);
		else {
			char str[12];
			sprintf(str, "%f", somme/denominateur);
			fputs(str, sortie);
		}
		fputc(c, sortie);
		if (c == '\n') c = fgetc(entre);
	}
}

int commande (int argc, char *argv[]){

	FILE *sortie = NULL;
	char *entre = malloc(1000*sizeof(char));
	char separateur = -1;
	char c;
	char *moyenne;
	char m = 0;
	char ent = 0;

	//recupere les options
	while ((c = getopt(argc, argv, "osmh")) != -1) {
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
		else if (c == 'm') {
			m = 1;
			moyenne = argv[optind];
		}
		else if (c == 'h') {
			printf("moyenne -m formule [fichier]\n\n"
				"Cette commande ajoute a la fin de chaque ligne un nouveau champs"
				" numerique qui a pour valeur la moyenne ponderee selon la formule donnee.\n\n"
				"L'option m est obligatoire ainsi que la formule. \n\n");
			commande_h();
		}
	}

	//verifie si l'option m a bien ete passe
	if (!m) return 0;

	//si le fichier d'entre n'est pas passer en argument
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

	int n = nb_occurrence(moyenne, '+') + 1;
	float coeff[n];
	int champs[n];
	get_coeff(moyenne, coeff, champs);
	tri(champs, coeff, n);

	//si le fichier de sortie est le fichier de sortie
	char t = 0;
	if (sortie == NULL) {
		t= 1;
		sortie = tmpfile();
	}

	//recupere le separateur
	FILE *f = fopen(entre, "r");
	c = fgetc(f);
	while (c == '#') {
		do {
			c = fgetc(f);
		} while(c != '\n' && c != EOF);
		c = fgetc(f);
	}
	char sep = c;
	if (separateur == -1) separateur = sep;
	fgetc(f);
	fputc(separateur, sortie);
	fputc('\n', sortie);

	f_moyenne(f, sortie, sep, separateur, champs, coeff, n);

	fclose(f);

	//reecrit si le fichier d'entre est le fichier de sortie
	if (t) {
		printf("oui\n");
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
	if (commande(argc, argv))
		printf("Une erreur s'est produit\n");
	return 0;
}