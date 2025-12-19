#include<stdio.h>
#include<stdlib.h>
#include "gererMem.c"

typedef struct {
    char * mot;
    int occurence;
} OccMot;

typedef struct occMotChaine{
    char * mot;
    struct occMotChaine * suivant;
    int occurence;
} OccMotChaine;




int fct1(char *argv[]){
    for(int i = 0; argv[i]; i++){
        FILE* fichier = fopen(argv[i], "r");
        if (!fichier) continue;

        int taille = 0;
        char * mot = NULL;
        char c = fgetc(fichier);
        while(c != EOF){

            if ((c == '\n' || c=='\t' || c ==' ')){
                //fin de mot
                if (taille != 0){
                    //AJOUTER MOT DANS LST STRUCT
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
        fclose(fichier);
    }
    return 1;
}

int main(int argc, char *argv[]){
    fct1(argv);
}