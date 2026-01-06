import matplotlib.pyplot as plt

def lireMots(fichier):
    """
    Crée une liste de tuples qui contiennent un mot et son nombre d'occurrences
    Entrée : fichier (str), le nom d'un fichier présent dans le même répertoire que le code présent
    Sortie : une liste de (str, int) comme expliqué en tête de cette documentation
    """
    with open(fichier, "r") as f:
        return [(mot, int(occ)) for mot, occ in (ligne.split() for ligne in f)]

def affichePremiersMots(nombreMots, donnees):
    """
    Affiche le graphique qui représente les mots les plus présents d'un texte et leur répartition en nombre classés par ordre décroissant de haut en bas
    Entrées : nombreMots (int), le nombre de mots qu'on veut voir sur le graphique
              donnees, une liste de (tuple) qui contiennent un mot (str) et son nombre d'occurrences (int) dans le texte
    Sorties : rien, on affiche simplement le graphique à l'aide de la bibliothèque matplotlib
    """
    
    nombreMots = min(nombreMots, len(donnees)) # Si l'utilisateur saisit un nombre de mots plus grand que n'en contient réellement la liste "donnees"
    
    liste_mots = []
    liste_occurrences = []
    
    for i in range(nombreMots):
        liste_mots.append(donnees[i][0])
        liste_occurrences.append(donnees[i][1])
    
    plt.barh(liste_mots, liste_occurrences)
    plt.ylabel("Mot")
    plt.xlabel("Nombre d'occurrences")
    plt.title(f"Les {nombreMots} mots les plus fréquents")
    plt.gca().invert_yaxis()
    plt.show()    

def main():
    fichier = input("Quels résultats traiter ? ")
    nombreMots = int(input("Saisissez le nombre de mots que vous voulez afficher : "))
    mots = lireMots(fichier)
    affichePremiersMots(nombreMots, mots)

if __name__ == "__main__":
    main()
