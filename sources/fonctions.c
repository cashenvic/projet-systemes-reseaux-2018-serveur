/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "../headers/fonctions.h"

void end_of_service() {
    wait(NULL);
}

void affiche_aide() {
    printf("Usage: \tserveur <numero port> <nombre max clients>\n");
    printf("\tExemple: serveur 20000 15\n");
    exit(-1);
}

void creation_process(int socket_client, int socket_server) {
    switch (fork()) {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            close(socket_server);
            servir_client(socket_client);
            exit(0);
        default:
            //fermer la socket de service
            close(socket_client);
    }
}

void servir_client(int socket_client) {
    int n;
    char buffer[T_BUFF];
    printf("connexion etablie avec %d\n", socket_client);
    //recuperer la requete du client soit Demande d'image ou envoi d'image
    read(socket_client, &n, sizeof (int));

    if (n == 2) {
        printf("\nEn attente d'un fichier\n");
        //read(socket_client, &n, sizeof (int));
        receptionFichier(socket_client, buffer);
        //construction du nom fichier (chemin) à creer à partir la construction Zohir
    } else if (n == 1) {
        chemin_de_fichier mes_images[20];
        int taille_mimg = 0;
        printf("envoi d'un fichier\n");
        lister_image("./images/", mes_images, &taille_mimg);

        //envoyer le tableau ainsi construit
        //recevoir le(s) choix du client
        //envoyer image(s) correspondante(s)
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
    char tampon[512];
    FILE * fichier_recu = NULL;
    //image img;
    //char ch;
    char nomF[128], repertoire[128] = "./images/";
    int nb_img_attendus = 0, taille_img_attendu = 0, paquetRec = 0;

    read(socket, &nb_img_attendus, sizeof (int)); //recuperer le nombre de fichiers que le client veut envoyer
    printf("j'attend %d fichiers\n",  nb_img_attendus);

    int i = 0;
    while (i < nb_img_attendus) {
        printf("\n\nreception du %de fichier\n", i+1);
        read(socket, &taille_img_attendu, sizeof (int)); //recuperer la taille du fichier
        printf("taille attendue %d\n", taille_img_attendu);

        recv(socket, tampon, 128, 0); //recuperer le nom du fichier
        strcpy(nomF, tampon);
        
        strcat(repertoire, nomF);
        printf("son nom est %s\n", nomF);
        //fichier_recu = fopen(repertoire, "w+");
        if ((fichier_recu = fopen(repertoire, "w+")) == NULL) {
            perror("fopen");
            exit(-1);
        }
        int pa;
        int lu = 0;
        memset(tampon, '0', 512);

        while (paquetRec < taille_img_attendu) {
            recv(socket, tampon, 512, 0) > 0;
            pa = fwrite(tampon, sizeof (char), 512, fichier_recu);
            memset(tampon, '0', 512);
            paquetRec += pa;
        }
        printf(" paquetRec = %d recus / %d envoyés\n", paquetRec, taille_img_attendu);

        //liberation/reinitialisation des ressources
        memset(tampon, '0', 512);
        strcpy(nomF, "");
        printf("fichier %s recu\n", nomF);
        lu = 0;
        paquetRec = 0;
        taille_img_attendu = 0;
        visualiser_image(repertoire);
        i++;
        fclose(fichier_recu);
        fichier_recu = NULL;
    }
    //printf("Reçu: %s\n", recu);
    //doit appeler la verification d'iamges
}

void create_fichier(char *chemin, char *mode, char *buffer) {
    FILE * fichier = NULL;
    char *repertoire = "./images/";
    strcat(repertoire, chemin);
    fichier = fopen(repertoire, mode);

    /* fopen() return NULL if last operation was unsuccessful */
    if (fichier == NULL) {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fputs(buffer, fichier);
    fclose(fichier);
}

void envoiFichier(int socket, char *cheminFichier, char *buffer) {
    //lis le fichier à partir du rep client
    FILE *fichier;
    char ch;
    int i = 0;

    if ((fichier = fopen(cheminFichier, "rb")) == NULL) {
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

void recompose(char *buffer_from_server, image img) {
    FILE * fichier = NULL;
    char * pch;

    pch = strtok(buffer_from_server, ":");
    strcpy(img.nom_fichier, pch);

    pch = strtok(NULL, ":");
    strcpy(img.contenu_fichier, pch);

    //create_fichier(img.nom_fichier, "w", img.contenu_fichier);
    char *chemin = "./images/";
    strcat(chemin, img.nom_fichier);
    fichier = fopen(chemin, "wb");
    fputs(img.contenu_fichier, fichier);

    char image[50];
    strcpy(image, img.nom_fichier);

    visualiser_image(image);

    printf("Le fichier %s a été reçu\n", img.nom_fichier);

    fclose(fichier);
}

void visualiser_image(char* image) {
    char *com[3] = {"xdg-open", image, (char *) 0};
    /* on forke le processus */
    switch (fork()) {
        case -1:
            perror("fork erreur");
            exit(-1);
        case 0:
            //comportement du fils
            if (execvp("xdg-open", com) == -1) {
                perror("execvp");
                exit(-1);
            }
        default:
            while (wait(NULL) != -1);
    }

}

void Mimetype(char nom_fichier[30], Mime tab[50], int* size) {
    FILE * fPtr = NULL;
    int i = 0;

    char str[50];

    fPtr = fopen(nom_fichier, "r");

    if (fPtr == NULL) {
        perror("fopen");
        exit(-1);
    }


    while (fgets(str, 50, fPtr) != NULL) {
        strcpy(tab[i].info, str);
        i++;
    }
    *size = i;
}

void compare_type(char *fichier) {
    //file -i :espace ext
    //comparer resultat avec le tableau de type mime...
    int p[2];
    pipe(p);
    char buf[50];
    char *com[] = {"file", "-i", fichier, (char *) 0};
    char * ptr;
    char* tmp = NULL;

    switch (fork()) {
        case -1:
            perror("fork erreur");
            exit(-1);
        case 0:
            dup2(p[1], STDOUT_FILENO);
            close(p[1]);
            close(p[0]);
            execvp("/usr/bin/file", com);
            exit(0);
        default:
            close(p[1]);
            read(p[0], buf, sizeof (buf));
            printf("==> %s", buf);

            ptr = strtok(buf, ":"); //initialisation (et en même temps, prend la première occurence)
            //printf ("\"%s\"\n",ptr);

            ptr = strtok(NULL, ":"); // le suivant de : 
            tmp = strtok(ptr, ";");
            printf("\"%s\"\n", tmp);
            wait(NULL);
    }
}

void lister_image(char *repertoire, chemin_de_fichier tab[10], int *taille) {

    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir(repertoire);
    if (reponse != NULL) {
        printf("\n \nListe des fichiers du repertoire d'image: \n");
        while ((lecture = readdir(reponse))) {
            //Amelioration: essayer de pas tenir compte de .. et .
            strcpy(tab[i].info, "");
            //strcat(tab[i].info, repertoire);
            strcat(tab[i].info, lecture->d_name);
            //struct stat st;
            i++;
        }
        *taille = i;
        i = 0;
        closedir(reponse), reponse = NULL;
        while (i < *taille) {
            printf("\n%d- %s \n", i, tab[i].info);
            i++;
        }
    }
}

void chaine_structure_liste(char p2[120], chemin_de_fichier tab [10], int taille, int choix) {
    int i = 0;
    char str[10];
    switch (choix) {
        case 1: /*Choix de Structure vers Chaine de caractere*/

            for (i; i < taille; i++) {
                sprintf(str, "%d", i);
                strcat(p2, str);
                strcat(p2, "|");
                strcat(p2, tab[i].info);
                if (i < taille - 1)
                    strcat(p2, ":");
            }
            break;
        case 2: /*Choix de chaine de caractaire vers Structure*/

            break;
        default: printf("--erreur de choix !!\n");
            break;
    }
}
