#include "../headers/fonctions.h"

//_(°_°)_Tous les prototypes devront finir dans des fichiers headers (*.h)

int main(int argc, char** argv) {
    //declaration des variables
    int socket_server, socket_client;
    int loop = 1;
    int binded;
    unsigned int client_add_len;
    struct sigaction sign; /* déclaration d'une variable de type struct sigaction */
    struct sockaddr_in server_add, client_add;

    if (argc != 3) {
        affiche_aide();
    }
    const int N_PORT = atoi(argv[1]);
    const int MAX_CLIENTS = atoi(argv[2]);

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
    close(socket_server);
    return (EXIT_SUCCESS);
}

