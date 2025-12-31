import matplotlib.pyplot as plt
import numpy as np

def lireMots(fichier):
    with open(fichier, "r") as f:
        return [(mot, int(occ)) for mot, occ in (ligne.split() for ligne in f)]

def affiche_premiers_mots(nombreMots, donnees):
    nombreMots = min(nombreMots, len(donnees)) # Si l'utilisateur saisit un nombre de mots plus grand que n'en contient réellement la liste "donnees"
    
    liste_mots = []
    liste_occurrences = []
    
    for i in range(nombreMots):
        liste_mots.append(donnees[i][0])
        liste_occurrences.append(donnees[i][1])
    
    plt.barh(liste_mots, liste_occurrences)
    plt.ylabel("Mot")
    plt.xlabel("Nombre d'occurrences")
    plt.title(f"Les{nombreMots} mots les plus fréquents")
    plt.gca().invert_yaxis()
    plt.show()
    
def main():
    mots = lireMots("resultatOcc.txt")
    affiche_premiers_mots(15, mots)

if __name__ == "__main__":
    main()
