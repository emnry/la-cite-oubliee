# Projet C : La cité oubliée

**Description :**
*La cité oubliée* est un jeu d’aventure textuel en C où le joueur explore une carte, interagit avec des PNJs, collecte des objets et affronte des ennemis pour atteindre le duel final.

---

## 📜 Éléments du jeu

### La carte

Le joueur peut afficher la carte avec la commande :

```bash
carte
```

Elle montre une grille de caractères représentant les lieux créés. Exemple :

**Carte vide :**

```
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
```

**Carte avec lieux :**

```
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~V~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~G~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~
```

* **V** : Village
* **G** : Grotte

**Commandes administrateur pour la carte :**

* Ajouter un lieu :

```bash
carte +
<x> <y> <initiale du lieu>
```

* Retirer un lieu (remettre la case à `~`) :

```bash
carte -
<x> <y>
```

---

### Les PNJs

* Alliés ou ennemis.
* Interactions :

  * **Échange** : vous devez posséder l’objet demandé par le PNJ.
  * **Combat** : la valeur des objets détermine les dégâts infligés.
    ⚠️ Fautes de frappe = perte d’avantage au combat.

---

### Les objets

* Servent à compléter des conditions de lieux, comme monnaie d’échange ou comme arme.

---

### Les lieux

Chaque lieu peut contenir :

* Objets
* PNJs
* Conditions d’accès (ex : objet requis pour entrer)

---

## 🎯 Objectif

Le joueur commence dans un lieu prédéfini et doit :

1. Collecter des objets.
2. Interagir avec les PNJs.
3. Explorer la carte.
4. Se préparer pour le duel final.

---

## 🎮 Comment y jouer

### Commandes du joueur

* `aller <lieu>` : se déplacer
* `examiner <objet>` : examiner un objet
* `prendre <objet>` : prendre un objet
* `lacher <objet>` : déposer un objet
* `echanger <PNJ>` : échanger avec un PNJ
* `etat` : état du personnage
* `carte` : afficher la carte
* `quitter` : quitter le jeu

### Histoire (exemple)

1. Village : récupérer **torche** et **dague**
2. Grotte : récupérer **pierre** (requiert la torche)
3. Forêt : échanger la pierre contre une **épée** avec un PNJ neutre
4. Château : affronter le boss pour progresser vers la place

---

## 💾 Sauvegarde

* Sauvegarde via commande dans un fichier `save.txt`
* Commandes de sauvegarde : `sauvegarder`, `charger`

---

## 🛠️ Mode Administrateur

Permet de créer et configurer le jeu avant de lancer la partie.

### Commandes générales

* `quitter` : quitter le mode admin
* `inventaire` : afficher l’inventaire
* `jouer` : lancer le jeu
* `aide` : afficher cette aide
* `carte` : afficher la carte actuelle
* `sauvegarder` / `charger` : sauvegarde et chargement de la partie

### Commandes de gameplay

* Déplacer le joueur : `aller <lieu>`
* Téléportation : `admin.aller <lieu>`
* Prendre / Lâcher objet : `prendre <objet>` / `lacher <objet>`

### Création / Affichage de tout les éléments / suppression

* **Objets** : `objet +` / `objet afficher` / `objet -`
* **Lieux** : `lieu +` / `lieu afficher` / `lieu -`
* **NPCs** : `npc +` / `npc afficher` / `npc -`

### Inventaire du joueur

* Ajouter : `inventaire + <objet>`
* Retirer : `inventaire - <objet>`

### Liaison de lieux

```bash
lieu lier <lieu1> <lieu2>
```

### Lieux et PNJs

* Ajouter un objet à un lieu : `lieu objet + <objet> <lieu>`
* Retirer : `lieu objet - <objet> <lieu>`
* Affecter un NPC à un lieu : `lieu npc + <npc> <lieu>`
* Définir condition d’accès : `lieu condition + <objet> <lieu>`

### PNJs et objets

* Donner un objet : `npc objet + <objet> <npc>`
* Retirer un objet : `npc objet - <objet> <npc>`
* Définir condition d’échange : `npc condition + <objet> <npc>`

---

### Carte et lieux

* Ajouter lieu sur la carte : `carte + <x> <y> <initiale lieu>`
* Modifier case : `carte - <x> <y>`
