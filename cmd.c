/*
  Projet minishell - Licence 3 Info - PSI 2023
 
 Nom : MONTGENIE - FRANCOIS
 Prénom : Yanis - Thibaut
 Num. étudiant : 22101878 - 22110629
 Groupe de projet : 11
 Date : 12 octobre 2023
 
  Gestion des processus (implémentation).
 
 */

/*
   Fichier cmd.c : Fichier contenant les fonction relatives aux commandes (initialisation, exécution, parsing) ainsi que des fonctions concernant les descripteurs
   et des fonctions concernant l'historique
   Auteur : MONTGENIE Yanis FRANCOIS Thibaut
   Dépendances : cmd.h stdio.h fcntl.h unistd.h string.h sys/wait.h
*/

#include "cmd.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>


/*
      Fonction init_history : Initialisation du tableau d'historique des commandes
      @param history : tableau de cmd_history_t contenant l'historique des commandes
      Fonction sans retour
*/
void init_history(cmd_history_t* history) {
    history->head = NULL;
    history->size = 0;
    history->last_command_number = 0;
}

/*
      Fonction add_to_history : Ajout d'une commande à lhistorique des commandes
      @param history : tableau de cmd_history_t contenant l'historique des commandes
      Fonction sans retour
*/
void add_to_history(cmd_history_t* history, const char* command) {
    cmd_history_entry_t* new_entry = (cmd_history_entry_t*)malloc(sizeof(cmd_history_entry_t));
    strncpy(new_entry->command, command, MAX_LINE_SIZE - 1);
    new_entry->command[MAX_LINE_SIZE - 1] = '\0';
    new_entry->number = ++(history->last_command_number); // Incrémentez le compteur et assignez le numéro
    new_entry->next = history->head;
    history->head = new_entry;
    history->size++;
}

/*
      Fonction print_history : Affichage de l'historique des commandes
      @param history : tableau de cmd_history_t contenant l'historique des commandes
      Fonction sans retour
*/
void print_history(cmd_history_t* history) {
    printf("Historique des commandes:\n");
    cmd_history_entry_t* current = history->head;
    while (current != NULL) {
        printf("[%d] %s\n", current->number, current->command);
        current = current->next;
    }
}

/*
      Fonction free_history : Libération mémoire du tableau d'historique des commandes
      @param history : tableau de cmd_history_t contenant l'historique des commandes
      Fonction sans retour
*/
void free_history(cmd_history_t* history) {
    cmd_history_entry_t* current = history->head;
    while (current != NULL) {
        cmd_history_entry_t* next = current->next;
        free(current);
        current = next;
    }
    history->head = NULL;
    history->size = 0;
    history->last_command_number = 0;
}

/*
      Fonction find_command_by_number : Recherche d'une commande dans l'historique des commandes grâce à son numéro
      @param history : tableau de cmd_history_t contenant l'historique des commandes
      @param command_number : numéro de la commande recherchée
      Fonction sans retour
*/
cmd_history_entry_t* find_command_by_number(cmd_history_t* history, int command_number) {
    cmd_history_entry_t* current = history->head;
    while (current != NULL) {
        if (current->number == command_number) {
            return current; // La commande a été trouvée
        }
        current = current->next;
    }
    return NULL; // La commande n'a pas été trouvée
}

/*
      Fonction add_fd
      @param fdclose : tableau d'entier indiquant quels sont les descripteurs de fichiers à fermer
      @param fd : entier permettant de savoir si ce descripteur est déjà ouvert ou non dans le tableau
      Fonction sans retour
*/
void add_fd(int* fdclose, int fd) {
    int i;
    for (i = 0; fdclose[i] != -1; i++) {  // On regarde si le descripteur n'est pas déjà ouvert en parcourant le tableau
        if (fdclose[i] == fd) {
            // Le descripteur de fichier existe déjà, ne pas ajouter
            return;
        }
    }
    fdclose[i] = fd;  // On ajoute le nouveau descripteur fd à la fin du tableau
    fdclose[i + 1] = -1;  // On marque la fin du tableau
}

/*
      Fonction merge_fdclose : Fusion des tableaux permettant la bonne gestion de la fermeture des descripteurs de fichiers
      @param fdclose1 : tableau d'entier indiquant quels sont les descripteurs de fichiers à fermer
      @param fdclose2 : tableau d'entier indiquant quels sont les descripteurs de fichiers à fermer
      Fonction sans retour
*/
void merge_fdclose(int* fdclose1, int* fdclose2) {
    for (int i = 0; fdclose2[i] != -1; i++) {
        add_fd(fdclose1, fdclose2[i]);
    }
}

/*
      Fonction exec_cmd
      @param p : structure cmd_t permettant l'exécution de la commande
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int exec_cmd(cmd_t* p) {
    // On crée un nouveau processus
    if((p->pid=fork())==0) {
        // On détourne les entrée/sorties
        dup2(p->stdin, 0);
        dup2(p->stdout, 1);
        dup2(p->stderr, 2);

        // On ferme les descripteurs ajoutés dans fdclose par parse_cmd lors de la redirections de flux
        for (int * fd = p->fdclose; *fd != -1; fd++) close(*fd);

        // Exécution de la commande
        execvp(p->path, p->argv);

        // S'il y a eu une erreur lors de l'exécution, on ferme les descripteurs restants
        if (p->stdin != 0 && p->stdin != 1 && p->stdin != 2) close(p->stdin);
        if (p->stdout != 0 && p->stdout != 1 && p->stdout != 2) close(p->stdout);
        if (p->stderr != 0 && p->stderr != 1 && p->stderr != 2) close(p->stderr);

        return -1;
    }

    // On attend si la commande est lancée en "avant-plan"
    if(p->wait==1){
        waitpid(p->pid, &p->status, 0);

        if(WEXITSTATUS(p->status) != 0) p->next_success=NULL;  // La fonction à échoué on passe le champ next_succes (cas de succès) à NULL
        else p->next_failure=NULL;  // La fonction à réussi, on passe le champ next_failure (cas d'échec) à NULL
    }

    // On ferme les descripteurs si besoin
    if (p->stdin != 0 && p->stdin != 1 && p->stdin != 2) close(p->stdin);
    if (p->stdout != 0 && p->stdout != 1 && p->stdout != 2) close(p->stdout);
    if (p->stderr != 0 && p->stderr != 1 && p->stderr != 2) close(p->stderr);

    return 0;
}

/*
      Fonction init_cmd
      @param p : structure cmd_t permettant l'initialisation de la commande
      @return un entier, 0 l'initialisation a réussi
*/
int init_cmd(cmd_t* p) {
    // Initialisation de chaque champ de p
    p->pid=-1;
    p->status=0;
    p->stdin=0;
    p->stdout=1;
    p->stderr=2;
    p->wait=1;  // Par défaut, on attendra la fin d'un processus avant de passer à un autre
    p->path=NULL;
    for(int i=0; i<MAX_CMD_SIZE; i++) p->argv[i]=NULL;
    p->fdclose[0] = -1;  // Aucun descripteur ouverts
    p->next=NULL;
    p->next_success=NULL;
    p->next_failure=NULL;
    return 0;
}

/*
      Fonction parse_cmd : Permettant de bien structurer un tableau de commande selon un tableau de ligne de commandes
      @param tokens : tableau de chaînes de caractères contenant des lignes de commandes
      @param cmds : tableau de structure de cmd_t permettant le stockage des commandes qui seront à exécuté
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int parse_cmd(char* tokens[], cmd_t* cmds, size_t max) {
    int idx_proc = 0;  // Indice dans notre variable cmds
    int idx_arg = 0;  // Indice dans notre tableau cmds->argv
    int idx_tok;  // Indice dans notre tableau tokens

    //On parcours le tableau de commandes pour trouver une case libre pour un nouveau processus
    while(cmds[idx_proc].pid != -1) idx_proc++;

    int is_not = 0;  // Booléen utilisé pour le not "!"

    // On parcourt le tableau tokens
    for(idx_tok = 0; tokens[idx_tok] != NULL; idx_tok++){
        if(idx_tok >= (int)max) break;

        // Premier argument d'une commande
        if (idx_arg==0 && strcmp(tokens[idx_tok], "!") != 0) {
            cmds[idx_proc].path = tokens[idx_tok];  // On met dans le path le premier tokens de la commande
            cmds[idx_proc].argv[idx_arg++] = tokens[idx_tok];  // On l'enregistre aussi comme premier argument dans argv
        }
        
        // Fin de commande ou commande lancée en arrière plan
        else if(strcmp(tokens[idx_tok], ";") == 0 || strcmp(tokens[idx_tok], "&") == 0){
            // Commande lancée en arrière plan
            if (strcmp(tokens[idx_tok], "&") == 0) cmds[idx_proc].wait = 0;  // On n'attend pas le processus fils lors du lancement de la commande
            
            if(tokens[idx_tok+1] != NULL){
                if (cmds[idx_proc].path == NULL) {
                    fprintf(stderr, "Commande vide");
                    return -1;
                }
                cmds[idx_proc].next=&cmds[idx_proc+1];  // Next pointera sur le prochain processus
                idx_proc++;  // On passe au processus suivant
                idx_arg=0;  // On réintialise notre compteur pour commencer au début du tableau du nouveau processus
                is_not = 0;  // On réinitialise aussi notre booléen à 0
                continue ;
            }
        }
        
        // Commande not
        else if(strcmp(tokens[idx_tok], "!") == 0){
            is_not = 1;
            continue ;
        }
        
        // Redirection de la sortie standard
        else if (strcmp(">", tokens[idx_tok])==0) {
            int fdout = open(tokens[idx_tok+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Ouverture du fichier de sortie standard
            if (fdout==-1) {
                perror("Erreur de redirection");
                return -1;
            }
            cmds[idx_proc].stdout=fdout;  // Changement de la nouvelle sortie standard dans la structure cmdt
            add_fd(cmds[idx_proc].fdclose, fdout);  // Enregistrement de l'ouverture du flux dans fdclose pour qu'il soit fermé plus tard
            idx_tok++;  // Passage au token suivant
            continue ;
        }
        
        // Redirection de la sortie standard en n'écrasant pas les données du fichier de sortie
        else if (strcmp(">>", tokens[idx_tok])==0) {
            int fdout = open(tokens[idx_tok+1], O_WRONLY | O_CREAT | O_APPEND, 0644);  // Ouverture du fichier de sortie standard en mode concaténation
            if (fdout==-1) {
                perror("Erreur de redirection");
                return -1;
            }
            cmds[idx_proc].stdout=fdout;  // Changement de la nouvelle sortie standard dans la structure cmdt
            add_fd(cmds[idx_proc].fdclose, fdout);  // Enregistrement de l'ouverture du flux dans fdclose pour qu'il soit fermé plus tard
            idx_tok++;  // Passage au token suivant
            continue ;
        }
        
        // Redirection de l'entrée standard
        else if (strcmp("<", tokens[idx_tok])==0) {
            int fdin = open(tokens[idx_tok+1], O_RDONLY, 0644);  // Ouverture du fichier d'entrée standard
            if (fdin==-1) {
                perror("Erreur de redirection");
                return -1;
            }
            cmds[idx_proc].stdin=fdin;  // Changement de la nouvelle entrée standard dans la structure cmdt
            add_fd(cmds[idx_proc].fdclose, fdin);  // Enregistrement de l'ouverture du flux dans fdclose pour qu'il soit fermé plus tard
            idx_tok++;  // Passage au token suivant
            continue ;
        }
        
        // Redirection de la sortie d'erreur
        else if (strcmp("2>", tokens[idx_tok])==0) {
            int fderr = open(tokens[idx_tok+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Ouverture du fichier de sortie d'erreur
            if (fderr==-1) { 
                perror("Erreur de redirection");
                return -1;
            }
            cmds[idx_proc].stderr=fderr;  // Changement de la nouvelle sortie d'erreur dans la structure cmdt
            add_fd(cmds[idx_proc].fdclose, fderr);  // Enregistrement de l'ouverture du flux dans fdclose pour qu'il soit fermé plus tard
            idx_tok++;  // Passage au token suivant
            continue ;
        }
        
        // Redirection de la sortie d'erreur en n'écrasant pas les données du fichier de sortie
        else if (strcmp("2>>", tokens[idx_tok])==0) {
            int fderr = open(tokens[idx_tok+1], O_WRONLY | O_CREAT | O_APPEND, 0644);  // Ouverture du fichier de sortie d'erreur en mode concaténation
            if (fderr==-1) {
                perror("Erreur de redirection");
                return -1;
            }
            cmds[idx_proc].stderr=fderr;  // Changement de la nouvelle sortie d'erreur dans la structure cmdt
            add_fd(cmds[idx_proc].fdclose, fderr);  //Enregistrement de l'ouverture du flux dans fdclose pour qu'il soit fermé plus tard
            idx_tok++;  // Passage au token suivant
            continue ;
        }
        
        // Redirection de la sortie standard sur la sortie d'erreur
        else if (strcmp(">&2", tokens[idx_tok])==0) {
            cmds[idx_proc].stdout=cmds[idx_proc].stderr;  // Changement de la nouvelle sortie standard dans la structure cmdt
            continue ;
        }
        
        // Redirection de la sortie d'erreur sur la sortie standard
        else if (strcmp("2>&1", tokens[idx_tok])==0) {
            cmds[idx_proc].stderr=cmds[idx_proc].stdout;  // Changement de la nouvelle sortie d'erreur dans la structure cmdt
            continue ;
        }
        
        // Redirection de la sortie standard de cmd1 sur l'entrée standard de cmd2
        else if (strcmp("|", tokens[idx_tok])==0){
            int tube[2];
            if (pipe(tube)==-1){  // Création d'un tube
                perror("pipe");
            }

            cmds[idx_proc+1].stdin = tube[0];  // Association du tube 0 à stdin du prochain processus
            cmds[idx_proc].stdout = tube[1];  // Association du tube 1 à stdout du prochain process
            add_fd(cmds[idx_proc+1].fdclose, tube[0]);  // Ajout des tubes à fdclose
            add_fd(cmds[idx_proc].fdclose, tube[1]);
            cmds[idx_proc].next=&cmds[idx_proc+1];  // Next pointera sur le prochain processus
            idx_proc++;  // On passe au processus suivant
            idx_arg=0;  // On réintialise notre compteur pour commencer au début du tableau du nouveau processus
            continue;
        }

        // Commande "ou" (la commande suivante ne se réalise qu'en cas d'échec de la commande précédente)
        else if(strcmp(tokens[idx_tok], "||") == 0){
            if(is_not){
                // La négation est utilisée cela signifie que la commande suivante sera exécutée uniquement en cas de succès de la commande précédente
                cmds[idx_proc].next_success = &cmds[idx_proc+1];  // next_succes pointera sur notre prochaine commande
                cmds[idx_proc].next_failure = NULL;
            } else {
                // La négation n'est pas utilisée cela signifie que la commande suivante sera exécutée uniquement en cas d'échec de la commande précédente
                cmds[idx_proc].next_success = NULL;
                cmds[idx_proc].next_failure = &cmds[idx_proc+1];  // next_failure pointera sur notre prochain commande
            }
            idx_proc++;  // On passe au processus suivant
            idx_arg=0;  // On réintialise notre compteur pour commencer au début du tableau du nouveau processus
            is_not = 0;  // On réinitialise aussi notre booléen à 0
            continue ;
        }

        // Commande "et" (la commande suivante ne se réalise qu'en cas de succès de la commande précédente)
        else if(strcmp(tokens[idx_tok], "&&") == 0){
            if(is_not){
                // La négation est utilisée cela signifie que la commande suivante sera exécutée uniquement en cas d'échec de la commande précédente
                cmds[idx_proc].next_success = NULL;
                cmds[idx_proc].next_failure = &cmds[idx_proc+1];  // next_failure pointera sur notre prochain processus
            }else{
                // La négation n'est pas utilisée cela signifie que la commande suivante sera exécutée uniquement en cas de succès de la commande précédente
                cmds[idx_proc].next_success = &cmds[idx_proc+1];  // next_succes pointera sur notre prochain processus
                cmds[idx_proc].next_failure = NULL;
            }
            idx_proc++;  // On passe au processus suivant
            idx_arg=0;  // On réintialise notre compteur pour commencer au début du tableau du nouveau processus
            is_not = 0;  // On réinitialise aussi notre booléen à 0
            continue ;
        }
        
        // Token non reconnu, on l'enregistre donc comme un argument du processus en cours
        else {
            cmds[idx_proc].argv[idx_arg++] = tokens[idx_tok];
            continue ;
        }
    }

    for(int i=1; i<=idx_proc; i++) merge_fdclose(cmds[0].fdclose, cmds[i].fdclose);
    for(int i=1; i<=idx_proc; i++) merge_fdclose(cmds[i].fdclose, cmds[0].fdclose);
    return 0;
}
