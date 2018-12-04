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
void servir_client(int socket_client, char *buffer);

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
    int n;
    char buffer[T_BUFF];
    switch (fork()) {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            servir_client(socket_client, buffer);
            close(socket_server);
            exit(0);
        default:
            //fermer la socket de service
            close(socket_client);
    }
}

void servir_client(int socket_client, char *buffer) {
    while (read(socket_client, buffer, T_BUFF)) {
        printf("Reçu: %s", buffer);
    }
}

