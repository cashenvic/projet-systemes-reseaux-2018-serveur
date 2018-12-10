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
    printf("\nConnexion etablie avec %d\n\n", socket_client);
    //recuperer la requete du client soit Demande d'image ou envoi d'image
    do {

        printf("\nAttente de requete\n");
        read(socket_client, &n, sizeof (int));

        if (n == 2) {
            system("clear");
            printf("\nRequete d'upload recue. En attente de fichier(s)\n");
            receptionFichier(socket_client, buffer);
        } else if (n == 1) {
            system("clear");
            printf("\nRequete de download recu. Procedure d'envoi de fichier(s)\n");
            chemin_de_fichier mes_images[20];
            chemin_de_fichier images_choisies[20];
            int taille_mimg = 0, taille_mimg_choisies = 0;
            
            printf("\n\nEnvoi de la liste des images du serveur\n");
            lister_image("./images/", mes_images, &taille_mimg);
            //envoyer la liste des images au client
            send(socket_client, mes_images, 2048, 0);
            //envoyer taille de la liste des images
            write(socket_client, &taille_mimg, sizeof (int)); 
            
            //recevoir le(s) choix du client
            printf("Attente des choix du client\n");
            //recperer le nombre d'images demandées
            read(socket_client, &taille_mimg_choisies, sizeof (int));
            //recupercer les noms des images choisies
            recv(socket_client, images_choisies, 2048, 0);
            printf("Demande de %d fichiers\n", taille_mimg_choisies);
            
            //envoyer image(s) correspondante(s):
            int i = 0;
            while (i < taille_mimg_choisies) {
                printf("\n");
                envoiFichier(socket_client, images_choisies[i].info, buffer);
                i++;
            }
        } else if (n == 0 || n == -1) {
            n = 0;
            printf("\nFin de la connexion\n");
            break;
        }
    } while ( n != 0);
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

    if (nb_img_attendus > 0){

        int i = 0;
        while (i < nb_img_attendus) {
            printf("\n\nreception du %de fichier\n", i+1);
            read(socket, &taille_img_attendu, sizeof (int)); //recuperer la taille du fichier
            printf("taille attendue %d\n", taille_img_attendu);

            recv(socket, tampon, 128, 0); //recuperer le nom du fichier
            strcpy(nomF, tampon);
            
            strcpy(repertoire, "./images/");
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

            int recevable = compare_type(repertoire);
            write(socket, &recevable, sizeof (int));

            //liberation/reinitialisation des ressources
            memset(tampon, '0', 512);
            strcpy(nomF, "");
            lu = 0;
            paquetRec = 0;
            taille_img_attendu = 0;
            //visualiser_image(repertoire);
            i++;
            fclose(fichier_recu);
            fichier_recu = NULL;
        }
    }
    //printf("Reçu: %s\n", recu);
    //doit appeler la verification d'iamges
}

void envoiFichier(int socket, char *cheminFichier, char *buffer) {
    FILE *fichier;
    char tampon[512];
    char repertoire[256] = "./images/";
    int paquetEnv = 0;

    strcat(repertoire, cheminFichier);
    printf("++++Le chemin du fichier %s\n", repertoire);
    if ((fichier = fopen(repertoire, "r")) == NULL) {
        perror("fopen");
        exit(-1);
    }

    fseek(fichier, 0, SEEK_END);
    int taille_image = ftell(fichier);
    write(socket, &taille_image, sizeof (int)); //envoi de la taille du fichier

    memset(tampon, '0', 512);
    fseek(fichier, 0, SEEK_SET);

    int lu = 0;
    while (paquetEnv < taille_image) {
        lu = fread(tampon, sizeof (char), 512, fichier);
        send(socket, tampon, 512, 0);
        paquetEnv += lu;
        memset(tampon, '0', 512);
    }
    printf("paquetRec = %d envoyés / %d\n\n", paquetEnv, taille_image);

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
                exit(0);
            }
        default:
            while (wait(NULL) != -1);
    }

}

void Mimetype(char nom_fichier[30], Mime tab[50], int* size) {
    FILE * fPtr = NULL;
    int i = 0;

    char str[50];
    //strcpy(str, "");

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
    fclose(fPtr);
}

int compare_type(char *cheminFichier) {
    //file -i :espace ext
    //comparer resultat avec le tableau de type mime...
    int p[2];
    pipe(p);
    char buf[50];
    char *com[] = {"file", "-i", cheminFichier, (char *) 0};
    char * ptr;
    char* mime_extrait = NULL, *fichier_mime = "mimetypes.txt";
    Mime tab_mimes[50];
    int taille_Mime = 0;

    strcpy(buf, "");
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

            ptr = strtok(buf, " "); //initialisation (et en même temps, prend la première occurence)
            
            ptr = strtok(NULL, ":"); // le suivant de :
            mime_extrait = strtok(ptr, ";");
            strcat(mime_extrait, "\n");
            printf("Le type mime de ce fichier est %s\n", mime_extrait);
            
            Mimetype(fichier_mime, tab_mimes, &taille_Mime);
            int i = 0, found = -1, comp = -1;
            while (i < taille_Mime && found < 0) {
                comp = strcmp(mime_extrait, tab_mimes[i].info);
                if (comp == 0) {                    
                    found = 1;
                }
                i++;
            }
            
            if (found < 0){
                if (unlink(cheminFichier) == -1){
                    perror("unlink");
                }
                printf("Le fichier %s n'est pas recevable, il sera supprimé\n", cheminFichier);
            } else {
                printf("Le fichier %s a été reçu et enregistré\n", cheminFichier);
            }
            return found;
            wait(NULL);
    }
}

void lister_image(char *repertoire, chemin_de_fichier tab[10], int *taille) {

    int i = 0;
    struct dirent *lecture;
    DIR *reponse;
    reponse = opendir(repertoire);
    if (reponse != NULL) {
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
    }
}
