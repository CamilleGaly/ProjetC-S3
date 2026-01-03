# Initiation à la programmation C
# Projet du semestre 3 
## Nombre d’occurrences des mots d’un texte
## Comparaison d’algorithmes

TD double-licences   
Amaury Coquelin et Camille Galy Sagrasta  
Rendu du 06/01/2026

<div style="page-break-after: always;"></div>

## Contenu du fichier
* Un fichier main.c contenant le coeur du programme.
* Un fichier gererMem.c contenant la structure pour étudier la mémoire et les fonctions d'allocation ou de désallocation y faisant appel.
* Des fichiers xxhash.c et xxhsh.h permettant l'utilisation d'une fonction de hachage.
* Un fichier graphiqueMotsCourants.py permettant comme son nom l'indique de générer un graphique des mots les plus courants à partir d'un fichier texte de résultats généré par le programme de main.c.
* Un fichier readMe.txt détaillant le lancement du programme.
* Un dossier textes avec les fichiers textes utilisés par les algorithmes.

## Algorithmes utilisés

### Structure générale 
Une structure générale à été adoptée dans chaque algorithme afin de réprésenter un mot et son nombre d'occurences.

```
typedef struct occMot{
    char * mot;
    int occurrence;
} OccMot;
```
Une structure Liste est également commune :
```
typedef struct liste{  
    OccMot * occmot;  
    int tailleLst; //Espace occupé  
    int tailleMax; //Espace alloué  
} Liste;
```

A la fin du programme on obtient une Liste contenant le nombre d'occurence de chaque mot du texte.  
Afin de renvoyer un résultat trié par nombre d'apparition des mots un tri fusion est effectué.  
La complexité de celui-ci est en o(nlog(n)), il est gourmand en espace mémoire bien que très efficace.  

### Liste non triée

Cet algorithme est basé uniquement sur l'utilisation de la structure Liste.  

1. Utilisation du temps :  
A chaque nouveau mot rencontré la liste est parcourue intégralement afin de déterminer si celui-ci est déjà présent.  
Lors de la création de la liste on distingue 2 cas de figure :  
    - Le mot est déjà dans la liste, simple recherche :  
        On parcourt les éléments un à un : o(n).
    - Le mot n'est pas dans la liste, il faut l'ajouter :  
        On fait une recherche (mais ne trouve pas le mot) et l'ajoute donc à la fin, donc o(n) + o(1) donc o(n).  

->  Chaque mot croisé dans le texte entraîne donc une opération en o(n).  

2. Utilisation de la mémoire :    
La mémoire occupée dépend du nombre de mots différents du texte et non du nombre réel de mots du texte.  
La taille étant double à chaque fois l'espace occupé par la Liste d'OccMot sera au maximum 2 fois la taille d'OccMot multiplié par le nombre de mots différents du texte.  



### Liste triée
L'algorithme utilisant une liste triée reprend la structure de Liste précédente mais en veillant à conserver un tri alphabétique. 
1. Utilisation du temps :  
Le fait d'avoir une liste triée permet de grandement améliorer l'efficacité de l'algorithme d'un point de vue complexité :  
Lors de la création de la liste on distingue 2 cas de figure :  
    - Le mot est déjà dans la liste, simple recherche :  
        On effectue une recherche dichotomique qui est en o(logn).  
    - Le mot n'est pas dans la liste, il faut l'ajouter :  
        On effectue une recherche dichotomique puis insère dans le tableau à la bonne place en décalant les éléments suivants, o(log(n)) + o(n) donc o(n).  

-> La majorité des textes contenant de nombreux mots plusieurs fois la plupart des appels sera en o(log(n)) et certains seront seront en o(n).  



2. Utilisation de la mémoire :   
L'utilisation de mémoire est strictement identique à celle de l'algorithme utilisant une liste non triée.

### Table de hachage
Pour cet algorithme une structure de table de hachage à été utilisée :
```
typedef struct table_h{
    int capacite; //Taille de la table
    int occupation; //Nombre de cases occupées
    OccMot ** occmot; //Liste de pointeurs
} Table_h;
```
Il a été choisi d'utiliser une table de hachage interne. En effet l'algorithme ayant pour but de simplement ajouter des mots aucune suppression n'a lieu ce qui rend plus simple l'implémentation d'une table interne.  

L'algorithme utilise la fonction de hachage xxhash importée grâce à xxhash.c et xxhash.h.  

1. Utilisation du temps :  
La complexité d'un algorithme utilisant une table de hachage dépend du nombre de collision, ici la taille de la table est doublée lorsque le facteur de charge est supérieur ou égal a 75%.  
La complexité des operations de recherche et d'ajout est alors un complexité amortie en moyenne en o(1).  

## Comparaison des performances observées

Utilisation mémoire : hachage > liste (triée = simple)  
Temps : hachage < liste triée < liste simple

Graphiques : 
Temps de chaques sur textes identiques
Memoire sur txt id

pour chaque algo :  
Evolution du tps/memoire en fonction du nbr de mots totaux  
Evolution du temps/memoire en fonction du nbr de mots diff

## Organisation du travail

### Conception des algorithmes
[ Votre organisation en termes de recherche et de conception d’algorithmes.]  
idée -> algo image -> code

[Jsp ce qu'on doit écrire ici en vrai...]

## Organisation du groupe
Contributions en % :
* Amaury :
* Camille :

Algorithmes implémentés :
* Amaury : listes triées
* Camille : liste classique et table de hachage  


