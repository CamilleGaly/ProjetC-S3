#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "gererMem.c"

typedef struct occMot{
    char * mot;
    struct occMot * suivant;
    int occurence;
} OccMot, * Liste;

void afficheLst(Liste lst){
    for(; lst != NULL; lst = lst->suivant){
        printf("%s : %d apparition(s)\n", lst->mot, lst->occurence);
    }
}

OccMot * creerOccMot(char * mot, int taille){
    OccMot * ptr = NULL;
    ptr = malloc(sizeof(OccMot));
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


//Ajoute un mot à la lst chainée ou augmente son nbr d'occurences si déjà présent
void ajoutMot(Liste * lst, char * mot, int taille){
    for(; (*lst) != NULL ; lst = &(*lst)->suivant){
        if(strcmp((*lst)->mot, mot)==0){
            (*lst)->occurence ++;
            return;
        }
    }

    OccMot * new = creerOccMot(mot, taille);
    *lst = new;
    return;
}


int fct1(char *argv[], Liste * lst){

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
                    ajoutMot(lst, mot, taille);
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
            ajoutMot(lst, mot, taille);
        }
        fclose(fichier);
    }
    return 1;
}

int main(int argc, char *argv[]){
    Liste lst = NULL;
    Liste lst2 = NULL;
    char mot[25] = { 'B', 'o', 'n', 'j', 'o', 'u', 'r' };
    char mot2[12] = { 'B', 'o', 'n' };
    ajoutMot(&lst, mot, 7);
    ajoutMot(&lst, mot2, 3);
    ajoutMot(&lst, mot, 7);
    afficheLst(lst);

    fct1(argv, &lst2);
    printf("\nListe 2 : \n");
    afficheLst(lst2);

    /*
    OccMot * liste1;
    liste1 = malloc(sizeof(OccMot)*50);
    fct1(argv, liste1);
    */
   return 0;
}