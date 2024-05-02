/*
  Projet minishell - Licence 3 Info - PSI 2023
 
 Nom : MONTGENIE - FRANCOIS
 Prénom : Yanis - Thibaut
 Num. étudiant : 22101878 - 22110629
 Groupe de projet : 11
 Date : 12 octobre 2023
 
  Gestion des commandes internes du minishell (implémentation).
 
 */

/*
   Fichier builtin.c : Fichier contenant les built-in du minishell, une fonction vérifiant si une chaîne de caractères est une commande (is_builtin)
   et une fonction permettant la bonne initialisation de la structure cmd_t selon la commande identifiée
   Auteur : MONTGENIE Yanis FRANCOIS Thibaut
   Dépendances : cmd.h stdlib.h stdio.h string.h
*/

#include "cmd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *commande[] = {"cd", "export", "unset", "exit", NULL};

/*
      Fonction is_builtin
      @param cmd : chaîne de cacractère permettant de déterminer si c'est une commande du mini-shell ou non
      @return un entier, (code de retour) 1 si la chaîne de caractère est une commande du mini-shell, 0 sinon
*/
int is_builtin(const char* cmd) {
    for (const char**tab = commande; *tab != NULL; tab++)
        if (strcmp(cmd, *tab) == 0) return 1; // cmd est une commande gérée par le mini-shell
    
    return 0; // cmd n'est pas gérée par le mini-shell
}

/*
      Fonction cd
      @param path : chaîne de caractères permettant le changement de dossier courant
      @param fderr : entier indiquant dans quel descripteur de fichier il faut écrire le message d'erreur
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int cd(const char* path, int fderr) {
    char work_directory[1024];

    if (chdir(path)==-1){   // Changement du dossier courrant
        write(fderr, "Dossier inexistant\n", strlen("Dossier inexistant\n"));
        return -1;
    }
    
    getcwd(work_directory, 1024);  // Récupération du cwd, stocké dans work_directory
    setenv("PWD", work_directory, 1);  // Modification de la variable d'environnement PWD
    
    return 0;
}

/*
      Fonction export : Modification de la valeur d'une variable d'environnement
      @param var : chaîne de caractères permettant l'identification de la variable d'environnement
      @param value : chaîne de caractères permettant le changement de la valeur de la variable d'environnement
      @param fderr : entier indiquant dans quel descripteur de fichier il faut écrire le message d'erreur
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int export(const char* var, const char* value, int fderr) {
    // Utilisation de variables temporaires pour éviter les erreurs suite à des possibles modifications des const char
    char var2[1024]; strcpy(var2, var);
    char value2[1024]; strcpy(value2, value);

    // Modification de la variable d'environnement
    if(setenv(var2, value2, 1) == -1) {
        write(fderr, "Problème lors de l'exportation de la variable d'environnement\n", strlen("Problème lors de modification de la variable d'environnement\n"));
        return -1;
    }

    return 0;
}

/*
      Fonction unset : Suppresion de la valeur d'une variable d'environnement
      @param var : chaîne de caractères permettant l'identification de la variable d'environnement
      @param fderr : entier indiquant dans quel descripteur de fichier il faut écrire le message d'erreur
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int unset(const char* var, int fderr){
    // Suppression de la variable d'environnement
    if(unsetenv(var)==-1){
        write(fderr, "Problème lors de la suppression de la variable d'environnement\n", strlen("Problème lors de la suppression de la variable d'environnement\n"));
        return -1;
    }
    
    return 0;
}

/*
      Fonction exit_shell
      @param ret : entier indiquant le code retour à retourner lors de la fermeture du mini-shell
      @param fderr : entier indiquant dans quel descripteur de fichier il faut écrire le message d'erreur
      @return un entier, -1 si la fonction a échoué
*/
int exit_shell(int ret, int fderr) {
    exit(ret);  // Fermeture du processus en cours

    write(fderr, "Erreur lors de la fermeture du terminal\n", strlen("Erreur lors de la fermeture du terminal\n"));
    return -1;
}

/*
      Fonction builtin
      @param cmd : structure cmd_t permettant l'exécution d'une commande du mini-shell
      @return un entier, (code de retour) 0 si l'exécution d'une commande a réussi, -1 sinon
*/
int builtin(cmd_t* cmd) {
    if(strcmp(cmd->path, "cd") == 0) cd(cmd->argv[1], cmd->stderr);  // Exécution de la fonction cd
    if(strcmp(cmd->path, "export") == 0) {
        char *tokens[MAX_CMD_SIZE];
        // Séparation de la chaîne avec la fonction strcut
        if(strcut_s(cmd->argv[1], '=', tokens, MAX_CMD_SIZE) != 2){
            fprintf(stderr, "Erreur trop d'arguments fonction export\n");
            return -1;
        }
        // Exécution du export
        export(tokens[0], tokens[1], cmd->stderr);
    }
    if(strcmp(cmd->path, "unset") == 0) unset(cmd->argv[1], cmd->stderr);  // Exécution de la fonction unset
    if(strcmp(cmd->path, "exit") == 0) exit_shell(0, cmd->stderr);  // Exécution de la fonction exit
    
    return 0;
}
