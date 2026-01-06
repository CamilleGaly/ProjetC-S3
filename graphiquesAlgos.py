import matplotlib.pyplot as plt
import os

def extraireNomAlgo(fichier):
    return fichier.split("__")[0]


def lirePerfs(fichier):
    data = {}
    temps = []

    with open(fichier, "r") as f:
        # première ligne utile
        for ligne in f:
            ligne = ligne.strip()
            if ligne:
                data["nbTotal"], data["nbDist"] = map(int, ligne.split())
                break

        # reste du fichier
        for ligne in f:
            ligne = ligne.strip()
            if ligne.startswith("Pic"):
                data["pic"] = int(ligne.split(":")[1].strip())
            elif ligne.startswith("Temps"):
                t = int(ligne.split(":")[1].strip().split()[0])
                temps.append(t)

    data["temps_moyen_ms"] = sum(temps) / len(temps)
    return data

def creePerformances(nomDossier):
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
    return performances
       
def graphiqueTempsEtNbTotal():
    nomDossier = "performancesDiffFixe"
    performances = creePerformances(nomDossier)
    
    plt.figure(figsize=(8,5))
    for algo, data in performances.items():
        # Tri par nbTotal
        data_sorted = sorted(data, key=lambda d: d["nbTotal"])
        x = [d["nbTotal"] for d in data_sorted]
        y = [d["temps_moyen_ms"] for d in data_sorted]
        plt.plot(x, y, marker="o", linewidth=2, markersize=6, label=algo)

    plt.xlabel("Nombre total de mots")
    plt.ylabel("Temps moyen (ms)")
    plt.title("Comparaison des algorithmes — Temps moyen / Nombre total de mots")
    plt.yscale("log")  # Échelle logarithmique pour mieux voir toutes les courbes
    plt.grid(True, linestyle="--", linewidth=0.5, which="both")
    plt.legend()
    plt.tight_layout()
    plt.show()
    
def graphiquePicEtNbTotal():
    nomDossier = "performancesDiffFixe"
    performances = creePerformances(nomDossier)
    
    # -----------------------------
    # Graphique : Pic mémoire moyen vs nombre total de mots
    # -----------------------------
    plt.figure(figsize=(8,5))
    for algo, data in performances.items():
        # Tri par nbTotal
        data_sorted = sorted(data, key=lambda d: d["nbTotal"])
        x = [d["nbTotal"] for d in data_sorted]   # SEULES les valeurs réelles
        y = [d["pic"] for d in data_sorted] # Moyenne par fichier
        plt.plot(x, y, marker="o", linewidth=2, markersize=6, label=algo)

    plt.xlabel("Nombre total de mots")
    plt.ylabel("Pic mémoire")
    plt.title("Comparaison des algorithmes — Pic mémoire / Nombre total de mots")
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.show()
    
def graphiqueTempsEtNbDiff():
    nomDossier = "performancesTotalFixe"
    performances = creePerformances(nomDossier)
    
    # -----------------------------
    # Graphique : Temps moyen vs nombre de mots distincts
    # -----------------------------
    plt.figure(figsize=(8, 5))

    for algo, data in performances.items():
        # Tri par nbDist pour une courbe lisible
        data_sorted = sorted(data, key=lambda d: d["nbDist"])

        x = [d["nbDist"] for d in data_sorted]        # valeurs réelles uniquement
        y = [d["temps_moyen_ms"] for d in data_sorted]   # moyennes réelles

        plt.plot(
            x, y,
            marker="o",
            linewidth=2,
            markersize=6,
            label=algo
        )

    plt.xlabel("Nombre de mots distincts")
    plt.ylabel("Temps moyen (ms)")
    plt.title("Comparaison des algorithmes — Temps moyen / Nombre de mots distincts")
    plt.yscale("log")  # TRÈS recommandé si un algo est beaucoup plus lent
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.show()
    
def graphiquePicEtNbDiff():
    nomDossier = "performancesTotalFixe"
    performances = creePerformances(nomDossier)
    
    # -----------------------------
    # Graphique : Pic mémoire vs nbDist
    # -----------------------------
    plt.figure(figsize=(8, 5))

    for algo, data in performances.items():
        data_sorted = sorted(data, key=lambda d: d["nbDist"])

        x = [d["nbDist"] for d in data_sorted]
        y = [d["pic"] for d in data_sorted]

        plt.plot(
            x, y,
            marker="o",
            linewidth=2,
            markersize=6,
            label=algo
        )

    plt.xlabel("Nombre de mots distincts")
    plt.ylabel("Pic mémoire")
    plt.title("Comparaison des algorithmes — Pic mémoire / Nombre de mots distincts")
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.show()
    
    
def main():
    graphiquePicEtNbTotal()
    graphiquePicEtNbDiff()
    graphiqueTempsEtNbTotal()
    graphiqueTempsEtNbDiff()

if __name__ == "__main__":
    main()