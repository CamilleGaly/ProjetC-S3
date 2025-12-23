#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "gererMem.c"

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

//J'avais commencé par des lst chaînées jsp pk mais ça peut-être utile pour la version hash je pense
//taille correspond à la taille du mot
OccMotChaine * creerOccMotChaine(char * mot, int taille){
    OccMotChaine * ptr = NULL;
    ptr = malloc(sizeof(OccMotChaine));
    if(!ptr){
        free(ptr);
        return NULL; //erreur
    }

    char * ptrMot = malloc(sizeof(char)*(taille+1));
    ptrMot[taille] = '\0';
    if(!ptrMot){
        free(ptrMot);
        free(ptr);
        return NULL;
    }
    ptr->mot = ptrMot;
    strcpy(ptr->mot, mot);
    ptr->occurence = 1;
    ptr->suivant = NULL;

    // ptr->mot = strdup(mot);  Fct donnée par Nathan pour allouer directement un pointeur en copiant (c'est tellement plus simple)

    return ptr;
}

Liste * initLst(int taille){
    Liste *lst = malloc(sizeof(Liste));
    if (!lst) 
        return NULL;

    lst->occmot = malloc(sizeof(OccMot)*taille); //Initialise avec une capacité de taille mots
    if(lst->occmot == NULL){
        free(lst);
        return NULL; //erreur
    }
    lst->tailleLst = 0;
    lst->tailleMax = taille;
    return lst;
}

void freeListe(Liste *lst){
    if(!lst)
        return;

    if(lst->occmot != NULL){
        for (int i = 0; i < lst->tailleLst; i++){
            free(lst->occmot[i].mot);
        }
        free(lst->occmot);
    }
    free(lst);
}

//Ajoute un mot à la lst chainée ou augmente son nbr d'occurences si déjà présent
void ajoutMotChaine(ListeChaine * lst, char * mot, int taille){
    for(; (*lst) != NULL ; lst = &(*lst)->suivant){
        if(strcmp((*lst)->mot, mot)==0){
            (*lst)->occurence ++;
            return;
        }
    }

    OccMotChaine * new = creerOccMotChaine(mot, taille);
    *lst = new;
    return;
}

//Ajoute un mot au tableau ou augmente son nbr d'occurences si déjà présent
//Plus tard on pourra ptet envisager une version avec un tableau trié et une recherche dichotomique
void ajoutMotLst(Liste * lst, char * mot){
    for(int i = 0; i< lst->tailleLst ; i++){
        if(strcmp((lst->occmot)[i].mot, mot)==0){
            (lst->occmot)[i].occurence ++;
            return;
        }
    }

    if (lst->tailleLst == lst->tailleMax){
        lst->occmot = realloc(lst->occmot, sizeof(OccMot)* lst->tailleMax * 2); //Agrandis la taille en doublant
        lst->tailleMax = lst->tailleMax * 2;
    }
    
    lst->occmot[lst->tailleLst].mot = strdup(mot);
    lst->occmot[lst->tailleLst].occurence = 1;
    lst->tailleLst ++;

    return;
}

void fusionLst(Liste * lst, Liste *a, Liste *b){
    int pt_a = 0, pt_b = 0;

    for(int i = 0; i < lst->tailleLst; i++){
        free(lst->occmot[i].mot);
        if(pt_a == a->tailleLst){
            lst->occmot[i].mot = strdup(b->occmot[pt_b].mot);
            lst->occmot[i].occurence = b->occmot[pt_b].occurence;
            pt_b ++;
        }else if(pt_b == b->tailleLst){
            lst->occmot[i].mot = strdup(a->occmot[pt_a].mot);
            lst->occmot[i].occurence = a->occmot[pt_a].occurence;
            pt_a ++;
        }else if(a->occmot[pt_a].occurence > b->occmot[pt_b].occurence){
            lst->occmot[i].mot = strdup(a->occmot[pt_a].mot);
            lst->occmot[i].occurence = a->occmot[pt_a].occurence;
            pt_a ++;
        }else{
            lst->occmot[i].mot = strdup(b->occmot[pt_b].mot);
            lst->occmot[i].occurence = b->occmot[pt_b].occurence;
            pt_b ++;
        }
    }
}

//Tri une liste en fonction du nombre d'occurence de chaque mot (pour la visualisation finale)
void triFusionOccurence(Liste * lst){
    if(lst->tailleLst == 0 || lst->tailleLst == 1){
        return;
    }

    //Sépare en 2
    int taille_a = lst->tailleLst / 2;
    Liste *a = initLst(taille_a);
    for (int i = 0; i < taille_a; i++) {
        a->occmot[i].mot = strdup(lst->occmot[i].mot);
        a->occmot[i].occurence = lst->occmot[i].occurence;
    }
    a->tailleLst = taille_a;

    int taille_b = lst->tailleLst - lst->tailleLst / 2;
    Liste * b = initLst(taille_b);
    for (int i = 0; i < taille_b; i++) {
        b->occmot[i].mot = strdup(lst->occmot[taille_a + i].mot);
        b->occmot[i].occurence = lst->occmot[taille_a + i].occurence;
    }
    b->tailleLst = b->tailleMax;

    triFusionOccurence(a);
    triFusionOccurence(b);

    fusionLst(lst,a,b); //Détruit a et b et remplit lst de façon triée
    freeListe(a);
    freeListe(b);
}

//Version test avec la structure chaînée
int fct0(char *argv[], ListeChaine * lst){

    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = malloc(sizeof(char)*50); //Bourrin faudra essayer de faire bien avec des realloc (ou pas)
        int c = fgetc(fichier);
        while(c != EOF){
            if ((c == '\n' || c=='\t' || c ==' ')){
                //fin de mot
                if (taille != 0){
                    //printf("Ajout mot");
                    mot[taille] = '\0';
                    ajoutMotChaine(lst, mot, taille);
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
            ajoutMotChaine(lst, mot, taille);
        }
        fclose(fichier);
    }
    return 1;
}

int fct1(char *argv[], Liste * lst){

    //Fais tous les arguments pour trouver un fichier
    for(int i = 1; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;//Si c'est pas un ficher on saute

        int taille = 0;
        char * mot = malloc(sizeof(char)*30); //Bourrin faudra essayer de faire bien avec des realloc (ou pas)
        int c = fgetc(fichier);
        while(c != EOF){
            //if ((c == '\n' || c=='\t' || c ==' ')){
            if(!((c>64 && c<91)||(c>96 && c<123)||(c=='-'))){ //Si c n'est pas une lettre ni un tiret
                //fin de mot
                if (taille != 0){
                    mot[taille] = '\0';
                    ajoutMotLst(lst, mot);
                    taille = 0;   
                }
            //Ajout lettre au mot
            }else{
                if(c>64 && c<91){
                    c = c -'A' + 'a'; //Si majuscule passe en minuscule
                }

                mot[taille] = c;
                taille++;
            }

            printf("%c", c);
            c = fgetc(fichier);
        }
        //gestion du dernier mot
        if (taille != 0){
            mot[taille] = '\0';
            ajoutMotLst(lst, mot);
        }
        fclose(fichier);
    }
    return 1;
}

int main(int argc, char *argv[]){
    /*
    //Version de test pour les lst chaînées
    ListeChaine lst0 = NULL;
    fct0(argv, &lst0);
    printf("\nListe 0 : \n");
    afficheLstChaine(lst0);
    */

    //Version avec tableau dynamique (pas trié)
    Liste * lst1 = initLst(10);
    if(!lst1){
        printf("Erreur d'allocation");
        return 1;
    }
    fct1(argv, lst1);
    printf("\nListe 1 : \n");
    afficheLst(*lst1);

    triFusionOccurence(lst1);

   return 0;
}