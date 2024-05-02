/*
  Projet minishell - Licence 3 Info - PSI 2023
 
 Nom : MONTGENIE - FRANCOIS
 Prénom : Yanis - Thibaut
 Num. étudiant : 22101878 - 22110629
 Groupe de projet : 11
 Date : 12 octobre 2023
 
  Gestion des processus.
 
*/

#ifndef _PROCESSUS_H
#define _PROCESSUS_H

#include <unistd.h>
#include <stdio.h>
#include "parser.h"

typedef struct cmd_t {
  pid_t pid;
  int status;
  int stdin, stdout, stderr;
  int wait;
  char* path;
  char* argv[MAX_CMD_SIZE];
  int fdclose[MAX_CMD_SIZE];
  struct cmd_t* next;
  struct cmd_t* next_success;
  struct cmd_t* next_failure;
} cmd_t;
/*
  Structure de représentation d'une commande à lancer.

  pid:
  status:
  stdin, stdout, stderr:
  wait:
  fdclose:
  next:
  next_success:
  next_failure:
*/

void add_fd(int* fdclose, int fd);
void merge_fdclose(int* fdclose1, int* fdclose2);

int exec_cmd(cmd_t* p);
/*
  Lancer la commande en fonction des attributs de
  la structure et initialiser les champs manquants.
 
  On crée un nouveau processus, on détourne
  éventuellement les entrée/sorties.
  On conserve le PID dans la structure du processus
  appelant (le minishell).
  On attend si la commande est lancée en "avant-plan"
  et on initialise le code de retour.
  On rend la main immédiatement sinon.
 
  p : un pointeur sur la structure contenant les
      informations pour l'exécution de la commande.
 
  Retourne 0 ou un code d'erreur.
*/

int init_cmd(cmd_t* p);
/*
  Initialiser une structure cmd_t avec les
  valeurs par défaut.
 
  p : un pointeur sur la structure à initialiser
 
  Retourne 0 ou un code d'erreur.
*/

int parse_cmd(char* tokens[], cmd_t* cmds, size_t max);
/*
  Remplit le tableau de commandes en fonction du contenu
  de tokens.
    Ex : {"ls", "-l", "|", "grep", "^a", NULL} =>
         {{path = "ls",
           argv = {"ls", "-l", NULL},
           bg = 1,
           ...},
          {path = "grep",
           argv = {"grep", "^a", NULL},
           bg = 0,
           ...}}
  tokens : le tableau des éléments de la ligne de
           commandes (peut être modifié)
  cmds : le tableau dans lequel sont stockés les
         différentes structures représentant le
         commandes.
  max : le nombre maximum de commandes dans le tableau

  Retourne 0 ou un code d'erreur.
*/

typedef struct cmd_history_entry_t {
    int number; // Numéro de la commande
    char command[MAX_LINE_SIZE];
    struct cmd_history_entry_t* next;
} cmd_history_entry_t;

/*
  Structure de représentation d'une commande dans l'historique

  number:
  command:
  next:
*/

typedef struct cmd_history_t {
    cmd_history_entry_t* head;
    int size;
    int last_command_number; // Compteur pour suivre le numéro de la dernière commande
} cmd_history_t;
/*
  Structure de représentation de l'historique

  head:
  size:
  last_command_number:
*/

void init_history(cmd_history_t* history);
// Initialise un tableau d'historique

void add_to_history(cmd_history_t* history, const char* command);
// Ajoute une commande à l'historique

void print_history(cmd_history_t* history);
// Affiche l'historique

void free_history(cmd_history_t* history);
// Libère l'espace mémoire allouée à l'historique

cmd_history_entry_t* find_command_by_number(cmd_history_t* history, int command_number);
// Trouve une commande dans l'historique par son numéro 

#endif
