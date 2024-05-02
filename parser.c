/*
  Projet minishell - Licence 3 Info - PSI 2023
 
 Nom : MONTGENIE - FRANCOIS
 Prénom : Yanis - Thibaut
 Num. étudiant : 22101878 - 22110629
 Groupe de projet : 11
 Date : 12 octobre 2023
 
  Parsing de la ligne de commandes utilisateur (implémentation).
 
 */

/*
   Fichier parser.c : Fichier contenant les fonctions relatives au bon parsing d'une ligne de commande
   Auteur : MONTGENIE Yanis FRANCOIS Thibaut
   Dépendances : stddef.h stdio.h stdlib.h string.h
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
      Fonction trim : Suppression de tous les espaces au début et à la fin d'une chaîne de caractères
      @param str : chaîne de caractères
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int trim(char* str) {
    if (str == NULL) {
        return -1;
    }

    int length = strlen(str);
    int start = 0;
    int end = length - 1;

    // Trouver l'indice du premier caractère non espace au début de la chaîne
    while (start < length && str[start]==' ') {
        start++;
    }

    // Trouver l'indice du premier caractère non espace à la fin de la chaîne
    while (end > start && str[end]==' ') {
        end--;
    }

    // Si la chaîne est composée uniquement d'espaces, la réduire à une chaîne vide
    if (start > end) {
        str[0] = '\0';
    } else {
        // Déplacer les caractères non espace au début de la chaîne.
        memmove(str, &str[start], end - start + 1);
        str[end - start + 1] = '\0'; // Ajout du caractère de fin de chaîne.
    }

    return 0;
}

/*
      Fonction clean : Suppression de tous les doublons d’espaces d'une chaîne de caractères
      @param str : chaîne de caractères
      @return un entier, (code de retour) 0 si la fonction a réussi, -1 sinon
*/
int clean(char* str) {
    if (str == NULL) {
        return -1;
    }

    int ind = 0; // Indice d'écriture
    int nb_espace = 1; // Indique si un espace a été vu, initialisé avec 1 pour ignorer les espaces de début

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i]!=' ') {
            str[ind++] = str[i];
            nb_espace = 0;
        } else if (!nb_espace) {
            str[ind++] = ' ';
            nb_espace = 1;
        }
    }

    // Si la chaîne n'est pas vide et le dernier caractère est un espace, le supprimer
    if (ind > 0 && str[ind - 1]==' ') {
        ind--;
    }

    str[ind] = '\0'; // Ajout du caractère nul de fin à la nouvelle chaîne

    return 0;
}

/*
      Fonction separate_s : Ajout d'espaces autour de toutes les occurrences des caractères d'une chaîne lorsqu'il n'y a pas d'espace
      et sans ajouter d'espace au début ou à la fin

      @param str : chaîne de caractères
      @param s : chaîne de caractères indiquant autour de quel caractère doivent être insérée les espaces
      @param max : entier indiquant la capacité à ne pas dépasser
      @return un entier, (code de retour) 0 si la fonction a réussi, 1 si la valeur de max est inférieure à la taille de str
      2 si str est vide ou 3 si s est vide
*/
int separate_s(char* str, char* s, size_t max) {
    if(max < strlen(str)){
        fprintf(stderr, "max doit être supérieur ou égal à la taille de str\n");
        return 1;
    }

    if(str[0] == '\0' || str == NULL) {
        fprintf(stderr, "La chaîne str est vide\n");
        return 2;
    }

    if(s[0] == '\0' || s == NULL) {
        fprintf(stderr, "La chaîne s est vide\n");
        return 3;
    }

    // Parcours de str
    for(int i=0; i < (int)strlen(str); i++){
        // Parcours de tous les caractères dans s
        for(int j=0; j < (int)strlen(s); j++){

            if(str[i]!=s[j]) continue;  // Si le caractère à l'indice i de str ne correspond pas au caractère à l'indice j de s on passe au caractère suivant

            // Le caractère de str correspond à un caractère de s

            int casparticulier = 0;
            //  - Redirection de la sortie par 2> ou 2>>
            if(str[i-1]=='2' && str[i]=='>') { i--; casparticulier=1; }
            //  - Redirection de la sortie par 2>&1
            if(str[i-2]=='2' && str[i-1]=='>' && str[i]=='&' && str[i+1]=='1') { i-=2; casparticulier=2;  }

            // Il n'y a pas d'espace avant
            if(str[i-1]!=' '){
                if(strlen(str) >= max){
                    fprintf(stderr, "Taille maximale dépassée, changement impossible\n");
                    return 4;
                }

                if (i!=0){  // Si un caractère de s se trouve à l'indice 0 dans str on n'ajoute pas d'espace
                    memmove(str+i+1, str+i, strlen(str)-i);  // On déplace la chaîne str d'un indice à partir de l'indice i
                    str[i] = ' ';  // On insère l'espace
                    i++;  // On avance d'un indice pour bien se replacer dans str
                }
            }

            i+=casparticulier;

            //  - Répétition du même symbole pour && || >> <<
            if(str[i+1]==s[j]) i++;
            //  - Redirection de la sortie >&2
            if(str[i]=='>' && str[i+1]=='&' && (str[i+2]=='1' || str[i+2]=='2')) i+=2;

            // Il n'y a pas d'espace après
            if(str[i+1] != ' '){
                if(strlen(str) >= max){
                    fprintf(stderr, "taille maximale dépassée, changement impossible\n");
                    return 4;
                }
                if (i!=(int)strlen(str)-1){
                    memmove(str+i+2, str+i+1, strlen(str)-i);  // On déplace la chaîne str de deux indices à partir de l'indice i
                    str[i+1] = ' ';
                }
            }
            i++;  // On avance d'un indice dans str car on vient d'ajouter un espace
        }
    }
    return 0;
}

/*
      Fonction substenv : Substitution de la variable d'environnement par sa valeur si elle existe
      @param str : chaîne de caractères permettant l'identification de la variable d'environnement
      @param max : entier indiquant la capacité à ne pas dépasser
      @return un entier, (code de retour) 0 si la fonction a réussi, 1 si la valeur de max est inférieure à la taille de str, 2 si str est vide
*/
int substenv(char* str, size_t max) {
    if (str == NULL || str[0] == '\0') {
        fprintf(stderr, "La chaîne str est vide ou NULL\n");
        return 1;
    }

    size_t len = strlen(str);
    if (len >= max) {
        fprintf(stderr, "La chaîne str dépasse la taille maximale autorisée\n");
        return 2;
    }

    char *p = str;
    char buffer[256];
    char *start, *end, *varEnvVal;
    size_t len_tmp = len;

    while (*p && len_tmp < max) {
        if (*p == '$' && (isalpha(*(p + 1)) || (*(p + 1) == '{'))) {
            start = p++;
            int accolade = (*p == '{');
            p += accolade;

            end = p;
            while (*end && (isalnum(*end))) end++;
            if (accolade && *end != '}') {
                p = end + 1;
                continue;
            }

            strncpy(buffer, p, end - p);
            buffer[end - p] = '\0';

            varEnvVal = getenv(buffer);
            if (varEnvVal) {
                size_t lenVar = strlen(varEnvVal);
                size_t total_len = (start - str) + lenVar + strlen(end + accolade);
                if (total_len >= max) {
                    fprintf(stderr, "Substitution dépasse la taille maximale\n");
                    return -1;
                }

                memmove(start + lenVar, end + accolade, strlen(end + accolade) + 1);
                memcpy(start, varEnvVal, lenVar);
                len_tmp = strlen(str);
                p = start + lenVar;
            } else {
                p = end + accolade;
            }
        } else {
            p++;
        }
    }

    return 0;
}


/*
      Fonction strcut : Découpe d'une chaîne de caractères selon un séparateur (caractère)
      @param str : chaîne de caractères
      @param sep : caractère permettant de découper la chaîne de caractères
      @param tokens : tableau de chaînes de caractères permettant le stockage des chaînes de caractères découpées selon le séparateur
      @param max : entier indiquant la capacité à ne pas dépasser
      @return un entier, le nombre de mots séparé par sep
*/
int strcut(char* str, char sep, char** tokens, size_t max) {
    int count = 0; // Nombre de mots
    char* token = strtok(str, &sep);  // Découpe de str en tokens en utilisant sep et la fonction strtok

    while (token != NULL && count < max-1) {
        tokens[count++] = strdup(token);
        token = strtok(NULL, &sep);  // Passage au token suivant
    }

    tokens[count] = NULL;  // Terminer le tableau avec NULL

    return count;
}

// Optionnel
/*
      Fonction strcut_s : Version modifiée de strcut permettant en plus la découpe en un seul mot de tous les caractères situés entre " ou '
      @param str : chaîne de caractères
      @param sep : caractère permettant de découper la chaîne de caractères
      @param tokens : tableau de chaînes de caractères permettant le stockage des chaînes de caractères découpées selon le séparateur
      @param max : entier indiquant la capacité à ne pas dépasser
      @return un entier, le nombre de mots séparé par sep
*/
int strcut_s(char* str, char sep, char** tokens, size_t max) {
    int count = 0;
    char* start = str;

    while (*str != '\0' && count < max - 1) {
        if (*str == '"' || *str == '\'') {  // Le caractère est une guillemet ou un apostrophe
            char quote = *str;  // On récupère le caractère soit " soit '
            str++;  // On passe au caractère suivant
            start = str;

            while (*str != '\0' && *str != quote) str++;  // Tant qu'on est pas arrivé à la fin de la chaîne ou qu'on a pas trouvé la deuxième guillement ou apostrophe on avance dans str

            if (*str == quote) {  // On a trouvé la deuxième guillement ou apostrophe on avance dans str
                size_t token_len = str - start;  // On récupère la longueur entre les deux guillemets ou apostrophe
                tokens[count] = (char*)malloc(token_len + 1);  // Allocation mémoire
                strncpy(tokens[count], start, token_len);  // Copie du mot de longueur token_len à partir de l'indice start dans le tableau de tokens
                tokens[count][token_len] = '\0';
                count++;
                str++;  // On passe la deuxième guillemet ou apostrophe
            }
        }
        else if (*str != sep) {  // Le caractère n'est ni un séparateur ni une guillement ou un apostrophe
            // Reproduit le fonctionnement de strcut
            start = str;

            while (*str != '\0' && *str != sep) {
                str++;
            }

            size_t token_len = str - start;
            tokens[count] = (char*)malloc(token_len + 1);
            strncpy(tokens[count], start, token_len);
            tokens[count][token_len] = '\0';
            count++;
        }
        else {  // Le caractère est un séparateur
            str++;
        }
    }

    tokens[count] = NULL;
    return count;
}