Vorassic Park (Server)
=============

Projet *Vorassic Park* réalisé dans le cadre de la matière *Algorithmique et structures de données*, durant le second semestre de 1ère année (1i) 2013-2014 à l'*École Supérieure de Génie Informatique* (ESGI).

Ce dépôt contient le programme serveur permettant l'hébergement de parties **Vorassic Park**.

Mapping
------------

Les fichiers .map sont situés dans le répertoire *Maps*.

Un fichier MAP est constitué comme suit : 

1. La taille X de la grille de jeu.
2. La taille Y de la grille de jeu.
3. Le chemin local vers le fichier de musique joué durant la partie.
4. Le chemin local vers le fichier image servant de fond à la map.
5. Une grille X*Y composée de différentes valeurs définissant le contenu et l'effet des cases.

Une case peut avoir les valeurs suivantes : 

* -37 : vide (innaccessible)
* -36 : vide (accessible)
* -35 : spawn j4
* -34 : spawn j3
* -33 : spawn j2
* -32 : spawn j1
* -31 : mur
* -30 à 30 : case à points
* 31 : x2 score
* 32 : vol de X points à un joueur choisi (X étant un nombre aléatoire compris entre 0 et 30)
* 33 : prochain tour sauté
* 34 : bloque un joueur au choix au tour suivant
* 35 : échange position avec un joueur choisi
* 36 : coup supplémentaire
* 37 : échange des scores de manière aléatoire

Bibliothèques
------------

* [SDL 1.2](https://www.libsdl.org/)
* [SDL_net 1.2](https://www.libsdl.org/projects/SDL_net/release-1.2.html)

Équipe
------------
* Aurélien Duval
* [Valentin Fries](http://fries.io)