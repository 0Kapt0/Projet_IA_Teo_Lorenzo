# Projet_IA_Teo_Lorenzo

# Projet IA - Jeu en C++ avec SFML

## Description
Ce projet consiste en l'implémentation d'une intelligence artificielle (IA) avancée pour un jeu développé en C++ avec SFML. L'objectif est d'améliorer le comportement des ennemis grâce à des algorithmes tels que le pathfinding A*, les machines à états finis (FSM), les arbres de comportement (Behaviour Trees) et le Goal-Oriented Action Planning (GOAP).

## Fonctionnalités principales
- **Pathfinding A*** : Les ennemis utilisent l'algorithme A* pour se déplacer vers leur cible de manière efficace.
- **Machines à états finis (FSM)** : Gestion des comportements basiques des ennemis.
- **Arbres de comportement (Behaviour Trees)** : Prise de décisions complexe pour certains personnages.
- **GOAP (Goal-Oriented Action Planning)** : Système avancé permettant aux ennemis de s'adapter dynamiquement à la situation.
- **Gestion des alliés** : Ajout d'un personnage allié pouvant répondre à certaines actions.
- **Caméra de surveillance** : Détection du joueur et alerte des ennemis.
- **Mécaniques de jeu** : Mouvement du joueur, sprint, largage d'objets (cookies) pour détourner l'attention des ennemis.

## Technologies utilisées
- **Langage** : C++ (C++17 ou supérieur recommandé)
- **Bibliothèque graphique** : SFML (Simple and Fast Multimedia Library)
- **Outils recommandés** : GitHub pour le versioning, Valgrind pour le débogage

## Installation et exécution
### Prérequis
1. Installer **Visual Studio Community 2022** ou un IDE compatible avec C++
2. Installer la bibliothèque **SFML**

### Cloner le projet
```bash
git clone https://github.com/votre_nom_utilisateur/votre_repository.git
cd votre_repository
```

### Compilation sous Windows (Visual Studio)
1. Ouvrir le projet dans Visual Studio
2. Ajouter la bibliothèque SFML et configurer les dépendances
3. Compiler et exécuter le projet

### Compilation sous Linux
```bash
g++ -std=c++17 -o jeu main.cpp Player.cpp EnemyPatroller.cpp EnemyDogo.cpp CameraAI.cpp Grid.cpp EntityManager.cpp AllyAi.cpp -lsfml-graphics -lsfml-window -lsfml-system
./jeu
```

## Structure du projet
```
├── assets/                # Ressources graphiques (textures, sprites, etc.)
├── src/                   # Code source du jeu
│   ├── main.cpp           # Point d'entrée du jeu
│   ├── Player.cpp/.hpp    # Gestion du joueur
│   ├── EnemyPatroller.cpp/.hpp  # Ennemis patrouilleurs avec FSM et GOAP
│   ├── EnemyDogo.cpp/.hpp # IA de poursuite avec pathfinding A*
│   ├── CameraAI.cpp/.hpp  # IA des caméras de surveillance
│   ├── AllyAI.cpp/.hpp    # IA des alliés avec Behaviour Trees
│   ├── Grid.cpp/.hpp      # Système de grille pour la navigation IA
│   ├── EntityManager.cpp/.hpp  # Gestion des entités du jeu
├── map.txt                # Fichier de la carte du jeu
├── README.md              # Documentation du projet
```

## Comment jouer ?
- **Déplacements** : ZQSD (ou flèches directionnelles)
- **Sprint** : Maintenir **Shift** enfoncé
- **Lancer un cookie** : Appuyer sur **Espace** (peut attirer les ennemis)
- **Éviter la caméra** : Ne pas entrer dans son champ de vision sous peine d'alerter les ennemis
- **Échapper aux ennemis** : Se cacher ou les semer grâce à des obstacles

## Roadmap et Améliorations futures
- ✅ Implémentation des IA de base (Patrouilleur, Chien, Caméra)
- ✅ Ajout du pathfinding A*
- ✅ Intégration des Behaviour Trees et GOAP
- 🔜 Amélioration du comportement des alliés
- 🔜 Ajout de niveaux supplémentaires

## Auteurs
- **[Votre Nom]** - Développeur principal
- Projet réalisé dans le cadre de l'enseignement à Gaming Campus

## Licence
Ce projet est sous licence MIT - voir le fichier [LICENSE](LICENSE) pour plus de détails.

