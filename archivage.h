#ifndef H_ARCHIVAGE
#define H_ARCHIVAGE

void archive(char *[],int,int [],char *,int);
void archive_file(char *,int,char *,int);
void ecrit_rep(char *,int ,struct stat,char *,int);
void archive_rep(char *,int ,uid_t,gid_t ,int,char *,int,int,int);
void archive_slink(char *,int,char *,int);
void ajoute_fichiers(char *[],int,int);
#endif
