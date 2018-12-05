/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: cash
 *
 * Created on 1 décembre 2018, 21:38
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define N_PORT 20000
#define MAX_CLIENTS 10
#define T_BUFF 1024

//_(°_°)_Tous les prototypes devront finir dans des fichiers headers (*.h)

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

void end_of_service() {
    wait(NULL);
}

int main(int argc, char** argv) {
    //declaration des variables
    int socket_server, socket_client;
    int loop = 1;
    int binded;
    int client_add_len;
    struct sigaction sign; /* déclaration d'une variable de type struct sigaction */
    struct sockaddr_in server_add, client_add;


    sign.sa_handler = end_of_service; /* le champ sa_handler de cette variable reçoit (le nom de) la fonction sur laquellele déroutement devra se faire */
    sign.sa_flags = SA_RESTART; /* cela permettra d'eviter l'interruption de "accept" par la reception du SIGCHLD */
    sigaction(SIGCHLD, &sign, NULL);

    //preparation des champs pour sockaddr_in adresse
    client_add.sin_family = AF_INET;
    client_add.sin_port = htons(N_PORT);
    client_add.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_server = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_server == -1) {
        perror("socketServ");
        exit(-1);
    }

    //attachement de la socket d'ecoute à une adresse
    printf("bind...\n");
    binded = bind(socket_server, (struct sockaddr *) &client_add, sizeof (client_add));
    if (binded == -1) {
        perror("bind");
        exit(-1);
    }

    //ouverture du service sur la socket d'ecoute
    printf("listen...\n");
    if (listen(socket_server, MAX_CLIENTS) == -1) {
        perror("listen");
        exit(-1);
    }

    printf("arrive...\n");
    client_add_len = sizeof (server_add);
    while (loop) {
        socket_client = accept(socket_server, (struct sockaddr *) &client_add, &client_add_len);
        if (socket_client == -1) {
            perror("accept");
            exit(-1);
        }
        creation_process(socket_client, socket_server);
    }
    return (EXIT_SUCCESS);
}

void creation_process(int socket_client, int socket_server) {
    switch (fork()) {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            servir_client(socket_client);
            close(socket_server);
            exit(0);
        default:
            //fermer la socket de service
            close(socket_client);
    }
}

void servir_client(int socket_client) {
    int n;
    char buffer[T_BUFF];
    //recuperer la requete du client soit Demande d'image ou envoi d'image
    read(socket_client, &n, sizeof (int));
    
    if (n == 2) {
        printf("\nen attente d'un fichier\n");
        receptionFichier(socket_client, buffer);
    } else if (n == 1) {
        printf("envoi d'un fichier\n");
        envoiFichier(socket_client, "djeliba.png", buffer);
    } else {
        //envoi code d'ereur??
    }
}

void sendToClient(int socket, char *buffer) {
    if (write(socket, buffer, strlen(buffer)) == -1) {
        perror("send");
        exit(-1);
    }
}

void receptionFichier(int socket, char *buffer) {
    char* recu;
    while (read(socket, buffer, T_BUFF)) {
        //strcat(recu, buffer);
        printf("Reçu: %s\n", buffer);
    }
    printf("Reçu: %s\n", recu);
    //doit appeler la verification d'iamges
}

void envoiFichier(int socket, char *cheminFichier, char *buffer) {
    //lis le fichier à partir du rep client
    FILE *fichier;
    char ch;
    int i = 0;

    if ((fichier = fopen(cheminFichier, "r")) == NULL) {
        perror("fopen");
        exit(-1);
    }
    printf("successfull opening of file %s\n", cheminFichier);

    while ((ch = fgetc(fichier)) != EOF) {
        printf("%c\n", ch);
        printf("%d\n", i);
        i++;
    }
    while ((ch = fgetc(fichier)) != EOF) {
        strcat(buffer, &ch);
        if ((i == T_BUFF - 1)) {
            sendToClient(socket, buffer);
            printf("Envoyé: %s\n", buffer);
            i = 0;
            strcpy(buffer, "");
            //strcat(buffer, &ch);
        }
        i++;
    }
    if (strcmp(buffer, "") != 0) {
        sendToClient(socket, buffer);
        printf("Envoyé: %s\n", buffer);
        strcpy(buffer, "");
    }

    fclose(fichier);

    //ecrire dans la socket serveur
}
