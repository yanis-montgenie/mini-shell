# Mini-Shell Documentation

## Description

Le mini-shell est une application simple implémentée en langage C qui simule un shell Unix de base.
Il offre plusieurs fonctionnalités de base, notamment l'exécution de commandes,
la gestion des commandes intégrées, la manipulation des redirections, la gestion des variables d'environnement, etc.

## Structure du Projet

Le projet est organisé selon la structure suivante :

- src/: Contient les fichiers source du projet.
- doc/: Contient la documentation du projet.
- exemples: Fichier contenant des exemples de commandes testées avec succès dans le mini-shell.
- Makefile: Fichier permettant de compiler le projet.

## Compilation

Pour compiler le projet, utilisez la commande suivante :
make
Cela générera l'exécutable minishell.

Utilisation

Pour exécuter le mini-shell, utilisez la commande suivante :
./minishell

Fonctionnalités

Le mini-shell offre les fonctionnalités suivantes :

Commandes Intégrées :
$ cd [dir]: Change le répertoire courant.
$ exit [n]: Termine le mini-shell avec le code de retour n.
$ export VAR="ma variable" : Change le contenu de la variable d'environnement.
$ unset VAR : Efface le contenu de la variable d'environnement.

Exécution de Commandes :
$ commande: Exécute une commande en avant-plan.
$ commande &: Exécute une commande en arrière-plan.
$ commande1 ; commande2 ; commande3 & commande4 ; ...: Exécute une séquence de commandes.

Redirections :
$ commande > file.output: Redirige la sortie standard vers un fichier.
... (Ajoutez d'autres redirections selon vos implémentations)
Variables d'Environnement :
$ export VAR="ma variable": Définit une variable d'environnement.
$ echo $VAR: Affiche la valeur d'une variable.
$ unset VAR: Supprime une variable d'environnement.

Opérateurs :
!, &&, ||: Gère les opérateurs logiques comme le bash.
Bonus :
Historique
Pour afficher l'historique taper la commande : history.
Et pour exécuter une commande de l'historique taper la commande : ! N (avec N le numéro de la commande dans l'historique).

Consultez le fichier exemples pour des exemples de commandes testées avec succès dans le mini-shell.
