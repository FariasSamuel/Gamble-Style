# Rapport — GambleStyle

> Les diagrammes UML ci-dessous sont rédigés en syntaxe **PlantUML**.
> Pour les visualiser : [plantuml.com/plantuml](https://www.plantuml.com/plantuml/uml)
> ou VS Code avec l'extension *PlantUML* (raccourci `Alt+D`).

---

## 1. Guide d'utilisation complet

### 1.1 Démarrage

Lancez le jeu depuis la racine du projet :

```bash
./bin/gamble_run
```

Un écran de configuration s'affiche.

### 1.2 Écran de configuration

| Champ | Description |
|---|---|
| Nombre de joueurs | 2 à 4. Utilisez les boutons `−` / `+`. |
| Noms | Cliquez sur un champ et tapez le nom (Entrée valide). |
| Durée | Durée maximale en minutes (0 = illimitée). |
| Bouton Jouer | Lance la partie après avoir déterminé l'ordre de jeu par les dés. |

L'ordre de jeu est déterminé automatiquement : chaque joueur lance un dé, celui
avec le plus grand résultat joue en premier.

### 1.3 Plateau de jeu

L'interface est divisée en trois zones :

- **Plateau (gauche)** : grille 4×4 avec les cases, les pions des joueurs et les dés.
- **Panneau (droite)** : état de chaque joueur (capital, propriétés, cartes en main)
  et le bouton **Lancer le dé**.
- **Journal (bas)** : derniers événements de la partie.

### 1.4 Déroulement d'un tour

1. Le joueur actif clique sur **Lancer le dé**.
   - Si le joueur est **en prison**, un prompt s'affiche pour utiliser
     la carte Sortie de Prison ou lancer les dés normalement.
2. Les deux dés sont lancés ; le pion se déplace automatiquement.
3. Après 1 seconde, l'effet de la case est résolu (prompt ou animation).
4. Si le joueur fait un **double**, il rejoue automatiquement.
   Trois doubles consécutifs → prison immédiate.
5. Le tour passe au joueur suivant.

### 1.5 Cases et effets détaillés

#### Case Départ
- Verse **200 €** (configurable dans `board.txt`) lorsque le joueur atterrit
  dessus ou la survole.

#### Case Propriété
| Situation | Effet |
|---|---|
| Non achetée | Prompt "Acheter pour X € ?" — Oui ou Non. |
| Propriétaire = joueur actif | Rien. |
| Propriétaire = adversaire | Le propriétaire choisit une mise (50/100/150/200 €). Un duel Tron est lancé. |

**Mise** : le joueur visiteur peut activer sa carte **Double Gain** avant le duel
pour percevoir 2× la mise en cas de victoire.

#### Case Événement
Tous les joueurs actifs s'affrontent dans **DiceBattle**.
Chaque perdant verse sa mise au gagnant.

#### Case Aléatoire
Une carte est piochée dans le deck tournant. Un overlay s'affiche avec l'image de
la carte et une explication. Cliquer **Continuer** (ou `Espace`/`Entrée`) ferme l'overlay.

| Carte | Effet immédiat |
|---|---|
| **Anniversaire** | Chaque adversaire verse X € au joueur (X = paramètre dans `board.txt`). |
| **Argent** | Tirage 50/50 : +X € (gain) ou −X € (perte). |
| **Déplacement** | Avance de 2 à 6 cases aléatoirement. Si passage par le Départ : +200 €. |
| **Double Gain** | Carte conservée en main. Double le gain si utilisée avant un duel de propriété. |
| **Sortie de Prison** | Carte conservée en main. Libère instantanément sans lancer de double. |

#### Station Prison (visite)
Un joueur qui atterrit sur la case par ses dés est simplement en visite — aucun effet.

#### Prison (emprisonné)
Un joueur envoyé en prison par la case "Aller en Prison" ou par triple double :

- Il doit lancer un **double** pour sortir (1 essai par tour).
- Alternativement, il peut utiliser sa **carte Sortie de Prison** avant de lancer.
- Après **3 tours** sans double, il est libéré automatiquement.

#### Aller en Prison
Téléporte immédiatement le joueur vers la Station Prison et démarre le compteur.

### 1.6 Conditions de fin de partie

La partie se termine dans l'un de ces cas :

- Il ne reste qu'**un seul joueur solvable** (capital > 0 ou propriétés non hypothéquées).
- Le **temps configuré** est écoulé.

**Banqueroute** : si le capital d'un joueur passe en dessous de 0, il est en banqueroute.
Il peut appeler `misebanqueroute()` pour liquider ses propriétés à mi-prix.
Si le capital reste ≤ 0 après liquidation → **Faillite** : le joueur est éliminé.

Le **vainqueur** est le joueur avec le score le plus élevé (capital + valeur des propriétés).

---

## 2. Diagramme de classes UML

```plantuml
@startuml GambleStyle_Classes

skinparam classAttributeIconSize 0
skinparam linetype ortho
hide empty members

' ─── Plateau ──────────────────────────────────────────────────────────────────
class Plateau {
    # joueurs : vector<Joueur*>
    # cases   : vector<Case*>
    --
    + ajouterJoueur(Joueur*)
    + getJoueur(idx) : Joueur*
    + addCase(Case*)
    + getCase(idx) : Case*
    + ordre()
    + donner_capital(Joueur*, int)
    + enlever_capital(Joueur*, int)
    + transfert_argent(Joueur*, Joueur*, int)
    + fin() : Joueur*
    + update()
    + gamelooping()
    + nbCases() : int
    + nb_joueur() : int
}

class PlateauSFML {
    - window_ : RenderWindow
    - cfg_     : BoardConfig
    - state_   : State
    --
    + initialization()
    + gamelooping()
    - buildGame()
    - doTurn()
    - resolveAction()
    - advanceTurn()
    - renderBoard()
    - renderPanel()
    - renderLog()
    - renderBuyPrompt()
    - renderGamblePrompt()
    - renderCarteDrawnPrompt()
    - renderUseSortiePrisonPrompt()
    - renderUseDoubleGainPrompt()
    - renderGameOver()
    - prepareCarteDisplay(Carte*, Joueur*, int, int, int)
}

Plateau <|-- PlateauSFML

' ─── Joueur ───────────────────────────────────────────────────────────────────
class Joueur {
    - nom           : string
    - capital       : int
    - condition_    : Condition
    - position      : Case*
    - proprietes    : vector<CasePropriete*>
    - cartes        : vector<Carte*>
    - compteur_double : int
    - cpt_prison_   : int
    --
    + lancerde() : int
    + bougerjoueur(Case*)
    + acheter(CasePropriete*)
    + hypotequer(CasePropriete*)
    + misebanqueroute()
    + conditionfinanciere() : Condition
    + donnerCarte(Carte*)
    + retirerCarte(Carte*)
    + getSortiePrison() : CarteSortiePrison*
    + getDoubleGain()   : CarteDoubleGain*
    + tour(cases, prisonIdx)
}

enum Condition {
    RICHE
    BANQUEROUTE
    FAILLITE
}

Joueur --> Condition

' ─── Case ─────────────────────────────────────────────────────────────────────
abstract class Case {
    - num_case     : int
    - joueur_actif : Joueur*
    --
    + action()
    + setJoueurActif(Joueur*)
    + getJoueurActif() : Joueur*
    + get_num_case() : int
}

class CaseDepart {
    - montant : int
    --
    + action()
    + getMontant() : int
}

class CasePropriete {
    - prix_achat   : int
    - achetable    : bool
    - proprietaire : Joueur*
    - command_     : string
    --
    + action()
    + confirmerMise(int)
    + repartition(Joueur*, int)
    + achatPropose()    : bool
    + gambleEnAttente() : bool
    + getGagnantDernierJeu() : Joueur*
}

class CaseEvenement {
    - mise     : int
    - gagnant  : Joueur*
    - command_ : string
    --
    + action()
    + ajouterJoueur(Joueur*)
    + distribution()
}

class CasePrison {
    - tours_ : map<Joueur*, int>
    --
    + action()
    + forcerResultatDes(int, int)
}

class CaseGotoPrison {
    - prison_ : CasePrison*
    --
    + action()
}

class CaseAleatoire {
    - file : deque<Carte*>
    --
    + action()
    + ajouterCarte(Carte*)
    + getPremiereCarteDeFile() : Carte*
}

Case <|-- CaseDepart
Case <|-- CasePropriete
Case <|-- CaseEvenement
Case <|-- CasePrison
Case <|-- CaseGotoPrison
Case <|-- CaseAleatoire

' ─── Carte ────────────────────────────────────────────────────────────────────
abstract class Carte {
    - nom_carte : string
    - text      : string
    --
    + action()
    + setTitulaire(Joueur*)
}

class CarteAnniversaire {
    - titulaire_ : Joueur*
    - plateau_   : Plateau*
    - montant_   : int
    --
    + action()
    + setTitulaire(Joueur*)
}

class CarteArgent {
    - titulaire_ : Joueur*
    - montant_   : int
    --
    + action()
    + setTitulaire(Joueur*)
}

class CarteDeplacement {
    - titulaire_  : Joueur*
    - plateau_    : Plateau*
    - caseTarget_ : Case*
    - minSteps_   : int
    - maxSteps_   : int
    --
    + action()
    + setTitulaire(Joueur*)
}

class CarteDoubleGain {
    - titulaire_ : Joueur*
    - active_    : bool
    + {static} ID : int = 2
    --
    + action()
    + consommer()
    + estActive() : bool
}

class CarteSortiePrison {
    - titulaire_ : Joueur*
    + {static} ID : int = 1
    --
    + action()
    + setTitulaire(Joueur*)
}

Carte <|-- CarteAnniversaire
Carte <|-- CarteArgent
Carte <|-- CarteDeplacement
Carte <|-- CarteDoubleGain
Carte <|-- CarteSortiePrison

' ─── BoardConfig ──────────────────────────────────────────────────────────────
class BoardConfig {
    + grid_size   : int
    + board_image : string
    + dark_coeff  : float
    + cases       : vector<CaseConfig>
    --
    + {static} loadFromFile(path) : BoardConfig
    + nbCases() : int
    + caseNormPos(idx) : pair<float,float>
    + findCaseType(CaseType) : int
}

' ─── Relations ────────────────────────────────────────────────────────────────
Plateau "1" *-- "0..*" Case       : possède >
Plateau "1" o-- "0..4" Joueur     : enregistre >
PlateauSFML --> BoardConfig        : lit >
CaseAleatoire "1" *-- "1..*" Carte : possède >
Joueur "0..4" --> "0..*" CasePropriete : proprietes >
Joueur "0..4" --> "0..*" Carte         : cartes >
CaseGotoPrison --> CasePrison          : téléporte vers >
CasePropriete --> Plateau              : transfert_argent >
CarteAnniversaire --> Plateau          : itère joueurs >

@enduml
```

---

## 3. Diagrammes de séquence

### SD1 — Tour complet

```plantuml
@startuml SD1_Tour_Complet

actor Joueur as J
participant PlateauSFML as P
participant "Case\ncourante" as C
participant Joueur_obj as JO

J -> P : clic "Lancer le dé"
activate P

P -> P : doTurn()
P -> JO : lancerde() x2
JO --> P : d1, d2

P -> P : calcul nouvelle position\n(passage départ → +200 €)
P -> JO : setCaseActuelle(newCase)
P -> P : state_ = MOVING\nactionClock_.restart()

note over P : délai 1 seconde

P -> P : resolveAction()
P -> C : setJoueurActif(joueur)
P -> C : action()
C --> P : flags (achatPropose / gambleEnAttente / …)

alt case propriété non achetée
    P -> P : state_ = BUY_PROMPT
else case propriété adverse
    P -> P : state_ = GAMBLE_PROMPT
else case aléatoire
    P -> P : prepareCarteDisplay()\nstate_ = CARTE_DRAWN_PROMPT
else autre case
    P -> P : advanceTurn()
end

deactivate P

@enduml
```

---

### SD2 — Achat de propriété

```plantuml
@startuml SD2_Achat_Propriete

actor Joueur as J
participant PlateauSFML as P
participant CasePropriete as CP
participant Joueur_obj as JO

P -> CP : action()
CP --> P : achatPropose() = true
P -> P : state_ = BUY_PROMPT
P --> J : affiche prompt "Acheter X € ?"

J -> P : clic "Oui"
P -> JO : acheter(CasePropriete*)
JO -> JO : capital -= prix_achat
JO -> CP : setProprietaire(this)
P -> P : addMsg("Joueur achète …")
P -> P : advanceTurn()

@enduml
```

---

### SD3 — Jeu de propriété (Tron)

```plantuml
@startuml SD3_Jeu_Propriete

actor Visiteur as V
actor Proprietaire as PR
participant PlateauSFML as P
participant CasePropriete as CP

P -> CP : action()
CP --> P : gambleEnAttente() = true
P -> P : state_ = GAMBLE_PROMPT
P --> PR : affiche prompt sélection mise

PR -> P : clic mise (ex. 100 €)
P --> V : affiche "Confirmer"
V -> P : clic "Confirmer"

P -> CP : confirmerMise(100)
CP -> CP : lancerJeu(command_, visiteur, proprio)\n→ popen("./bin/Tron …")

note over CP : mini-jeu Tron s'ouvre\nen plein écran

CP --> P : gagnant = visiteur | proprio | nullptr

CP -> CP : repartition(gagnant, 100)
CP -> Joueur : transfert capital

P -> P : addMsg("X gagne ! (mise 100€)")
P -> P : advanceTurn()

@enduml
```

---

### SD4 — Carte aléatoire

```plantuml
@startuml SD4_Carte_Aleatoire

actor Joueur as J
participant PlateauSFML as P
participant CaseAleatoire as CA
participant Carte as C

P -> CA : setJoueurActif(joueur)
P -> CA : getPremiereCarteDeFile()
CA --> P : drawnCard (pour l'overlay)

P -> CA : action()
CA -> CA : rotation FIFO\n(pop_front / push_back)
CA -> C : setTitulaire(joueur)
CA -> C : action()
C -> C : applique effet sur joueur

P -> P : prepareCarteDisplay(drawnCard, …)
P -> P : state_ = CARTE_DRAWN_PROMPT
P --> J : overlay : image carte + explication

J -> P : clic "Continuer" ou Espace
P -> P : state_ = PLAYING
P -> P : advanceTurn()

@enduml
```

---

### SD5 — Prison

```plantuml
@startuml SD5_Prison

actor Joueur as J
participant PlateauSFML as P
participant CaseGotoPrison as CGP
participant CasePrison as CP
participant Joueur_obj as JO

' ── Entrée en prison ──────────────────────────────────────────────────────────
J -> P : atterrit sur "Aller en Prison"
P -> CGP : action()
CGP -> JO : bougerjoueur(casePrison)
P -> JO : setCptPrison(1)
P -> P : addMsg("Joueur envoyé en Prison !")

' ── Tour suivant : joueur en prison ───────────────────────────────────────────
J -> P : clic "Lancer le dé"

alt possède CarteSortiePrison
    P -> P : state_ = USE_SORTIE_PRISON_PROMPT
    P --> J : "Utiliser la carte ?"
    J -> P : clic "Utiliser"
    P -> JO : setCptPrison(0)
    P -> JO : retirerCarte(carteSortiePrison)
    P -> P : doTurn() → lancer dés normalement
else
    P -> CP : action()
    CP -> JO : lancerde() x2

    alt double obtenu
        CP -> CP : tours_[j] = 0\n(libéré)
        P -> JO : setCptPrison(0)
        P -> P : addMsg("Joueur sort de prison !")
    else compteur < 3
        CP -> CP : tours_[j]++
        P -> JO : setCptPrison(cpt + 1)
    else 3 tours écoulés
        CP -> CP : tours_[j] = 0\n(libération automatique)
        P -> JO : setCptPrison(0)
    end
end

@enduml
```
