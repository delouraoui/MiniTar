#include "variables.h"
#include "utiles.h"
#include "archivage.h"
#include "extraction.h"
#include "reconstruction.h"
/*permet le mode affichage de l archive en affichant 
les informations d un fichier ou repertoire passe en argument*/
void affichage_ls(entete infos,uid_t user,gid_t group){
	char doits_and_type[10],*heure;
	struct passwd *pwu;
	int i;
	if(S_ISREG(infos.mode))
		doits_and_type[0]='_';
	else if(S_ISFIFO(infos.mode))
		doits_and_type[0]='p';
	else if(S_ISCHR(infos.mode))
		doits_and_type[0]='c';
	else if(S_ISBLK(infos.mode))	
		doits_and_type[0]='b';
	else if(S_ISDIR(infos.mode))
		doits_and_type[0]='d';
	else if(S_ISLNK(infos.mode))
		doits_and_type[0]='l';
	else if(S_ISSOCK(infos.mode))
		doits_and_type[0]='s';
	else
		doits_and_type[0]=' ';
	if(infos.mode & S_IRUSR)
		doits_and_type[1] = 'r';
	else doits_and_type[1] ='-';
	if(infos.mode & S_IWUSR)
		doits_and_type[2] = 'w';
	else doits_and_type[2] ='-';
	if(infos.mode & S_IXUSR)
		doits_and_type[3] = 'x';
	else doits_and_type[3] ='-';
	if(infos.mode & S_IRGRP)
		doits_and_type[4] = 'r';
	else doits_and_type[4] ='-';
	if(infos.mode & S_IWGRP)
		doits_and_type[5] = 'w';
	else doits_and_type[5] ='-';
	if(infos.mode & S_IXGRP)
		doits_and_type[6] = 'x';
	else doits_and_type[6] ='-';
	if(infos.mode & S_IROTH)
		doits_and_type[7] = 'r';
	else doits_and_type[7] ='-';
	if(infos.mode & S_IWOTH)
		doits_and_type[8] = 'w';
	else doits_and_type[8] ='-';
	if(infos.mode & S_IXOTH)
		doits_and_type[9] = 'x';
	else doits_and_type[9] ='-';
	for(i=0;i<10;i++)
		printf("%c",doits_and_type[i]);
	pwu = getpwuid (user);
	heure = malloc(sizeof(char)*strlen(ctime(&infos.m_time)));
	strcpy(heure,ctime(&infos.m_time));
	heure[strlen(ctime(&infos.m_time))-1] = '\0';
	fprintf(stdout," %s %s %lu %s %s\n",pwu->pw_name,pwu->pw_name,infos.file_length,heure,infos.path);
	free(heure);
	//fprintf(stdout," %d %s %s %lld %s %s \n",infos.st_nlink,pwu->pw_name,pwu->pw_name,infos.st_size,ctime(&infos.st_mtime),argv);
}
/*realise le mode affichage pour toute l archive*/
void visite_archive(char *fichier,int entreeStd,char *specifier){
	entete et;
	struct stat st;
	int archive,lu;
	if(entreeStd==0){
		if((archive = open(fichier,O_RDONLY)) < 0)
		err(1,"impossible d ouvrie l archive %s \n",fichier);
	}else archive = 0;
	
	lseek(archive,0,SEEK_SET);
	stat(fichier,&st);
	lu=1;
	while(lu!=0){
		lu=0;
		lu = safeRead(archive,&et,fichier);
		if(lu==0) break;
		if(specifier!=NULL){
			if(verifie_extract(et.path,specifier)==1){
				if(et.file_length!=0){//cela signifie que nous somme sur un fichier 
					affichage_ls(et,st.st_uid,st.st_gid);
					lseek(archive,et.file_length,SEEK_CUR);
				}
				else{//c est un repertoire car les repertoires ont pour taille 0
					affichage_ls(et,st.st_uid,st.st_gid);
				}
			}else if(strcmp(et.path,specifier)==0){
				affichage_ls(et,st.st_uid,st.st_gid);
				lseek(archive,et.file_length,SEEK_CUR);
			}else{
				lseek(archive,et.file_length,SEEK_CUR);
			}
		}else{
			if(et.file_length!=0){//cela signifie que nous somme sur un fichier 
				affichage_ls(et,st.st_uid,st.st_gid);
				lseek(archive,et.file_length,SEEK_CUR);
			}
			else{//c est un repertoire car les repertoires ont pour taille 0
				affichage_ls(et,st.st_uid,st.st_gid);
			}
		}
		
	}
	close(archive);
}

/*fonction renvoyant la longueure d un chemin type a/b/c */
int profondeur_chemin(char *entree){
	int i,slash;
	slash = 0;
	for(i=strlen(entree);i>0;i--){
		if(entree[i]=='/')
			slash = slash + 1;
	}
	return slash;
}
int analyse_path(char *path){
	char copie [PATH_MAX];
	char *token;
	strcpy(copie,path);
	token = strtok(copie,"/");
	while( token != NULL){
		if(strcmp(token,"..")==0)
			return -1;
      token = strtok(NULL, "/");
   }
   return 0;
}
/*fonction qui compte le nombre de sous chemin*/
int compte_souschaine(char *entree){
	int i;
	for(i=strlen(entree);i>0;i--){
		if(entree[i]=='/')
			return strlen(entree)-(i+1);
	}
	return 0;
}
/*fonction permetant de verifier la validiter de lextention d une archive */
short veri_extention(const char *entree,int gzip){
	int i,length;
	char fichier[strlen(entree)];
	length = strlen(entree);
	strcpy(fichier,entree);
	for(i=0;i<length;i++){
		if(fichier[i]=='.'){
			if(( (length-i==4&&(strcmp(fichier+i,".mtr")==0))&&gzip==0 )||((length-i==7 && (strcmp(fichier+i,".mtr.gz")==0)) && gzip==1) )
				return 1;
		}
	}
	return 0;
}
unsigned long tailleentete(entete et){
	return sizeof(et.path_length)+sizeof(et.file_length)+sizeof(et.mode)+sizeof(et.m_time)+sizeof(et.a_time)+(sizeof(char)*32)+(sizeof(char)*et.path_length);
}
/*fonction qui permet le calcul du hash md5 en realisant un recouvrement 
de la commande md5 ou md5sum celon les systemes*/
int checksu(char *check,entete *et,int verifie){
	int fp[2],i=0;
	char comp[32];
	pid_t pid;

    if(pipe(fp) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
    }
	pid = fork();
	if(pid == 0){
		close(fp[0]);//on ferme le descripteur en lecture 
		dup2(fp[1],1);//on duplique le descripteur du tube sur l entree standar 
		//close(1);//on ferme l entree standar
		
		/*l executable de la commande md5 se trouve dans le fichier sbin du systeme*/
		

		execlp("md5","md5","-q",check,NULL);//on lance la commande md5
		execlp("md5sum","md5sum",check,NULL);//on lance la commande md5
		err(1,"probleme lors du calcul de l empreinte md5 du fichier : %s \n",check);
		exit (EXIT_FAILURE);
	}
	else{
		close(fp[1]);//on ferme le descripteur en lecture 
		if(verifie==0){
			read(fp[0],&(et->checksum),32);
			close(fp[0]);
		}else{
			read(fp[0],&(comp),32);
			close(fp[0]);
			for(i=0;i<32;i++){
				if(et->checksum[i]!=comp[i]){
					close(fp[0]);
					return -1;
				}
			}
		}
	}
	return 0;
}
/* fonction de compression utilise a larchivage */
void compresse_gzip(char *archive){
	pid_t pid;
	pid = fork();
	if(pid == 0){
		
		/*l executable de la commande md5 se trouve dans le fichier sbin du systeme*/
		execlp("gzip","gzip",archive,NULL);//on lance la commande md5
		err(1,"probleme lors de la compression : %s \n",archive);
	}
	else{
		wait(NULL);
	}
}
/* fonction de decompression utilise a l extraction */
void decompresse_gunzip(char const *archive,char *tmp){
	pid_t pid;
	int desc;	
	if((desc = open(tmp,O_WRONLY | O_CREAT | O_EXCL))<0){
		err(1,"probleme dans l ouverture du fichier temporaire : %s \n",tmp);
	}
	pid = fork();
	if(pid == 0){
		dup2(desc,STDOUT_FILENO);
		close(desc);
		/*l executable de la commande md5 se trouve dans le fichier sbin du systeme*/
		execlp("gunzip","gunzip","-c",archive,NULL);//on lance la commande md5
		err(1,"probleme lors de la decompression : %s \n",archive);
	}
	else{
		wait(NULL);
	}
}
int differentes(char const *argv){
	return (strcmp(argv,"-z")!=0)&&(strcmp(argv,"-f")!=0)&&(strcmp(argv,"-c")!=0)&&(strcmp(argv,"-d")!=0)&&(strcmp(argv,"-x")!=0)&&(strcmp(argv,"-l")!=0)&&(strcmp(argv,"-k")!=0)&&(strcmp(argv,"-a")!=0)&&(strcmp(argv,"-s")!=0)&&(strcmp(argv,"-C")!=0);
}
/*fonction de switch permetant de gerer les differentes combinaison a l archivage*/
void creation_archive(char const *argv[],int argc){
	int tab[5],option,i,nbpath=0,passe=0;
	char *fichiers[argc-2];
	char root[PATH_MAX],tmp[PATH_MAX];

	for(i=0;i<5;i++)
		tab[i]=0;
	for(option=0;option<argc;option++){

		if(strcmp(argv[option],"-f")==0){
			if((option+2)<argc){
				if(veri_extention(argv[(option+1)],0)==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[(option+1)]);exit(EXIT_FAILURE);}
				passe = 1;
				for(i=option+2;i<argc;i++){
					
					fichiers[i-(option+1)] = malloc(strlen(argv[i]));
					fichiers[i-(option+1)] = strcpy(fichiers[i-(option+1)],argv[i]);
					nbpath++;
				}

				fichiers[0] = malloc(strlen(argv[(option+1)]));
				fichiers[0] = strcpy(fichiers[0],argv[(option+1)]);
				
			}else{
				printf("usage mytar : la liste ne peut pas etre vide \n");
				exit(EXIT_FAILURE);
			}
			
		}
		if(strcmp(argv[option],"-s")==0)tab[0]=1;
		if(strcmp(argv[option],"-l")==0)tab[1]=1;
		if(strcmp(argv[option],"-v")==0)tab[2]=1;
		if(strcmp(argv[option],"-z")==0)tab[4]=1;
		if(strcmp(argv[option],"-C")==0){
			option++;
			strcpy(root,argv[option]);
			tab[3]=1;
		}

	}
	if(tab[3]==1&&passe==1)
		archive(fichiers,nbpath+1,tab,root,0);
	else if(passe==1)archive(fichiers,nbpath+1,tab,NULL,0);
	else{
		
		for(i=argc-1;i>0;i--){
			if(differentes(argv[i])==0)break;		
			fichiers[nbpath] = malloc(strlen(argv[i]));
			fichiers[nbpath] = strcpy(fichiers[nbpath],argv[i]);
			nbpath++;
		}

		/*echange du premier avec le dernier comm on lit a l'envert */
		if(nbpath>1){
			strcpy(tmp,fichiers[nbpath-1]);
			free(fichiers[nbpath-1]);

			fichiers[nbpath-1] = malloc(strlen(fichiers[0]));
			strcpy(fichiers[nbpath-1],fichiers[0]);

			free(fichiers[0]);
			fichiers[0] = malloc(strlen(tmp));
			strcpy(fichiers[0],tmp);

			/*fin switch */
		}
		printf("bon\n");
		if(tab[3]==1)
			archive(fichiers,nbpath,tab,root,1);
		else if(nbpath>0) archive(fichiers,nbpath,tab,NULL,1);
		}
		if(tab[1]==1&&passe==1)
			visite_archive(fichiers[0],0,NULL);
		if(tab[4]==1&&passe==1)
			compresse_gzip(fichiers[0]);
		if(passe==1){
			for(i=0;i<nbpath+1;i++)
				free(fichiers[i]);
		}else{
			for(i=0;i<nbpath;i++)
				free(fichiers[i]);
		}
	
}
/*fonction qui gere la commande -x et ses differentes options*/
void extraction_archive(char const *argv[],int argc){
	int tab[6],option,i,nbpath=0,passe=0;
	char *fichiers[argc-2];
	char repCur[PATH_MAX],temporaire[PATH_MAX];
	for(i=0;i<6;i++)
		tab[i]=0;
	getcwd(repCur,PATH_MAX);
	for(option=0;option<argc;option++){
		if(strcmp(argv[option],"-f")==0){
			passe=1;
			if((option+2)<argc){
				
				if(veri_extention(argv[(option+1)],tab[0])==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[(option+1)]);exit(EXIT_FAILURE);}
				
				for(i=option+2;i<argc;i++){
					fichiers[i-(option+1)] = malloc(strlen(argv[i]));
					fichiers[i-(option+1)] = strcpy(fichiers[i-(option+1)],argv[i]);
					nbpath++;
				}
				if(tab[0]==1){
					sprintf(temporaire,"%s/%s%s",repCur,argv[(option+1)],".tmp");
					decompresse_gunzip(argv[(option+1)],temporaire);
					fichiers[0] = malloc(strlen(temporaire));
					fichiers[0] = strcpy(fichiers[0],temporaire);

				}else{
					fichiers[0] = malloc(strlen(argv[(option+1)])+strlen(repCur)+1);
					sprintf(fichiers[0],"%s/%s",repCur,argv[(option+1)]);
				}
				
				extrait_archive(fichiers,nbpath+1,tab[4],tab[2],0,tab[5]);

				if(tab[1]==1)
					visite_archive(fichiers[0],0,NULL);
				if(tab[0]==1){
					unlink(fichiers[0]);
				}
				for(i=0;i<nbpath+1;i++)
					free(fichiers[i]);

				break;
			}else{
				if(veri_extention(argv[(option+1)],tab[0])==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[(option+1)]);exit(EXIT_FAILURE);}
				if(tab[0]==1){
					sprintf(temporaire,"%s/%s%s",repCur,argv[(option+1)],".tmp");
					decompresse_gunzip(argv[(option+1)],temporaire);
					fichiers[0] = malloc(strlen(temporaire));
					fichiers[0] = strcpy(fichiers[0],temporaire);
				}else{
					fichiers[0] = malloc(strlen(argv[(option+1)])+strlen(repCur)+1);
					sprintf(fichiers[0],"%s/%s",repCur,argv[(option+1)]);
				}
				extrait_archive(fichiers,argc-(option+1),tab[4],tab[2],0,tab[5]);
				if(tab[1]==1)
					visite_archive(fichiers[0],0,NULL);
				if(tab[0]==1){
					unlink(fichiers[0]);
				}

				free(fichiers[0]);

				break;
				exit(EXIT_FAILURE);
			}
			
		}
		if(strcmp(argv[option],"-l")==0)tab[1]=1;
		if(strcmp(argv[option],"-v")==0)tab[2]=1;
		if(strcmp(argv[option],"-z")==0)tab[0]=1;
		if(strcmp(argv[option],"-s")==0)tab[5]=1;
		if(strcmp(argv[option],"-k")==0)tab[4]=1;
		if(strcmp(argv[option],"-C")==0){
			option++;
			tab[3]=1;
			if(chdir(argv[option])<0){
				struct stat st;
				lstat(repCur,&st);

				if(mkdir((argv[option]),st.st_mode)<0){
					err(1,"erreur dans la cration du fichier %s \n",argv[option]);
				}
				if(chdir(argv[option])<0){
					err(1,"erreur dans la cration du fichier %s \n",argv[option]);
				}

			}
		}

	}
	if(passe == 0){
		extrait_archive(fichiers,1,tab[4],tab[2],1,tab[5]);
		if(tab[1]==1)
			visite_archive(fichiers[0],1,NULL);
	}
}
/*fonction permetant de gerer l option -a et les combinaison possible avec cette commande*/
void ajoute_archive(char const *argv[],int argc){
	int tab[2],option,i,indiceL,indiceV,nbpath=0;
	char *fichiers[argc-2];
	for(i=0;i<2;i++)
		tab[i]=0;
	for(option=0;option<argc;option++){
		if(strcmp(argv[option],"-f")==0){

			if((option+1)<argc){
				if(veri_extention(argv[(option+1)],0)==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[(option+1)]);exit(EXIT_FAILURE);}
				if(indiceV==0)indiceV=option;
				if(indiceL==0)indiceL = option;
				for(i=2;i<indiceV&&i<indiceL&&i<option;i++){
					if(strcmp(argv[i],"-f")!=0){
						fichiers[i-1] = malloc(strlen(argv[i]));
						fichiers[i-1] = strcpy(fichiers[i-1],argv[i]);
						nbpath++;
					}else{
						break;
					}
				}
				fichiers[0] = malloc(strlen(argv[option+1]));
				fichiers[0] = strcpy(fichiers[0],argv[option+1]);

				ajoute_fichiers(fichiers,nbpath+1,tab[1]);	

				if(tab[0]==1)
					visite_archive(fichiers[0],0,NULL);
				for(i=0;i<nbpath+1;i++)
					free(fichiers[i]);

				break;
			}else{
				printf("mtr : Usage -a path1...pathn \n");
				break;
				exit(EXIT_FAILURE);
			}
			
		}
		if(strcmp(argv[option],"-l")==0){
			tab[0]=1;
			indiceL = option;
		}
		if(strcmp(argv[option],"-v")==0){
			tab[1]=1;
			indiceV = option;
		}

	}
}
/*fonction principale gerant l interface du programme et ses differentes options*/
void recupere(char const *argv[],int argc){
	int i;
	char *fichiers[argc-2],temporaire[PATH_MAX],repCur[PATH_MAX];
	if(argc >= 2){
		if(strcmp(argv[1],"-c")==0){
			if(argc > 2)
				creation_archive(argv,argc);
		}
		else if(strcmp(argv[1],"-x")==0){
			extraction_archive(argv,argc);
		}
		else if(strcmp(argv[1],"-a")==0){
			ajoute_archive(argv,argc);
		}
		else if(strcmp(argv[1],"-l")==0){
			if(argc==3){printf("usage mytar : probleme arguments -l <element> -f <archive> \n");exit(EXIT_FAILURE);}
			if(strcmp(argv[2],"-z")==0&&strcmp(argv[3],"-f")==0){
				getcwd(repCur,PATH_MAX);
				if(veri_extention(argv[4],1)==0){unlink(fichiers[0]);free(fichiers[0]);fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[4]);exit(EXIT_FAILURE);}
				if(argc==6){
					fichiers[1] = malloc(strlen(argv[5]));
					fichiers[1] = strcpy(fichiers[1],argv[5]);
				}
				sprintf(temporaire,"%s/%s%s",repCur,argv[4],".tmp");
				decompresse_gunzip(argv[4],temporaire);
				fichiers[0] = malloc(strlen(temporaire));
				fichiers[0] = strcpy(fichiers[0],temporaire);
				if(argc==4) visite_archive(fichiers[0],0,fichiers[1]);
				else visite_archive(fichiers[0],0,NULL);
				unlink(fichiers[0]);
				free(fichiers[0]);
				if(argc==5)
					free(fichiers[1]);
			}else if(strcmp(argv[2],"-f")==0){
				if(argc==5){
					fichiers[1] = malloc(strlen(argv[4]));
					fichiers[1] = strcpy(fichiers[1],argv[4]);
				}
				if(veri_extention(argv[3],0)==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[3]);exit(EXIT_FAILURE);}
				fichiers[0] = malloc(strlen(argv[3]));
				fichiers[0] = strcpy(fichiers[0],argv[3]);
				if(argc==5) visite_archive(fichiers[0],0,fichiers[1]);
				else visite_archive(fichiers[0],0,NULL);
				free(fichiers[0]);
				if(argc==5)
					free(fichiers[1]);
			}
			else{
				printf("usage : \n");
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(argv[1],"-d")==0){
			if(argc<=4){printf("usage mytar : il manque des arguments -d <element> \n");exit(EXIT_FAILURE);}
			if(strcmp(argv[3],"-f")==0){
				if(veri_extention(argv[4],0)==0){fprintf(stderr, "erreur Format : %s , Usage : .mtr \n",argv[4]);exit(EXIT_FAILURE);}
				fichiers[1] = malloc(strlen(argv[2]));
				fichiers[1] = strcpy(fichiers[1],argv[2]);
				fichiers[0] = malloc(strlen(argv[4]));
				fichiers[0] = strcpy(fichiers[0],argv[4]);
				supprime_elements(fichiers,2);
				for(i=0;i<argc-3;i++)
					free(fichiers[i]);
			}
			else{
				printf("usage : \n");
				exit(EXIT_FAILURE);
			}	
		}
		else
			exit(0);
	}
	else
		exit(0);
}
/*fonction permetant la verification des droits d un fichier */
int verifieDroits(struct stat f_stat,gid_t group,uid_t user){
	if(f_stat.st_uid==user){
		if((f_stat.st_mode & S_IRUSR)&&(f_stat.st_mode & S_IXUSR))
			return 1;
		else
			return 0;
	}
	else if(f_stat.st_gid==group){
		if((f_stat.st_mode & S_IRGRP)&&(f_stat.st_mode & S_IXGRP))
			return 1;
		else
			return 0;
	}
	else if(f_stat.st_gid!=group && f_stat.st_uid!=user){
		if((f_stat.st_mode & S_IROTH)&&(f_stat.st_mode & S_IXOTH))
			return 1;
		else
			return 0;
	}
	else
		return 0;
	return 0;
}
/*fonction de lecture dans un fichier d archive donner au format */
int safeRead(int desc,entete *buf,char *name){
	int lu=0;
	entete et;
	if((lu = read(desc,&(et.path_length),sizeof(size_t)))< 0){
		fprintf(stderr, "1erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.file_length),sizeof(off_t)))< 0){
		fprintf(stderr, "2erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.mode),sizeof(mode_t)))< 0){
		fprintf(stderr, "3erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.m_time),sizeof(time_t)))< 0){
		fprintf(stderr, "4erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.a_time),sizeof(time_t)))< 0){
		fprintf(stderr, "5erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.checksum),(sizeof(char)*32)))< 0){
		fprintf(stderr, "6erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.path),(sizeof(char)*(et.path_length))))< 0){
		fprintf(stderr, "7erreur lecture du fichier : %s\n",name);
		exit(EXIT_FAILURE);
	}
	buf->path_length = et.path_length;
	buf->file_length = et.file_length;
	buf->mode = et.mode;
	buf->m_time = et.m_time;
	buf->a_time = et.a_time;
	strcpy(buf->checksum,et.checksum);
	strcpy(buf->path,et.path);
	buf->path[buf->path_length] ='\0';
	return lu;

}
/*fonction de lecture avec utilisation de verrou */
int safeReadLock(int desc,int verrou,entete *buf,char *name){
	int lu=0;
	entete et;
	if((lu = read(desc,&(et.path_length),sizeof(size_t)))< 0){
		fprintf(stderr, "1erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.file_length),sizeof(off_t)))< 0){
		fprintf(stderr, "2erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.mode),sizeof(mode_t)))< 0){
		fprintf(stderr, "3erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.m_time),sizeof(time_t)))< 0){
		fprintf(stderr, "4erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.a_time),sizeof(time_t)))< 0){
		fprintf(stderr, "5erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.checksum),(sizeof(char)*32)))< 0){
		fprintf(stderr, "6erreur lecture du fichier : %s\n",name);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	if(lu==0) return 0;
	if((lu = read(desc,&(et.path),(sizeof(char)*(et.path_length))))< 0){
		fprintf(stderr, "7erreur lecture du fichier : %s fichier name : %s %zu\n",name,et.path,et.path_length);
		close(verrou);
		exit(EXIT_FAILURE);
	}
	buf->path_length = et.path_length;
	buf->file_length = et.file_length;
	buf->mode = et.mode;
	buf->m_time = et.m_time;
	buf->a_time = et.a_time;
	strcpy(buf->checksum,et.checksum);
	strcpy(buf->path,et.path);
	buf->path[buf->path_length] ='\0';
	return lu;

}
/*fonction permetant d ecrire l entete d une archive */
void safeWrite_entete(int desc,entete buf){
	if (write(desc,&(buf.path_length),sizeof(size_t)) != (int)sizeof(size_t)) 
		err(1,"Probleme d'ecriture \n");
	if (write(desc,&(buf.file_length),sizeof(off_t)) != (int)sizeof(off_t)) 
		err(1,"Probleme d'ecriture\n");
	if (write(desc,&(buf.mode),sizeof(mode_t)) != (int)sizeof(mode_t)) 
		err(1,"Probleme d'ecriture\n");
	if (write(desc,&(buf.m_time),sizeof(time_t)) != (int)sizeof(time_t)) 
		err(1,"Probleme d'ecriture\n");
	if (write(desc,&(buf.a_time),sizeof(time_t)) != (int)sizeof(time_t)) 
		err(1,"Probleme d'ecriture\n");
	if (write(desc,&(buf.checksum),(sizeof(char)*32)) != (int)(sizeof(char)*32)) 
		err(1,"Probleme d'ecriture\n");
	if (write(desc,&(buf.path),(sizeof(char)*(buf.path_length))) != (int)(sizeof(char)*(buf.path_length))) 
		err(1,"Probleme d'ecriture\n");
	//printf("Write %lld %zu %s \n",buf.file_length,buf.path_length,buf.path);
}
/*fonction permetant d ecrire l entete d une archive avec l utilisation de verrou*/
void safeWrite_enteteLock(int desc,int verrou,entete buf){
	if (write(desc,&(buf.path_length),sizeof(size_t)) != (int)sizeof(size_t)){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.file_length),sizeof(off_t)) != (int)sizeof(off_t)){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.mode),sizeof(mode_t)) != (int)sizeof(mode_t)){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.m_time),sizeof(time_t)) != (int)sizeof(time_t)){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.a_time),sizeof(time_t)) != (int)sizeof(time_t)){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.checksum),(sizeof(char)*32)) != (int)(sizeof(char)*32)){
		close(verrou); 
		err(1,"Probleme d'ecriture\n");
	}
	if (write(desc,&(buf.path),(sizeof(char)*(buf.path_length))) != (int)(sizeof(char)*(buf.path_length))){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	}
	//printf("Write %lld %zu %s \n",buf.file_length,buf.path_length,buf.path);
}
/*fonction d ecriture avec utilisation de verrou */
void safeWriteLock(int desc,int verrou,const void *buf,size_t nbytes){
	if (write(desc,buf,nbytes) != (int)nbytes){
		close(verrou);
		err(1,"Probleme d'ecriture\n");
	} 	
}
/*fonction de lecture avec utilisation de verrou */
void safeWrite(int desc,const void *buf,size_t nbytes){
	if (write(desc,buf,nbytes) != (int)nbytes) 
		err(1,"Probleme d'ecriture\n");	
}
