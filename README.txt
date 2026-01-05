Initiation à la Programmation C - Projet S3
Nombre d’occurrences des mots d’un texte
Comparaison d’algorithmes
Rendu du 06/01/2026
Amaury Coquelin, Camille Galy Sagrasta
TD Double Licence
-------------------------------------------------
Fichiers présents:
* Un fichier main.c contenant le coeur du programme.
* Un fichier gererMem.c contenant la structure pour étudier la mémoire et les fonctions d'allocation ou de désallocation y faisant appel.
* Des fichiers xxhash.c et xxhsh.h permettant l'utilisation d'une fonction de hachage.
* Des fichiers graphiqueMotsCourants.py et graphiquePerfs permettant de générer des graphiques à partir des résultats des algorithmes.
* Un fichier readMe.txt détaillant le lancement du programme.
* Un dossier textes avec les fichiers textes utilisés par les algorithmes.
* Un dossier performances avec les fichiers de performance générés par le programme, ces fichiers sont utilisés afin de générer des graphiques de performance.
-------------------------------------------------
Lancement : 
1. Dézipper l'archive .zip téléchargée
2. Compiler le fichier main.c avec la commande gcc main.c -o main
3. Lancer le programme avec la commande ./main [-help] [-p] [-n int] [-k int] [-a simple|tri|hach] [-s fichierdesortie] [-l fichierdeperf] fichiers de données

-> Le détail du fonctionnement des options peut être visualisé avec ./main -help