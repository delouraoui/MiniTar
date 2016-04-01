#include "variables.h"
#include "utiles.h"
#include "reconstruction.h"	
/*fonction qui cree un repertoire a partir d une archive */
void creat_rep(char *path_dir,entete et,int no_ecrease){
	struct utimbuf *modification;
	if(mkdir(path_dir,et.mode)<0){
		if(errno == EEXIST){
			if(no_ecrease==1){
				fprintf(stderr, "Avertissement le repertoire : %s existe deja \n",path_dir);
				return;
			}
		}
		else{
			fprintf(stderr, "ERREUR SUR le repertoire : %s \n",path_dir);
			return;
		}
	}
	analyse_path(et.path);
	modification = malloc(sizeof(struct utimbuf));
	modification->actime = et.a_time;
	modification->modtime = et.m_time;
	utime(path_dir, modification);
	chmod(path_dir,et.mode);
	free(modification);
}
/*fonction qui creer un fichier a partire d une archive */
void creat_file(char *fichier,int archive,entete et,int no_ecrease,int cheksum,int no_ignore){
	struct flock fl;
	struct utimbuf *modification;
	char buffeur[BUFSIZE];
	int file,lu,courant,verrou;
	long long decrm;
	/*creation du fichier verifi si -k est specifier */

	if(S_ISLNK(et.mode)){
		if(no_ignore==1){
			if((lu = read(archive, &buffeur, et.file_length))<0 ){
			 	err(1,"x impossible d ouvrie le fichier %s \n",fichier);
			 	return;
			 }	 	
	 		 buffeur[lu] = '\0';
	 		 if(symlink(buffeur,fichier)<0){
	 		 	warn("x impossible de creer le lien %s \n",fichier);
			 	return;
	 		 }
	 		 return;
	 		}else{
	 			lseek(archive,et.file_length,SEEK_CUR);
				return;
	 		}
		 
	}else{
		if(no_ecrease==1){
			if((file = open(fichier,O_WRONLY | O_CREAT | O_EXCL,0600)) < 0){
				fprintf(stdout,"Avertissement le fichier : %s existe deja \n",fichier);
				lseek(archive,et.file_length,SEEK_CUR);
				return;
			}
		}
		else{

			if((file = open(fichier,O_WRONLY | O_CREAT | O_TRUNC,0600)) < 0)
				err(1,"impossible d ouvrie le fichier %s \n",fichier);
			if((verrou = open(fichier,O_WRONLY))<0){
				err(1,"impossible d archiver %s \n",fichier);
				return;
			}
			fl.l_type = F_WRLCK;
			fl.l_whence = SEEK_CUR;
			fl.l_start=0;
			fl.l_len=0;
			fl.l_pid=getpid();
			if( fcntl(verrou, F_SETLKW, &fl) == -1 ){
				warn(" verrou probleme %s \n",fichier);
			}
		}	
		
		/*ajustement de la taille du buffer */
		if(et.file_length>BUFSIZE){
			courant = BUFSIZE;
			decrm = et.file_length - BUFSIZE;
		}
		else{
			decrm = 0;
			courant = et.file_length;
		}
		/*lecture et ecriture du fichier */
		while(1){
			if((lu = read(archive,buffeur,(courant)))< 0){
				close(verrou);
				fprintf(stderr, "erreur lecture du fichier : %s\n",fichier);
				exit(EXIT_FAILURE);
			}
			safeWriteLock(file,verrou,buffeur,courant);
			if (lu < BUFSIZE) break;
			else{
				if(decrm>BUFSIZE){
					courant = BUFSIZE;
					decrm = decrm - BUFSIZE;
				}
				else
					courant = (int)decrm;
			}
		}

		modification = malloc(sizeof(struct utimbuf));
		modification->actime = et.a_time;
		modification->modtime = et.m_time;
		utime(fichier, modification);
		free(modification);
		chmod(fichier,et.mode);
		if(cheksum==1){
			if(checksu(fichier,&et,1)==-1){
				printf("le fichier : %s est corrompu extration stopé\n",fichier);
				unlink(fichier);
				close(verrou);
				close(file);
				return;
			}
		}
		
		close(verrou);
		close(file);
	}
	
}
/*focntion de vificatin lors de lextraction partielle en comparrant les deux chemins passe en parametre*/
int verifie_extract(char *path,char *racine){
	int i,profondeur_courant,profondeur_racine;
	profondeur_courant = profondeur_chemin(path);
	profondeur_racine = profondeur_chemin(racine);
	/*	si la profondeur du repertoire donne est inferieur a celle de la racine alors cela signifie 
		que l elements ne se trouve pas dans l arborescence l element doit donc etre ignore */
	if(strcmp(path,racine)==0) return 0;
	if(profondeur_courant < profondeur_racine) return 0;
	else if(profondeur_courant >= profondeur_racine){
		/*si les profondeures sont les memes alors on verifie que le chemin donnee par la racine est le meme */
		i=0;
		while(i<(int)strlen(racine)){
			if(path[i]!=racine[i]){
				/*il ne sagit pas de la meme racine on ne valide pas */
				return 0;
			}
			else
				i = i + 1;
		}
		if((int)strlen(path)==(int)strlen(racine)){
			return 1;
		}
		else{
			if(path[strlen(racine)]=='/'){

				return 1;
			}
		}
		/*la racine est commune on peut donc validee l extraction */
	}
	return 0;
}
/*fonction dextraction partielle permet d extraire que un partie de l archive */
void extrait_archive_partielle(int archive,int verrou,char *liste[],int no_ecrease,int taille,int cheksum,int no_ignore){
	entete et;
	int lu,passe=1;
	char cible_arche[PATH_MAX],absolut_path[PATH_MAX],fichier[PATH_MAX];

	/*on recupere le chemin absolu pour desarchiver*/
	
	for(int i=1;i<taille;i++){
		getcwd(absolut_path,PATH_MAX);
		/*nom de la racine */
		/*la partie a desarchiver */
		strcpy(cible_arche,liste[i]);

		lu =1;
		/*on cherche d abord la racine */

		lseek(archive,0,SEEK_SET);
		if(passe==1){
			//strcpy(absolut_path,fichier);
			do{
				/*le fichier en parametre sert juste a afficher si il y a une erreur dans la lecture */
				lu = safeReadLock(archive,verrou,&et,liste[0]);
				/*archive totalment lu*/
				if(lu==0) break;
				
				if(verifie_extract(et.path,cible_arche)){

					if(et.file_length!=0){//cela signifie que nous somme sur un fichier 
						sprintf(fichier,"%s%s",absolut_path,(et.path+strlen(cible_arche)));
						if(analyse_path(fichier)==-1){
							printf("x %s path containe .. \n", et.path);
							lseek(archive,et.file_length,SEEK_CUR);
						}else{
							if(S_ISLNK(et.mode)&&no_ignore==1){
								printf("x %s \n", et.path);
							}
							else if(!S_ISLNK(et.mode))
								printf("x %s \n", et.path);
							creat_file(fichier,archive,et,no_ecrease,cheksum,no_ignore);
						}
					}
					else{//c est un repertoire car les repertoires ont pour taille 0

						sprintf(fichier,"%s%s",absolut_path,(et.path+strlen(cible_arche)));
						if(analyse_path(fichier)==-1){
							printf("x %s path containe .. \n", et.path);
							lseek(archive,et.file_length,SEEK_CUR);
						}else{
							printf("x %s \n", et.path);
							creat_rep(fichier,et,no_ecrease);
						}	
					}
				}
				else{	
					if(strcmp(et.path,liste[i])==0 && S_ISREG(et.mode)){
						printf("x %s \n", et.path);
						creat_file(basename(et.path),archive,et,no_ecrease,cheksum,no_ignore);
					}
					else lseek(archive,et.file_length,SEEK_CUR);
				}
			}while(lu!=0);
		}
	}
	
	close(archive);
}
/*fonction d extraction d une archive */
void extrait_archive(char *liste[],int taille,int no_ecrease,int checksum,int entreeStandar,int no_ignore){
	entete et;
	//struct stat st;
	struct flock fl;
	char current_rep[PATH_MAX],fichier[PATH_MAX];
	int archive,verrou,lu;


	if(entreeStandar==0){
		if((archive = open(liste[0],O_RDONLY)) < 0)
			err(1,"impossible d ouvrie l archive %s \n",liste[0]);

		if((verrou = open(liste[0],O_RDONLY))<0){
			err(1," 6 impossible d archiver %s \n",liste[0]);
			return;
		}
		fl.l_type = F_RDLCK;
		fl.l_whence = SEEK_CUR;
		fl.l_start=0;
		fl.l_len=0;
		fl.l_pid = getpid();
		if( fcntl(verrou, F_SETLKW, &fl) == -1 ){
			warn(" verrou probleme %s \n",liste[0]);
		}
	}else archive = 0;
		
	/*si il ya un chemin de l erchive a extraire */
	if(taille>1){

		extrait_archive_partielle(archive,verrou,liste,no_ecrease,taille,checksum,no_ignore);
		close(verrou);
		return;
	}
	getcwd(fichier,PATH_MAX);

	strcpy(current_rep,fichier);
	do{
		/*on lit l archive */
		if(entreeStandar==0)
			lu = safeReadLock(archive,verrou,&et,liste[0]);
		else lu = safeReadLock(archive,verrou,&et,"entreeStandar");

		if(lu==0) break;

		if(taille == 1){

			if(et.file_length!=0){//cela signifie que nous somme sur un fichier 

				sprintf(fichier,"%s/%s",current_rep,et.path);
				if(analyse_path(fichier)==-1){
					printf("x %s path containe .. \n", fichier);
					lseek(archive,et.file_length,SEEK_CUR);
				}else{
					if(S_ISLNK(et.mode)&&no_ignore==1)
						printf("x %s \n", fichier);
					else if(!S_ISLNK(et.mode))
						printf("x %s \n", fichier);
					creat_file(fichier,archive,et,no_ecrease,checksum,no_ignore);
				}	
			}
			else{//c est un repertoire car les repertoires ont pour taille 0
				if(analyse_path(fichier)==-1){
					printf("x %s path containe .. \n", fichier);
					lseek(archive,et.file_length,SEEK_CUR);
				}else{
					sprintf(fichier,"%s/%s",current_rep,et.path);
					printf("x %s \n", fichier);
					creat_rep(fichier,et,no_ecrease);
				}	
			}
		}
		
	}while(lu!=0);
	close(verrou);
	close(archive);
}
