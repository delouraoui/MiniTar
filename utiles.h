#ifndef H_UTILES
#define H_UTILES 
#include "variables.h"
int checksu(char *,entete*,int);
void recupere(char const *[],int);
int verifieDroits(struct stat,gid_t,uid_t);
void safeWrite(int,const void *,size_t);
short veri_extention(const char *,int);
int compte_souschaine(char *);
int profondeur_chemin(char *);
void affichage_ls(entete ,uid_t ,gid_t );
int safeRead(int ,entete *,char *);
int analyse_path(char *);
void safeWrite_entete(int,entete);
void visite_archive(char *,int,char *);
int safeReadLock(int ,int ,entete *,char *);
void safeWrite_enteteLock(int ,int ,entete );
void safeWriteLock(int ,int ,const void *,size_t );
unsigned long tailleentete(entete);
#endif
