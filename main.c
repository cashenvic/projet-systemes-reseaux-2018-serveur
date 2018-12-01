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

void creation_process();
void servir_client();

void end_of_service() {
    wait(NULL);
}

int main(int argc, char** argv) {
    //declaration des variables
    int socketServ, socket_client;
    int loop = 1;
    struct sigaction sign; /* déclaration d'une variable de type struct sigaction */
    struct sockaddr_in adresse;
    int n_port = 20000;
    int binded;
    const int max_clients = 10;


    sign.sa_handler = end_of_service; /* le champ sa_handler de cette variable reçoit (le nom de) la fonction sur laquellele déroutement devra se faire */
    sign.sa_flags = SA_RESTART; /* cela permettra d'eviter l'interruption de "accept" par la reception du SIGCHLD */
    sigaction(SIGCHLD, &sign, NULL);


    socketServ = socket(AF_INET, SOCK_STREAM, 0);
    //preparation des champs pour sockaddr_in adresse
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(n_port);
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);


    if (socketServ == -1) {
        perror("socketServ");
        exit(-1);
    }

    //attachement de la socket d'ecoute à une adresse

    binded = bind(socketServ, (struct sockaddr *) &adresse, sizeof (adresse));
    if (binded == -1) {
        perror("bind");
        exit(-1);
    }

    //ouverture du service sur la socket d'ecoute
    if (listen(socketServ, max_clients) == -1) {
        perror("listen");
        exit(-1);
    }

    socket_client = accept(socketServ, (struct sockaddr *) adresse, sizeof (adresse));
    while (loop) {
        if (socket_client == -1) {
            perror("accept");
            exit(-1);
        }
        creation_process();
        switch (fork()) {
            case -1:
                perror("fork");
                exit(-1);
            case 0:
                servir_client();
                close(socketServ);
                break;
            default:
                //fermer la socket de service
                close(socket_client);
        }
    }
    return (EXIT_SUCCESS);
}

void creation_process() {
    /*switch (fork()) {
                case -1:
                    perror("fork");
                    exit(-1);
                case 0:
                    servir_client();
                    break;
                default:
                    //fermer la socket de service
                    close(socket_client);
            }*/
}

void servir_client() {
    //fermer la socket d'ecoute
    //close(socketServ);
}

