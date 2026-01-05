#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "gererMem.c"
#include "xxhash.c" //hash = XXH3_64bits(donnée, taille_donnée);


typedef struct occMot{
    char * mot;
    int occurrence;
} OccMot;

//Liste d'OccMot
typedef struct liste{
    OccMot * occmot;
    int tailleLst; //Espace occupé
    int tailleMax; //Espace alloué
} Liste;

//Table de hachage
typedef struct table_h{
    int capacite; //Taille de la table
    int occupation; //Nombre de cases occupées
    OccMot ** occmot; //Liste de pointeurs
} Table_h;

/////////Fonctions d'affichage ////////////

void afficheLst(Liste lst, int n){
    //n correspond au nombre de mots a afficher, si n = -1 on affcihe tout
    for(int i = 0; i < lst.tailleLst && (n == -1 || i < n); i++){
        printf("%s : %d apparition(s)\n", lst.occmot[i].mot, lst.occmot[i].occurrence);
    }
}

void afficheTableHach(Table_h table){
    for(int i = 0; i < table.capacite; i++){
        if(table.occmot[i] != NULL){
            printf("%s : %d apparition(s)\n", table.occmot[i]->mot, table.occmot[i]->occurrence);
        }
    }
}

void afficheMemoire(InfoMem memoire){
    printf("\nUtilisation de la memoire :\nTotal allocations : %ld\nTotal desallocation : %ld\nPic d'allocation : %ld\n", (unsigned long)memoire.cumul_alloc, (unsigned long)memoire.cumul_desalloc, (unsigned long)memoire.max_alloc);
}


//////////////////// Ecriture des resultats dans un fichier ////////////////////

void ecrireOcc(char* argv[], Liste lst){
    for(int i = 0; argv[i]; i++){
        if(argv[i][0] == '-' && argv[i][1] == 's'){
            FILE* fichier = fopen(argv[i+1], "w");
            for(int i = 0; i < lst.tailleLst; i++){
                fprintf(fichier, "%s %d \n", lst.occmot[i].mot, lst.occmot[i].occurrence);
            }
            fclose(fichier);
        }
    }
}

void ecrirePerf(char* argv[], InfoMem memoire, unsigned long duree_milli, int nbr_mots, int nbr_diff){
    FILE* fichier = NULL;
    for(int i = 0; argv[i]; i++){
        if(argv[i][0] == '-' && argv[i][1] == 'l'){
            char nom[100];
            strcpy(nom, argv[i+1]);
            strcat(nom, ".txt");
            fichier = fopen(nom, "a");
        }
    }
    if(fichier == NULL)
        fichier = fopen("performances.txt", "w"); //fichier par défaut si aucun nom n'est spécifié

    fprintf(fichier, "\n%d %d\n", nbr_mots, nbr_diff);
    fprintf(fichier, "Allocations : %ld\n", (unsigned long)memoire.cumul_alloc);
    fprintf(fichier, "Desallocation : %ld\n", (unsigned long)memoire.cumul_desalloc);
    fprintf(fichier, "Pic : %ld\n",  (unsigned long)memoire.max_alloc);
    fprintf(fichier, "Temps : %ld millisecondes\n", duree_milli);
    fclose(fichier);
}


//////////////////// Liste classiques ////////////////////

Liste * initLst(int taille, InfoMem * memoire){
    Liste *lst = myMalloc(sizeof(Liste), memoire);
    if (!lst) 
        return NULL;

    lst->occmot = myMalloc(sizeof(OccMot)*taille, memoire); //Initialise avec une capacité de taille mots
    if(lst->occmot == NULL){
        myFree(lst, memoire, sizeof(Liste));
        return NULL; //erreur
    }
    lst->tailleLst = 0;
    lst->tailleMax = taille;
    return lst;
}


void freeListe(Liste *lst, InfoMem * memoire){
    for (int i = 0; i < lst->tailleLst; i++){
        myFree(lst->occmot[i].mot, memoire, strlen(lst->occmot[i].mot)+1);//+1 pour le '\0' alloué aussi
    }
    myFree(lst->occmot, memoire, sizeof(OccMot)*lst->tailleMax);
    myFree(lst, memoire, sizeof(Liste));
}

//Ajoute un mot au tableau ou augmente son nbr d'occurrences si déjà présent
void ajoutMotLst(Liste * lst, char * mot, InfoMem * memoire){
    for(int i = 0; i< lst->tailleLst ; i++){
        if(strcmp((lst->occmot)[i].mot, mot)==0){
            (lst->occmot)[i].occurrence ++;
            return;
        }
    }

    if (lst->tailleLst == lst->tailleMax){
        lst->occmot = myRealloc(lst->occmot, sizeof(OccMot)*lst->tailleMax * 2, memoire, sizeof(OccMot)* lst->tailleMax); //Agrandis la taille en doublant
        lst->tailleMax = lst->tailleMax * 2;
    }
    
    lst->occmot[lst->tailleLst].mot = myStrdup(mot, memoire);
    lst->occmot[lst->tailleLst].occurrence = 1;
    lst->tailleLst ++;

    return;
}

void fusionLst(Liste * lst, Liste *a, Liste *b, InfoMem * memoire){
    int pt_a = 0, pt_b = 0;

    for(int i = 0; i < lst->tailleLst; i++){
        myFree(lst->occmot[i].mot, memoire, strlen(lst->occmot[i].mot) + 1);
        if(pt_a == a->tailleLst){
            lst->occmot[i].mot = myStrdup(b->occmot[pt_b].mot, memoire);
            lst->occmot[i].occurrence = b->occmot[pt_b].occurrence;
            pt_b ++;
        }else if(pt_b == b->tailleLst){
            lst->occmot[i].mot = myStrdup(a->occmot[pt_a].mot, memoire);
            lst->occmot[i].occurrence = a->occmot[pt_a].occurrence;
            pt_a ++;
        }else if(a->occmot[pt_a].occurrence > b->occmot[pt_b].occurrence){
            lst->occmot[i].mot = myStrdup(a->occmot[pt_a].mot, memoire);
            lst->occmot[i].occurrence = a->occmot[pt_a].occurrence;
            pt_a ++;
        }else{
            lst->occmot[i].mot = myStrdup(b->occmot[pt_b].mot, memoire);
            lst->occmot[i].occurrence = b->occmot[pt_b].occurrence;
            pt_b ++;
        }
    }
}

//Tri une liste en fonction du nombre d'occurrence de chaque mot (pour la visualisation finale)
void triFusionOccurrence(Liste * lst, InfoMem * memoire){
    if(lst->tailleLst == 0 || lst->tailleLst == 1){
        return;
    }

    //Sépare en 2
    int taille_a = lst->tailleLst / 2;
    Liste *a = initLst(taille_a, memoire);
    for (int i = 0; i < taille_a; i++) {
        a->occmot[i].mot = myStrdup(lst->occmot[i].mot, memoire);
        a->occmot[i].occurrence = lst->occmot[i].occurrence;
    }
    a->tailleLst = taille_a;

    int taille_b = lst->tailleLst - lst->tailleLst / 2;
    Liste * b = initLst(taille_b, memoire);
    for (int i = 0; i < taille_b; i++) {
        b->occmot[i].mot = myStrdup(lst->occmot[taille_a + i].mot, memoire);
        b->occmot[i].occurrence = lst->occmot[taille_a + i].occurrence;
    }
    b->tailleLst = b->tailleMax;

    triFusionOccurrence(a, memoire);
    triFusionOccurrence(b, memoire);

    fusionLst(lst,a,b, memoire); //Détruit a et b et remplit lst de façon triée
    freeListe(a, memoire);
    freeListe(b, memoire);
}

/////////// Liste triée par ordre alphabétique ///////////

void ajoutMotTriee(Liste *lst, char *mot, InfoMem *memoire){
    // Recherche dichotomique
    int gauche = 0, droite = lst->tailleLst - 1;
    int milieu;
    while(gauche <= droite){
        milieu = (gauche + droite) / 2;
        int cmp = strcmp(lst->occmot[milieu].mot, mot);
        if(cmp == 0){
            lst->occmot[milieu].occurrence ++;
            return;
        }
        else if(cmp < 0){
            gauche = milieu + 1;
        }
        else{
            droite = milieu - 1;
        }
    }
    // Insertion du mot à gauche
    if(lst->tailleLst == lst->tailleMax){
        lst->occmot = myRealloc(lst->occmot, sizeof(OccMot)*lst->tailleMax * 2, memoire, sizeof(OccMot)* lst->tailleLst); //Agrandit la taille en doublant
        lst->tailleMax = lst->tailleMax * 2;
    }
    for(int i = lst->tailleLst; i > gauche; i--){
        lst->occmot[i] = lst->occmot[i-1];
    }
    lst->occmot[gauche].mot = myStrdup(mot, memoire);
    lst->occmot[gauche].occurrence = 1;
    lst->tailleLst ++;
    return;
}

//////////////////// Table de hachage ////////////////////

OccMot * initOccMot(char * mot, InfoMem * memoire){
    OccMot* occMot = myMalloc(sizeof(OccMot), memoire);
    if (!occMot){
        return NULL;
    }
    occMot->mot = myStrdup(mot, memoire);
    if(!occMot->mot){
        myFree(occMot, memoire, sizeof(occMot));
    }
    occMot->occurrence = 1;
    return occMot;
}

//Table de hachage interne
Table_h * initTableHach(int taille, InfoMem * memoire){
    Table_h * table = myMalloc(sizeof(Table_h), memoire);
     if (!table) 
        return NULL;
    table->occmot = myMalloc(sizeof(void*)*taille, memoire);//Initialise avec une capacité de taille pointeurs
    if(table->occmot == NULL){
        myFree(table, memoire, sizeof(Table_h));
        return NULL; //erreur
    }
    for(int i = 0; i < taille; i++){
        table->occmot[i] = NULL;
    }
    table->occupation = 0;
    table->capacite = taille;
    return table;
}

void freeTableHash(Table_h * table, InfoMem * memoire){
    for(int i = 0; i < table->capacite; i++){
        if(table->occmot[i] != NULL){
            myFree((table->occmot[i])->mot, memoire, strlen((table->occmot[i])->mot) + 1); // +1 pour le '\0'
            myFree(table->occmot[i], memoire, sizeof(OccMot));
        }
    }
    myFree(table->occmot, memoire, sizeof(OccMot *)*table->capacite);
    myFree(table, memoire, sizeof(Table_h));
}

void doubleCapaHach(Table_h * table, InfoMem * memoire){
    int nouv_taille = table->capacite * 2;
    OccMot ** nouv_lst = myMalloc(nouv_taille * sizeof(OccMot*), memoire);//Nouvelle liste avec 2 fois plus de place pour les pointeurs
    if(!nouv_lst){
        printf("erreur allocation");
        return;
    }
    for(int i = 0; i < nouv_taille; i++){
        nouv_lst[i] = NULL;
    }
    
    for(int i = 0; i < table->capacite; i++){
        if(table->occmot[i] != NULL){
            int nouv_hach = XXH3_64bits(table->occmot[i]->mot, strlen(table->occmot[i]->mot)) % nouv_taille;
            for(; nouv_lst[nouv_hach] != NULL; nouv_hach = (nouv_hach+1) % nouv_taille){
            }
            nouv_lst[nouv_hach] = table->occmot[i]; //Copie le pointeur vers l'élément voulu
        }
    }
    myFree(table->occmot, memoire, sizeof(OccMot*)*table->capacite);
    table->occmot = nouv_lst;
    table->capacite = nouv_taille;
    return;
}

void ajoutTableHach(Table_h * table, char * mot, InfoMem * memoire){
    if ((float)table->occupation/table->capacite >= 0.75){
        doubleCapaHach(table, memoire);
    }
    int hach = XXH3_64bits(mot, strlen(mot)) % table->capacite;
    //Il n'y a pas de suppression donc la recherche est simple
    for(; table->occmot[hach] != NULL; hach = (hach+1) % table->capacite){
        if( strcmp(table->occmot[hach]->mot, mot) == 0){
            table->occmot[hach]->occurrence += 1;
            return;
        }
    }
    //Si le mot n'a jamais été vu on l'ajoute à la prochaine place vide
    OccMot * occMot = initOccMot(mot, memoire);
    table->occmot[hach] = occMot;
    table->occupation += 1;
    return; 
}

Liste * tableHach_to_lst(Table_h * table, InfoMem * memoire){
    Liste * lst = initLst(table->occupation, memoire);
    int i = 0;
    for(int j = 0; j < table->capacite; j++){
        if(table->occmot[j] != NULL){
            lst->occmot[i].mot = myStrdup(table->occmot[j]->mot, memoire);
            lst->occmot[i].occurrence = table->occmot[j]->occurrence;
            i++;
        }
    }
    lst->tailleMax = table->occupation;
    lst->tailleLst = table->occupation;
    return lst;
}



//////////////////// Fonctions principales ////////////////////


int listeSimple(char *argv[], Liste * lst, InfoMem * memoire, int k, int * nbr_mots){

    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        char texte[100] ="textes/";
        strcat(texte, argv[i]);
        FILE* fichier = fopen(texte, "r");

        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = myMalloc(sizeof(char)*30, memoire); //Plus grand mot français a moins de 30 lettres
        int c = fgetc(fichier);
        while(c != EOF){

            if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == '-'))){ //Si c n'est pas une lettre classique
                if(taille >= 30){
                    taille = 0; //Ce n'est pas un mot on en prend pas compte
                }
                else if(taille < k){
                    taille = 0; //Le mot ne fait pas au moins k lettres
                }

                //fin de mot valide
                if (taille != 0){
                    (*nbr_mots)++;
                    mot[taille] = '\0';
                    ajoutMotLst(lst, mot, memoire);
                    taille = 0;   
                }
            //Ajout lettre au mot
            }else{
                if((c>='A' && c<='Z')){
                    c = c -'A' + 'a';
                }
                if(taille < 29)
                    mot[taille] = c; //Ajout que si on est dans la zone de 30 allouée
                taille++;
            }

            //printf("%c", c);
            c = fgetc(fichier);
        }
        //gestion du dernier mot
        if(taille != 0 && taille >= 30 && taille < k){
            (*nbr_mots)++;
            mot[taille] = '\0';
            ajoutMotLst(lst, mot, memoire);
        }
        fclose(fichier);
    }
    return 1;
}

int listeTriee(char *argv[], Liste * lst, InfoMem * memoire, int k, int * nbr_mots){
    // Fait tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){

        char texte[100] ="textes/";
        strcat(texte, argv[i]);
        FILE* fichier = fopen(texte, "r");
        
        if(!fichier) continue; // Si ce n'est pas un fichier, on saute

        int taille = 0;
        char *mot = myMalloc(sizeof(char) * 30, memoire); // Plus grand mot français a moins de 30 lettres
        int c = fgetc(fichier);
        while(c != EOF){
            if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '-'))){ // Si c n'est pas une lettre classique
                if(taille >= 30){
                    taille = 0; // Ce n'est pas un mot on en prend pas compte
                }else if(taille < k){
                    taille = 0; //Le mot ne fait pas au moins k lettres
                }

                //fin de mot valide
                if(taille != 0){
                    (*nbr_mots)++;
                    mot[taille] = '\0';
                    ajoutMotTriee(lst, mot, memoire);
                    taille = 0;
                }
            // Ajout lettre au mot
            }else{
                if((c >= 'A' && c <= 'Z')){
                    c = c - 'A' + 'a';
                }
                if(taille < 29)
                    mot[taille] = c; // Ajout que si on est dans la zone de 30 allouée
                taille++;
            }

            // printf("%c", c);
            c = fgetc(fichier);
        }
        // gestion du dernier mot 
        if(taille != 0 && taille >= 30 && taille < k){
            (*nbr_mots)++;
            mot[taille] = '\0';
            ajoutMotTriee(lst, mot, memoire);
        }
        myFree(mot, memoire, sizeof(char) * 30);
        fclose(fichier);
    }
    return 1;
}



int fonctionHachage(char * argv[], Table_h * table, InfoMem * memoire, int k, int * nbr_mots){
    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        char texte[100] ="textes/";
        strcat(texte, argv[i]);
        FILE* fichier = fopen(texte, "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = myMalloc(sizeof(char)*30, memoire); //Plus grand mot français a moins de 30 lettres
        int c = fgetc(fichier);
        while(c != EOF){

            if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == '-'))){ //Si c n'est pas une lettre classique
                if(taille >= 30){
                    taille = 0; //Ce n'est pas un mot on en prend pas compte
                }else if(taille < k){
                    taille = 0; //Le mot ne fait pas au moins k lettres
                }

                //fin de mot valide
                if (taille != 0){
                    (*nbr_mots) ++;
                    mot[taille] = '\0';
                    ajoutTableHach(table, mot, memoire);
                    taille = 0;   
                }
            //Ajout lettre au mot
            }else{
                if((c>='A' && c<='Z')){
                    c = c -'A' + 'a';
                }
                if(taille < 29)
                    mot[taille] = c; //Ajout que si on est dans la zone de 30 allouée
                taille++;
            }

            //printf("%c", c);
            c = fgetc(fichier);
        }
        //gestion du dernier mot
        if(taille != 0 && taille >= 30 && taille < k){
            (*nbr_mots)++;
            mot[taille] = '\0';
            ajoutTableHach(table, mot, memoire);
        }
        fclose(fichier);
    }
    return 1;
}



int main(int argc, char *argv[]){
    //Cas du help
    if(argv[1] && strcmp("-help", argv[1])==0){
            printf("Usage : Etudier le nombre d'occurrences de chaque mot d'un texte.\n");
            printf("Lancement : ./main [-help] [-p] [-n int] [-k int] [-a simple|tri|hach] [-s fichierdesortie] [-l fichierdeperf] fichiers de données\n");
            printf("Options :\n");
            printf("-help Affiche ce message d'aide à l'utilisation.\n");
            printf("-p Affiche le résultat dans le terminal.\n");
            printf("-n Permet de n'afficher que les n mots les plus présents dans le texte.\n");
            printf("-k Permet de ne prendre en compte que les mots d'au moins k lettres.\n");
            printf("-a Choix de l'algorithme utilisé : simple (liste classique), tri (liste triée) et hash (table de hachage, l'option par defaut).\n");
            printf("-s Afin de sauvegarder le résultat dans un fichier texte.\n");
            printf("-l Afin de sauvegarder les performances dans un fichier texte.\n");
            printf("fichiers de données Les fichiers sur lesquels l'étude de l'occurence des mots doit être faite.\n");
            return 0;
    }
    int algo = 3; // 1 = simple, 2 = tri, 3 = hach (algorithme par défaut)
    int n = 0; //Nombre de mots à afficher, par défaut on affiche rien, vaut -1 pour tout afficher
    int k = 0; //Par defaut prend tous les mots en compte
    int nbr_mots = 0; //Nombre de mots dans le texte
    for(int i = 0; argv[i]; i++){
        if(argv[i][0]=='-' && argv[i][1]=='a'){
            if(strcmp("simple", argv[i+1]) == 0)
                algo = 1;
            if(strcmp("tri", argv[i+1]) == 0)
                algo = 2;
        }
        if(argv[i][0]=='-' && argv[i][1]=='n'){
            n = atoi(argv[i+1]); //passe d'une chaîne de caractères à un entier
        }
        else if(argv[i][0]=='-' && argv[i][1]=='p' && n ==0) //Verifie qu'on n'affiche pas déjà n nombres dans le terminal
            n = -1; //Affiche tout
        else if(argv[i][0]=='-' && argv[i][1]=='k')
            k = atoi(argv[i+1]);
    }    

    clock_t debut = clock();
    InfoMem * memoire = initInfoMem();
    if(!memoire){
        printf("Erreur d'allocation");
        return 1;
    }



    //Version avec tableau dynamique (pas trié)
    if(algo == 1){
        Liste * lst = initLst(10, memoire);
        if(!lst){
            printf("Erreur d'allocation");
            return 1;
        }
        listeSimple(argv, lst, memoire, k, &nbr_mots);
        triFusionOccurrence(lst, memoire);

        clock_t fin = clock();
        unsigned long duree_milli = (fin -  debut) * 1000 / CLOCKS_PER_SEC;

        ecrireOcc(argv,*lst);
        ecrirePerf(argv, *memoire, duree_milli, nbr_mots, lst->tailleLst);
        afficheLst(*lst,n);
    }

    //Version liste triée par ordre alphabétique
    else if(algo == 2){
        Liste * lst = initLst(10, memoire);
        if(!lst){
            printf("Erreur d'allocation");
            return 1;
        }
        listeTriee(argv, lst, memoire, k, &nbr_mots);
        triFusionOccurrence(lst, memoire);

        clock_t fin = clock();
        unsigned long duree_milli = (fin -  debut) * 1000 / CLOCKS_PER_SEC;
        ecrireOcc(argv,*lst);
        ecrirePerf(argv, *memoire, duree_milli, nbr_mots, lst->tailleLst);
        afficheLst(*lst,n);
    }

    //Version table de hachage
    else{
        Table_h * table = initTableHach(30, memoire);
        if(!table){
            printf("Erreur d'allocation");
            return 1;
        }
        fonctionHachage(argv, table, memoire, k, &nbr_mots);

        Liste * resultat = tableHach_to_lst(table, memoire);
        //freeTableHash(table, memoire); //Libere la table maintenant qu'elle est convertie en liste
        triFusionOccurrence(resultat, memoire);

        clock_t fin = clock();
        unsigned long duree_milli = (fin -  debut) * 1000 / CLOCKS_PER_SEC;

        ecrireOcc(argv,*resultat);
        ecrirePerf(argv, *memoire, duree_milli, nbr_mots, resultat->tailleLst);
        afficheLst(*resultat, n);
    }

    return 0;
}
