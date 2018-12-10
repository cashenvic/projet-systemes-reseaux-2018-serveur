#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "definitions.h"

/** @brief Cree un processus fils qui se chargera de traiter la demande du client
 *  @param int socket_client : Le socket (descripteur de fichier) qu'il faut servir, c'est la socket du client
 *  @param int socket_server : Le socket (descripteur de fichier) qu'il faut fermer, c'est la socket du père
 *  @return void : Pas de valeur de retour
 **/
void creation_process(int socket_client, int socket_server);

/** @brief Méthode de traitement la demande du client
 *  @param int socket_client : Le socket (descripteur de fichier) qu'il faut servir, c'est la socket du client
 *  @return void : Pas de valeur de retour
 **/
void servir_client(int socket_client);

/** @brief Lis un fichier depuis le repertoire du client et l'envoie au serveur
 *  @param int socket : socket dans lequel il ecrit
 *  @param char *cheminFichier : chemin du fichier à lire
 *  @return void
 **/
void receptionFichier(int socket, char *buffer);

/** @brief Lis un fichier depuis le repertoire du client et l'envoie au serveur
 *  @param int socket : socket dans lequel il ecrit
 *  @param char *cheminFichier : chemin du fichier à lire
 *  @return void
 **/
void envoiFichier(int socket, char *cheminFichier, char *buffer);

/** @brief cree un fichier et l'ouvre selon le mode donné en paramètre
 *  @param char *chemin : nom du fichier qu'il doit creer et ouvrir
 *  @param char *mode : mode d'ouverture du fichier
 *  @return void
 **/
void create_fichier(char *chemin, char *mode, char *buffer);

/** @brief Recompse une chaine reçu depuis la socket
 *  @param char *buffer_from_server : contenu reçu du socket
 *  @param image img : fichier dans lequel le fichier recompsé sera mis
 *  @return void
 **/
void recompose(char *buffer_from_server, image img);

/** @brief Visualiser une image 
 *  @param char * image 
 
 *  @return void
 **/
void visualiser_image(char* image);

/** @brief Extrait le mimetype d'image 
 *  @param char * nom_fichier 
 *  @param Mime tab 
 *  @param int* size 
 *  @return void
 **/
void Mimetype(char nom_fichier[30], Mime tab[50], int* size);

/** @brief comapare le mimetype d'une image avec celles trouvées da le fichier mimes
 *  @param char * fichier
 *  @return int
 **/
int compare_type(char *fichier);

/** @brief Affiche la liste des fichiers dans le répértoire courant
 *  @param 
 *  @return char*
 **/
void lister_image(char *repertoire, chemin_de_fichier tab[10], int *taille);

/** @brief Conversion struct -> char*; char* -> struct
 *  @param char *cheminFichier: chemin du fichier à ouvrir
 *  @param char* mode: mode d'ouverture du fichier
 *  @return FILE: Retourne le fichier qu'il ouvert
 **/
void chaine_structure_liste(char p2[120], chemin_de_fichier tab [10], int taille, int choix);

void end_of_service();

void affiche_aide();
#endif /* FONCTIONS_H */

