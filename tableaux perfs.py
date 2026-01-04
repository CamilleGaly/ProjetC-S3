import matplotlib.pyplot as plt
import os            
    
def extraireNomAlgo(fichier):
    return fichier.split("__")[0]


def lirePerfs(fichier):
    """
    Crée un dictionnaire qui contient les informations d'un fichier de performances
    Entrées : fichier (str), le titre d'un fichier dont on veut récupérer les informations
    """
    data = {}
    with open(fichier, "r") as f:
        # Lecture de la première ligne
        firstline = f.readline().strip()
        nb_total, nb_distincts = map(int, firstline.split())
        data["nbTotal"] = nb_total
        data["nbDist"] = nb_distincts
        
        # Lecture du reste du fichier
        for ligne in f:
            ligne = ligne.strip()
            if ligne.startswith("Allocations"):
                data["allocations"] = int(ligne.split(":")[1].strip())
            elif ligne.startswith("Desallocation"):
                data["desallocations"] = int(ligne.split(":")[1].strip())
            elif ligne.startswith("Pic"):
                data["pic"] = int(ligne.split(":")[1].strip())
            elif ligne.startswith("Temps"):
                data["temps_ms"] = int(ligne.split(":")[1].strip().split()[0])
    return data

def analyseAlgo(performances, algo):
    if algo not in performances:
        return

    data = performances[algo]

    # ----- Temps vs nbTotal -----
    x = [d["nbTotal"] for d in data]
    y = [d["temps_ms"] for d in data]

    plt.figure(figsize=(7, 5))
    plt.scatter(x, y)
    plt.xlabel("Nombre total de mots")
    plt.ylabel("Temps (ms)")
    plt.title(f"{algo} — Temps vs nombre total de mots")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    # ----- Temps vs nbDist -----
    x = [d["nbDist"] for d in data]
    y = [d["temps_ms"] for d in data]

    plt.figure(figsize=(7, 5))
    plt.scatter(x, y)
    plt.xlabel("Nombre de mots distincts")
    plt.ylabel("Temps (ms)")
    plt.title(f"{algo} — Temps vs nombre de mots distincts")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    # ----- Pic mémoire vs nbTotal -----
    x = [d["nbTotal"] for d in data]
    y = [d["pic"] for d in data]

    plt.figure(figsize=(7, 5))
    plt.scatter(x, y)
    plt.xlabel("Nombre total de mots")
    plt.ylabel("Pic mémoire")
    plt.title(f"{algo} — Pic mémoire vs nombre total de mots")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

    # ----- Pic mémoire vs nbDist -----
    x = [d["nbDist"] for d in data]
    y = [d["pic"] for d in data]

    plt.figure(figsize=(7, 5))
    plt.scatter(x, y)
    plt.xlabel("Nombre de mots distincts")
    plt.ylabel("Pic mémoire")
    plt.title(f"{algo} — Pic mémoire vs nombre de mots distincts")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

def main():
    nomDossier = "performances"
    performances = {}

    for f in os.listdir(nomDossier):
        if not f.endswith(".txt"):
            continue
        
        algo = extraireNomAlgo(f)
        chemin = os.path.join(nomDossier, f)
        perf = lirePerfs(chemin)

        if algo not in performances:
            performances[algo] = []
        performances[algo].append(perf)

    print(performances)
    
    for algo in performances:
        analyseAlgo(performances, algo)
        
#     # Courbe Temps vs nb_mots_total
#     plt.figure(figsize=(8, 5))
#     for algo, data in performances.items():
#         x = [d["nbTotal"] for d in data]
#         y = [d["temps_ms"] for d in data]
#         plt.plot(x, y, marker="o", label=algo)
#     plt.xlabel("Nombre de mots")
#     plt.ylabel("Temps (ms)")
#     plt.title("Comparaison des algorithmes — Temps")
#     plt.legend()
#     plt.grid(True)
#     plt.tight_layout()
#     plt.show()
# 
#     # Courbe Pic mémoire vs nb_mots_total
#     plt.figure(figsize=(8, 5))
#     for algo, data in performances.items():
#         x = [d["nbTotal"] for d in data]
#         y = [d["pic"] for d in data]
#         plt.plot(x, y, marker="o", label=algo)
#     plt.xlabel("Nombre de mots")
#     plt.ylabel("Pic mémoire")
#     plt.title("Comparaison des algorithmes — Mémoire")
#     plt.legend()
#     plt.grid(True)
#     plt.tight_layout()
#     plt.show()
    
if __name__ == "__main__":
    main()
