/*
  Projet minishell - Licence 3 Info - PSI 2023
 
  Nom : MONTGENIE - FRANCOIS
  Prénom : Yanis - Thibaut
  Num. étudiant : 22101878 - 22110629
  Groupe de projet : 11
  Date : 12 octobre 2023
 
  Interface du minishell.
 
 */

/*
   Fichier main.c : Programme principale
   Auteur : MONTGENIE Yanis FRANCOIS Thibaut
   Dépendances : stdio.h stdlib.h string.h parser.h cmd.h builtin.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "cmd.h"
#include "builtin.h"

int main(int argc, char* argv[]) {
    char cmdline[MAX_LINE_SIZE]; // buffer des lignes de commandes
    char* cmdtoks[MAX_CMD_SIZE]; // "mots" de la ligne de commandes
    cmd_t cmds[MAX_CMD_SIZE];
    cmd_t* current;
    cmd_history_t global_history;
    init_history(&global_history);
    int find=0;
    cmd_history_entry_t *c ;

    while (1) {
        // Effacer les contenus de cmdline, cmdtoks et cmds
        // Initialiser les valeurs par défaut dans cmds (stdin, stdout, stderr, ...)
        memset(cmdline, 0, MAX_LINE_SIZE);
        memset(cmdtoks, 0, MAX_CMD_SIZE);
        for(int i=0; i<MAX_CMD_SIZE; i++) init_cmd(&(cmds[i]));

        // Afficher un prompt
        if (!find) printf("$ ");

        // Lire une ligne dans cmdline - Attention fgets enregistre le \n final
        if (!find) {
            if (fgets(cmdline, MAX_LINE_SIZE, stdin) == NULL) break;
            cmdline[strlen(cmdline) - 1] = '\0';
        } else {
            if (c != NULL) {
                printf("%s\n", c->command);
                strcpy(cmdline, c->command);
                c = NULL;
            }
            else printf("Commande introuvable\n");
        }

        find = 0;

        // Traiter la ligne de commande
        if(trim(cmdline) != 0) continue;  //   - supprimer les espaces en début et en fin de ligne
        if(separate_s(cmdline, "!;|&<>", MAX_CMD_SIZE) != 0) continue;  //   - ajouter d'éventuels espaces autour de ; ! || && & ...
        if(clean(cmdline) != 0) continue;  //   - supprimer les doublons d'espaces
        if(substenv(cmdline, MAX_LINE_SIZE) != 0) continue;  //   - traiter les variables d'environnement


        if (strcmp(cmdline, "history")!=0 && (cmdline[0] != '!' && !isdigit(cmdline[2]))) add_to_history(&global_history, cmdline);

        if (cmdline[0] == '!' && isdigit(cmdline[2])) {
            int num;
            if (sscanf(cmdline + 1, "%d", &num) == 1) {
                c = find_command_by_number(&global_history, num);
                find = 1;
                continue;
            }
        }

        // Découper la ligne dans cmdtoks
        int nbmot = strcut_s(cmdline, ' ', cmdtoks, MAX_CMD_SIZE);
        if (nbmot==-1) continue;

        // Traduire la ligne en structures cmd_t dans cmds
        // Les commandes sont chaînées en fonction des séparateurs
        //   - next -> exécution inconditionnelle
        //   - next_success -> exécution si la commande précédente réussit
        //   - next_failure -> exécution si la commande précédente échoue
        if(parse_cmd(cmdtoks, cmds, nbmot) != 0) continue;

        // Exécuter les commandes dans l'ordre en fonction des opérateurs de flux
        for (current = cmds; current!=NULL; ) {
            if (strcmp(current->path, "history")==0)
                print_history(&global_history);
            else if(is_builtin(current->path)) {
                if(builtin(current)!=0) break;
            }
            else {
                if (exec_cmd(current) != 0) break;
            }

            if (current->next!=NULL)                // Exécution inconditionnelle
                current=current->next;
            else if (current->next_success!=NULL)   // La commande précdente a réussi
                current = current->next_success;
            else if (current->next_failure!=NULL)   // La commande précédente a échoué
                current = current->next_failure;
            else
                break;
        }
    }

    free_history(&global_history);
    fprintf(stderr, "\nGood bye!\n");
    return 0;
}