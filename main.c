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

void afficheMemoire(InfoMem memoire){
    printf("Utilisation de la memoire :\nTotal allocations : %d\nTotal desallocation : %d\nPic d'allocation : %d\n", memoire.cumul_alloc, memoire.cumul_desalloc, memoire.max_alloc);
}



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

//Ajoute un mot au tableau ou augmente son nbr d'occurences si déjà présent
//Plus tard on pourra ptet envisager une version avec un tableau trié et une recherche dichotomique
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

            if(!((c>64 && c<91)||(c>96 && c<123))){ //Si c n'est pas une lettre classique
                //fin de mot
                if (taille != 0){
                    mot[taille] = '\0';
                    ajoutMotLst(lst, mot, memoire);
                    taille = 0;   
                }
            //Ajout lettre au mot
            }else{
                if((c>64 && c<91)){
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
            ajoutMotLst(lst, mot, memoire);
        }
        fclose(fichier);
    }
    return 1;
}


/////////////// Ecriture des resultats dans un fichier ////////////////////

void ecrireOcc(Liste lst){
    FILE* fichier = fopen("resultatOcc.txt", "w");
    for(int i = 0; i < lst.tailleLst; i++){
        fprintf(fichier, "%s %d \n", lst.occmot[i].mot, lst.occmot[i].occurence);
    }
    fclose(fichier);
}



int main(int argc, char *argv[]){
    InfoMem * memoire = initInfoMem();
    if(!memoire){
        printf("Erreur d'allocation");
        return 1;
    }

    //Version avec tableau dynamique (pas trié)
    Liste * lst1 = initLst(10, memoire);
    if(!lst1){
        printf("Erreur d'allocation");
        return 1;
    }
    listeSimple(argv, lst1, memoire);
    printf("\nListe 1 : \n");
    afficheLst(*lst1);

    triFusionOccurence(lst1, memoire);
    ecrireOcc(*lst1);
    afficheMemoire(*memoire);

   return 0;
}