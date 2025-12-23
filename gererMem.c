#include<stdio.h>
#include<stdlib.h>

typedef struct {
    size_t cumul_alloc; //cumul de l’espace mémoire alloué
    size_t cumul_desalloc; //cumul de l’espace mémoire désalloué
    size_t max_alloc; //pic d'allocation mémoire
} InfoMem;

InfoMem * initInfoMem(){
    InfoMem * memoire = malloc(sizeof(InfoMem));
    if(!memoire){
        return NULL;
    }
    memoire->cumul_alloc = 0;
    memoire->cumul_desalloc = 0;
    memoire->max_alloc = 0;
    return memoire;
}

void* myMalloc(size_t size, InfoMem* infoMem){
    void* pt = NULL;
    pt = malloc(size);
    if(pt == NULL)
        return NULL; //echec de l'allocation
    
    infoMem->cumul_alloc += size;

    //Gestion du pic d'allocation
    if(infoMem->cumul_alloc - infoMem->cumul_desalloc > infoMem->max_alloc){
        infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
    }
    return pt;
}

//Gestion du malloc implicite de strdup
void* myStrdup(char * mot, InfoMem * infoMem){
    infoMem->cumul_alloc += strlen(mot) + 1;

    //Gestion du pic d'allocation
    if(infoMem->cumul_alloc - infoMem->cumul_desalloc > infoMem->max_alloc){
        infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
    }
    return strdup(mot);
}

void* myRealloc(void* ptr, size_t new_size, InfoMem* infoMem, size_t old_size){
    void* newPtr = NULL;
    newPtr = realloc(ptr, new_size);

    if(newPtr == NULL)
        return NULL; //echec allocation
    
    if(new_size < old_size){//diminution zone mémoire
        infoMem->cumul_desalloc += old_size - new_size;
    }else if(newPtr == ptr){ //aggrandissement de la zone memoire
        infoMem->cumul_alloc += new_size - old_size;
    }else{ //déplacement
        infoMem->cumul_alloc += new_size; 
        infoMem->cumul_desalloc += old_size;
    }

    //Gestion du pic d'allocation
    if(infoMem->cumul_alloc - infoMem->cumul_desalloc > infoMem->max_alloc){
        infoMem->max_alloc = infoMem->cumul_alloc - infoMem->cumul_desalloc;
    }
    return newPtr;
}

void myFree(void* ptr, InfoMem* infoMem, size_t old_size){
    free(ptr);
    infoMem->cumul_desalloc += old_size;
}