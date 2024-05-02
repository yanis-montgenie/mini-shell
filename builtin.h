/*
  Projet minishell - Licence 3 Info - PSI 2023
 
 Nom : MONTGENIE - FRANCOIS
 Prénom : Yanis - Thibaut
 Num. étudiant : 22101878 - 22110629
 Groupe de projet : 11
 Date : 12 octobre 2023
 
  Gestion des commandes internes du minishell.
 
*/

#ifndef _BUILTIN_H
#define _BUILTIN_H

#include "cmd.h"

int is_builtin(const char* cmd);
/*
 La chaîne passée représente-t-elle une commande
 interne ?
 
 cmd : chaîne de caractères.
 
 Retourne 1 si la chaîne passée désigne une
 commande interne (cd, exit, ...)
*/

int builtin(cmd_t* cmd);
/*
  Exécute la commande interne
 
  proc : process_t dont le contenu a été initialisé
         au moment du parsing
 
  Retourne 0 si tout s'est bien passé, la valeur de
  retour de la commande ou -1 si la commande n'est
  pas reconnue.
*/

int cd(const char* path, int fderr);
/*
  Change directory : change le répertoire de travail
  courant du minishell.

  path : le chemin vers lequel déplacer le CWD
  fderr : le descripteur de la sortie d'erreur pour
          affichage éventuel (erreur du cd)
  
  Retourne le code de retour de l'appel système.
*/

int export(const char* var, const char* value, int fderr);
/*
  Ajout/modification d'une variable d'environnement.
 
  var : nom de la variable
  value : valeur à lui donner
  fderr : le descripteur de la sortie d'erreur pour
          affichage éventuel
 
  Retourne le code de retour de l'appel système.
*/

int exit_shell(int ret, int fderr);
/*
  Quitter le minishell
 
  ret : code de retour du minishell
  fdout : descripteur de la sortie standard pour
          l'affichage éventuel d'un message de sortie
 
  Retourne un code d'erreur en cas d'échec.
*/

int unset(const char* var, int fderr);
/*
 Suppression d'une variable d'environnement.

 var : nom de la variable
 fderr : le descripteur de la sortie d'erreur pour
          affichage éventuel

 Retourne le code de retour de l'appel système.
 */
#endif
