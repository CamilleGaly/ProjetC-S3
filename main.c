#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "gererMem.c"
#include "xxhash.c" //hash = XXH3_64bits(donnée, taille_donnée);

typedef struct occMotChaine{
    char * mot;
    struct occMotChaine * suivant;
    int occurence;
} OccMotChaine, * ListeChaine;

typedef struct occMot{
    char * mot;
    int occurence;
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

void afficheLstChaine(ListeChaine lst){
    for(; lst != NULL; lst = lst->suivant){
        printf("%s : %d apparition(s)\n", lst->mot, lst->occurence);
    }
}

void afficheLst(Liste lst){
    for(int i = 0; i < lst.tailleLst; i++){
        printf("%s : %d apparition(s)\n", lst.occmot[i].mot, lst.occmot[i].occurence);
    }
}

void afficheTableHach(Table_h table){
    for(int i = 0; i < table.capacite; i++){
        if(table.occmot[i] != NULL){
            printf("%s : %d apparition(s)\n", table.occmot[i]->mot, table.occmot[i]->occurence);
        }
    }
}

void afficheMemoire(InfoMem memoire){
    printf("\nUtilisation de la memoire :\nTotal allocations : %ld\nTotal desallocation : %ld\nPic d'allocation : %ld\n", (unsigned long)memoire.cumul_alloc, (unsigned long)memoire.cumul_desalloc, (unsigned long)memoire.max_alloc);
}


//////////////////// Ecriture des resultats dans un fichier ////////////////////

void ecrireOcc(Liste lst){
    FILE* fichier = fopen("resultatOcc.txt", "w");
    for(int i = 0; i < lst.tailleLst; i++){
        fprintf(fichier, "%s %d \n", lst.occmot[i].mot, lst.occmot[i].occurence);
    }
    fclose(fichier);
}

//////////////////// Liste Chainées ////////////////////

//J'avais commencé par des lst chaînées jsp pk mais ça peut-être utile pour la version hash je pense
//taille correspond à la taille du mot
OccMotChaine * creerOccMotChaine(char * mot, int taille, InfoMem * memoire){
    OccMotChaine * ptr = myMalloc(sizeof(OccMotChaine), memoire);
    if(!ptr){
        return NULL; //erreur
    }

    char * ptrMot = myMalloc(taille+1, memoire);
    if(!ptrMot){
        myFree(ptrMot, memoire, taille+1);
        myFree(ptr, memoire, sizeof(OccMotChaine));
        return NULL;
    }
    ptrMot[taille] = '\0';
    ptr->mot = ptrMot;
    strcpy(ptr->mot, mot);
    ptr->occurence = 1;
    ptr->suivant = NULL;

    // ptr->mot = strdup(mot);  Fct donnée par Nathan pour allouer directement un pointeur en copiant (c'est tellement plus simple)

    return ptr;
}

//Ajoute un mot à la lst chainée ou augmente son nbr d'occurences si déjà présent
void ajoutMotChaine(ListeChaine * lst, char * mot, int taille, InfoMem * memoire){
    for(; (*lst) != NULL ; lst = &(*lst)->suivant){
        if(strcmp((*lst)->mot, mot)==0){
            (*lst)->occurence ++;
            return;
        }
    }

    OccMotChaine * new = creerOccMotChaine(mot, taille, memoire);
    *lst = new;
    return;
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

//Ajoute un mot au tableau ou augmente son nbr d'occurences si déjà présent
void ajoutMotLst(Liste * lst, char * mot, InfoMem * memoire){
    for(int i = 0; i< lst->tailleLst ; i++){
        if(strcmp((lst->occmot)[i].mot, mot)==0){
            (lst->occmot)[i].occurence ++;
            return;
        }
    }

    if (lst->tailleLst == lst->tailleMax){
        lst->occmot = myRealloc(lst->occmot, sizeof(OccMot)*lst->tailleMax * 2, memoire, sizeof(OccMot)* lst->tailleMax); //Agrandis la taille en doublant
        lst->tailleMax = lst->tailleMax * 2;
    }
    
    lst->occmot[lst->tailleLst].mot = myStrdup(mot, memoire);
    lst->occmot[lst->tailleLst].occurence = 1;
    lst->tailleLst ++;

    return;
}

void fusionLst(Liste * lst, Liste *a, Liste *b, InfoMem * memoire){
    int pt_a = 0, pt_b = 0;

    for(int i = 0; i < lst->tailleLst; i++){
        myFree(lst->occmot[i].mot, memoire, strlen(lst->occmot[i].mot) + 1);
        if(pt_a == a->tailleLst){
            lst->occmot[i].mot = myStrdup(b->occmot[pt_b].mot, memoire);
            lst->occmot[i].occurence = b->occmot[pt_b].occurence;
            pt_b ++;
        }else if(pt_b == b->tailleLst){
            lst->occmot[i].mot = myStrdup(a->occmot[pt_a].mot, memoire);
            lst->occmot[i].occurence = a->occmot[pt_a].occurence;
            pt_a ++;
        }else if(a->occmot[pt_a].occurence > b->occmot[pt_b].occurence){
            lst->occmot[i].mot = myStrdup(a->occmot[pt_a].mot, memoire);
            lst->occmot[i].occurence = a->occmot[pt_a].occurence;
            pt_a ++;
        }else{
            lst->occmot[i].mot = myStrdup(b->occmot[pt_b].mot, memoire);
            lst->occmot[i].occurence = b->occmot[pt_b].occurence;
            pt_b ++;
        }
    }
}

//Tri une liste en fonction du nombre d'occurence de chaque mot (pour la visualisation finale)
void triFusionOccurence(Liste * lst, InfoMem * memoire){
    if(lst->tailleLst == 0 || lst->tailleLst == 1){
        return;
    }

    //Sépare en 2
    int taille_a = lst->tailleLst / 2;
    Liste *a = initLst(taille_a, memoire);
    for (int i = 0; i < taille_a; i++) {
        a->occmot[i].mot = myStrdup(lst->occmot[i].mot, memoire);
        a->occmot[i].occurence = lst->occmot[i].occurence;
    }
    a->tailleLst = taille_a;

    int taille_b = lst->tailleLst - lst->tailleLst / 2;
    Liste * b = initLst(taille_b, memoire);
    for (int i = 0; i < taille_b; i++) {
        b->occmot[i].mot = myStrdup(lst->occmot[taille_a + i].mot, memoire);
        b->occmot[i].occurence = lst->occmot[taille_a + i].occurence;
    }
    b->tailleLst = b->tailleMax;

    triFusionOccurence(a, memoire);
    triFusionOccurence(b, memoire);

    fusionLst(lst,a,b, memoire); //Détruit a et b et remplit lst de façon triée
    freeListe(a, memoire);
    freeListe(b, memoire);
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
    occMot->occurence = 1;
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
            table->occmot[hach]->occurence += 1;
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
            lst->occmot[i].occurence = table->occmot[j]->occurence;
            i++;
        }
    }
    lst->tailleMax = table->occupation;
    lst->tailleLst = table->occupation;
    return lst;
}



//////////////////// Fonctions principales ////////////////////


//Version test avec la structure chaînée
int fct0(char *argv[], ListeChaine * lst, InfoMem * memoire){

    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = myMalloc(50, memoire); //Bourrin faudra essayer de faire bien avec des realloc (ou pas)
        int c = fgetc(fichier);
        while(c != EOF){
            if ((c == '\n' || c=='\t' || c ==' ')){
                //fin de mot
                if (taille != 0){
                    //printf("Ajout mot");
                    mot[taille] = '\0';
                    ajoutMotChaine(lst, mot, taille, memoire);
                    taille = 0;   
                }
            //Ajout lettre au mot
            }else{
                mot[taille] = c;
                taille++;
            }

            printf("%c", c);
            c = fgetc(fichier);
        }
        //gestion du dernier mot
        if (taille != 0){
            mot[taille] = '\0';
            ajoutMotChaine(lst, mot, taille, memoire);
        }
        fclose(fichier);
    }
    return 1;
}

int listeSimple(char *argv[], Liste * lst, InfoMem * memoire){

    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = myMalloc(sizeof(char)*30, memoire); //Plus grand mot français a moins de 30 lettres
        int c = fgetc(fichier);
        while(c != EOF){

            if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == '-'))){ //Si c n'est pas une lettre classique
                if(taille >= 30){
                    taille = 0; //Ce n'est pas un mot on en prend pas compte
                }
                //fin de mot
                if (taille != 0){
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
        if (taille != 0){
            mot[taille] = '\0';
            ajoutMotLst(lst, mot, memoire);
        }
        fclose(fichier);
    }
    return 1;
}



int fonctionHachage(char * argv[], Table_h * table, InfoMem * memoire){
    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = myMalloc(sizeof(char)*30, memoire); //Plus grand mot français a moins de 30 lettres
        int c = fgetc(fichier);
        while(c != EOF){

            if(!((c>='a' && c<='z') || (c>='A' && c<='Z') || (c == '-'))){ //Si c n'est pas une lettre classique
                if(taille >= 30){
                    taille = 0; //Ce n'est pas un mot on en prend pas compte
                }
                //fin de mot
                if (taille != 0){
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
        if (taille != 0){
            mot[taille] = '\0';
            ajoutTableHach(table, mot, memoire);
        }
        fclose(fichier);
    }
    return 1;
}



int main(int argc, char *argv[]){
    InfoMem * memoire = initInfoMem();
    if(!memoire){
        printf("Erreur d'allocation");
        return 1;
    }
/*
    //Version avec tableau dynamique (pas trié)
    Liste * lst1 = initLst(10, memoire);
    if(!lst1){
        printf("Erreur d'allocation");
        return 1;
    }
    listeSimple(argv, lst1, memoire);
    printf("\nListe 1 : \n");
    //afficheLst(*lst1);

    triFusionOccurence(lst1, memoire);
    ecrireOcc(*lst1);
    afficheMemoire(*memoire);
*/
    //Version table de hachage
    Table_h * table = initTableHach(30, memoire);
    if(!table){
        printf("Erreur d'allocation");
        return 1;
    }
    fonctionHachage(argv, table, memoire);
    printf("\nResultat table Hachage : \n");
    afficheTableHach(*table);

    Liste * resultat = tableHach_to_lst(table, memoire);
    printf("\nListe : \n");
    afficheLst(*resultat);
    triFusionOccurence(resultat, memoire);
    ecrireOcc(*resultat);
    afficheMemoire(*memoire);
    return 0;
}