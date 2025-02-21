# Projet IA - Implémentation d'IA pour un jeu en C++ avec SFML

## 1. Introduction
### 1.1. Présentation du projet
Projet réalisé en 8 jours dans le cadre de notre enseignement.

### 1.2. Objectifs et ambitions
Les objectifs étaient de développer un système d'intelligence artificielle avancé pour un jeu en C++ avec SFML, en intégrant des techniques comme le pathfinding A*, les machines à états finis (FSM), les arbres de comportement (Behaviour Trees) et le Goal-Oriented Action Planning (GOAP).

### 1.3. Public cible
Tout public

### 1.4. Plateformes et technologies utilisées
* Microsoft Visual Studio
* C++17
* SFML (Simple and Fast Multimedia Library)

## 2. Concept du projet
### 2.1. Synopsis et objectifs
L'objectif est d'implémenter différentes techniques d'IA pour améliorer le comportement des entités ennemies et alliées dans le jeu.

### 2.2. Approches IA utilisées
- **Pathfinding A*** : Déplacement intelligent des ennemis.
- **FSM (Finite State Machine)** : Comportements basiques des ennemis.
- **Behaviour Trees** : Système de prise de décision avancé.
- **GOAP (Goal-Oriented Action Planning)** : Gestion dynamique des actions.

### 2.3. Mécaniques IA principales
- Déplacements optimisés et poursuite du joueur.
- Détection et alerte entre ennemis.
- Réactions des alliés face aux menaces.

## 3. Implémentation technique
### 3.1. Systèmes IA intégrés
Chaque ennemi et allié suit un ensemble de règles IA spécifiques :
- **Patrouilleur** : Suit un chemin prédéfini et réagit aux intrusions.
- **Chien de garde** : Utilise A* pour poursuivre le joueur s'il est repéré.
- **Caméra de surveillance** : Détecte le joueur et alerte les autres ennemis.
- **Allié** : Suit un arbre de comportement pour assister le joueur.

### 3.2. Gestion des entités et interactions
- Un **Entity Manager** centralise la mise à jour et le rendu des entités IA.
- Les entités ennemies et alliées communiquent entre elles pour adapter leur comportement.

### 3.3. Interface utilisateur et debug
- Indicateurs visuels pour la détection des ennemis.
- Mode debug pour afficher les chemins générés par A*.

## 4. Graphismes et Direction Artistique
### 4.1. Style visuel et animations
Utilisation de textures pour représenter les personnages et les ennemis, animés avec des sprite sheets.

### 4.2. Affichage des états IA
Les ennemis changent de couleur ou d'apparence en fonction de leur état.

## 5. Sound Design
### 5.1. Ambiance sonore
Ajout de sons d'alerte et d'effets sonores pour les interactions IA. (en cour)

## 6. Gestion de Projet et Planning
### 6.1. Méthodologie de gestion
Travail collaboratif avec une répartition des tâches par fonctionnalités IA.

### 6.2. Répartition des tâches et rôles
- **Pathfinding A*** : Développement du système de navigation.
- **FSM et Behaviour Trees** : Conception des comportements ennemis.
- **GOAP** : Implémentation des décisions stratégiques.
- **Debug et optimisations** : Amélioration des performances IA.

### 6.3. Planning et échéances
Projet réalisé en une semaine avec un objectif de prototype fonctionnel.

## 7. Conclusion et Perspectives
### 7.1. Résumé des résultats obtenus
Nous avons implémenté avec succès plusieurs techniques IA avancées dans un environnement de jeu.

### 7.2. Améliorations futures et mises à jour
- Optimisation des performances IA.
- Ajout d'un système d'apprentissage dynamique pour les ennemis.
- Amélioration des interactions entre alliés et ennemis.
- Mise en fonction de l'allié qui est inutile pour le moment

