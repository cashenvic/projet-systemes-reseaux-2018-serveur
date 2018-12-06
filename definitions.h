/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   definitions.h
 * Author: cash
 *
 * Created on 7 décembre 2018, 00:15
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define T_BUFF 1024

typedef struct {
    char nom_fichier[256];
    char contenu_fichier[1000];
} image;

/** @brief Respresente un fichier dans le listing des fichiers d'un repertoire
 *  @struct
 **/
typedef struct {
    char info[256];
    int taille;
} chemin_de_fichier;

typedef struct {
    char info[50];
} Mime;

#endif /* DEFINITIONS_H */

