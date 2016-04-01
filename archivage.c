#include "variables.h"
#include "utiles.h"


void archive_slink(char *fichier,int archive,char *root,int newRoot){
	int lu;
	char buf[BUFSIZE];
	struct stat st;
	entete et;
	/*if((input = open(fichier,O_RDONLY))<0){
		warn(" x impossible d archiver lien %s ",fichier);
		return;
	}*/
	if((lstat(fichier,&st) != 0)){
		warn(" x impossible d archiver lien %s \n",fichier);
		return;
	}
	strcpy(et.checksum,"\0");
	et.file_length = st.st_size;
	et.mode = st.st_mode;
	et.m_time = st.st_mtime;
	et.a_time = st.st_atime;
	if(newRoot==1){
		et.path_length = strlen(fichier)+strlen(root)+1;
		sprintf(et.path,"%s/%s",root,fichier);
		et.path[et.path_length]='\0';
	}else{
		et.path_length = strlen(fichier);
		strcpy(et.path,fichier);
		et.path[strlen(fichier)]='\0';
	}
	safeWrite_entete(archive,et);
	while(1){
		lu = readlink(fichier,buf,BUFSIZE);
		if (lu < 0){
        	perror("lstat");
        	exit(EXIT_FAILURE);
    	}
   		if (lu > st.st_size){
        	fprintf(stderr, "symlink increased in size between lstat() and readlink()\n");
        	exit(EXIT_FAILURE);
    	}
		safeWrite(archive,buf,lu);

		if (lu < BUFSIZE) break;
	}
	printf("v %s\n", et.path);
}
void archive_file(char *fichier,int archive,int check,char *root,int newRoot){
	int input,verrou,lu;
	struct flock fl;
	char buf[BUFSIZE];
	struct stat st;
	entete et;

	if((input = open(fichier,O_RDONLY))<0){
		warn("x impossible d archiver %s \n",fichier);
		return;
	}
	if((verrou = open(fichier,O_RDONLY))<0){
		warn("x probleme de verrou sur  %s \n",fichier);
		return;
	}
	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start=0;
	fl.l_len=0;
	if( fcntl(verrou, F_SETLKW, &fl) == -1 ){
		warn("x probleme de verrou sur %s \n",fichier);
		return;
	}
	if((stat(fichier,&st) != 0)){
		warn(" x impossible d archiver %s \n",fichier);
		close(verrou);
		close(input);
		return;
	}
	if(check==1){
		checksu(fichier,&et,0);
	}
	else strcpy(et.checksum,"\0");
	if(st.st_size==0)
		et.file_length = 1;
	else
		et.file_length = st.st_size;
	et.mode = st.st_mode;
	et.m_time = st.st_mtime;
	et.a_time = st.st_atime;	
	if(newRoot==1){
		et.path_length = strlen(fichier)+strlen(root)+1;
		sprintf(et.path,"%s/%s",root,fichier);
		et.path[et.path_length]='\0';
	}else{
		et.path_length = strlen(fichier);
		strcpy(et.path,fichier);
		et.path[strlen(fichier)]='\0';
	}
	safeWrite_enteteLock(archive,verrou,et);
	while(1){
		lu = read(input,buf,BUFSIZE);
		safeWriteLock(archive,verrou,buf,lu);
		if (lu < BUFSIZE) break;
	}
	if(archive!=1)
		printf("v %s\n", et.path);
	close(verrou);
	close(input);
}
void ecrit_rep(char *namerep,int archive,struct stat st,char *root,int newRoot){
	entete et;
	et.file_length = 0;
	et.mode = st.st_mode;
	et.m_time = st.st_mtime;
	et.a_time = st.st_atime;
	strcpy(et.checksum,"\0");
	if(newRoot==1&&(strcmp(root,namerep)!=0)){
		et.path_length = strlen(namerep)+strlen(root)+1;
		sprintf(et.path,"%s/%s",root,namerep);
		et.path[et.path_length]='\0';
	}else{
		et.path_length = strlen(namerep);
		strcpy(et.path,namerep);
		et.path[strlen(namerep)]='\0';
	}
	safeWrite_entete(archive,et);
	if(archive!=1)
		printf("v %s\n", et.path);
}

void archive_rep(char *fichier,int archive,uid_t user,gid_t group,int mode,char *racine,int check,int source,int newRoot){
	DIR *dir;
	char tmp[PATH_MAX];
	int longueur;

	dir = opendir(fichier);
	if(!dir){
		if (errno== EACCES) {
			/* Accès interdit */
			fprintf(stderr,"mytar : %s: acces impossible  \n",fichier);
			exit(0);
		}
		else if(errno == ENOENT){
			fprintf(stderr,"mytar : %s: reference non valide \n",fichier );
			exit(0);
		}
		else 
			exit(0);
	}
	else{

		while(1){
			struct dirent *entry;
			entry = readdir(dir);

			if(!entry)
				break;
			if(entry->d_type & DT_DIR){

				struct stat f_stat;
				lstat(fichier, &f_stat);
				if(source==1){	
					ecrit_rep(fichier,archive,f_stat,racine,newRoot);
					source = 0;
				}else if(source==1){
					ecrit_rep(fichier,archive,f_stat,racine,newRoot);
					source = 0;
				}
				if(entry->d_name[0]!='.'){
					longueur = strlen(fichier);
					if(fichier[longueur-1]=='/'){
						sprintf(tmp,"%s%s",fichier,entry->d_name);
    				}
    				else
						sprintf(tmp,"%s/%s",fichier,entry->d_name);	
				if(verifieDroits(f_stat,group,user)){
						if(mode==0){
							ecrit_rep(tmp,archive,f_stat,racine,newRoot);
						}
						archive_rep(tmp,archive,user,group,mode,racine,check,0,newRoot);
				}
				else
					fprintf(stderr,"mytar : %s : acces impossible \n",entry->d_name);
				}
			}
			else if(mode){
				if(entry->d_name[0]!='.'){
					struct stat f_stat;
					longueur = strlen(fichier);
					if(fichier[longueur-1]=='/'){
						sprintf(tmp,"%s%s",fichier,entry->d_name);
					}
    				else
    					sprintf(tmp,"%s/%s",fichier,entry->d_name);	
    				lstat(tmp, &f_stat);
    				if(S_ISLNK(f_stat.st_mode)&& mode == 2){
    					archive_slink(tmp,archive,racine,newRoot);
    				}
    				else if(S_ISREG(f_stat.st_mode) && mode == 1){
    					archive_file(tmp,archive,check,racine,newRoot);
    				}
    				else{
    					if((S_ISLNK(f_stat.st_mode)&& mode != 2) || (S_ISREG(f_stat.st_mode) && mode != 1 ) )
    						continue;
    					else{
    						fprintf(stderr, "Mauvais format de fichier %s \n",fichier);
        					exit (EXIT_FAILURE);
    					}	
    				}
				}
			}
		}
	}
	if (closedir(dir)) {
        fprintf(stderr, "le repertoire n a pas pu etre ferme '%s': %s\n",
                 fichier, strerror (errno));
        exit (EXIT_FAILURE);
    }
}
void ajoute_fichiers(char *liste[],int taille,int check){
	struct stat st;
	int archive,i,verrou;
	struct flock fl;

	if((archive = open(liste[0], O_WRONLY | O_APPEND)) < 0)
		err(1,"4 impossible d ouvrie l archive %s \n ",liste[0]);
	if((verrou = open(liste[0],O_WRONLY | O_APPEND))<0){
		warn(" x impossible de poser un verrou %s \n",liste[0]);
		return;
	}
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_CUR;
	fl.l_start=0;
	fl.l_len=0;
	fl.l_pid = getpid();
	if( fcntl(verrou, F_SETLKW, &fl) == -1 ){
		err(1," verrou probleme %s \n",liste[0]);
	}

	for(i=1;i<taille;i++){
		if((stat(liste[i],&st) != 0)){
			warn(" 5 impossible d archiver %s \n",liste[i]);
			continue;
		}
		else{
			if(S_ISREG(st.st_mode)){	
				archive_file(liste[i],archive,check,NULL,0);
			}
			else{
				fprintf(stderr, "Mauvais USAGE -a fichier ordinaire %s \n",liste[i]);
        		exit (EXIT_FAILURE);
			}
		}
	}

	close(verrou);
	close(archive);
}

/*fonction d archivage elle prend en paramettre la liste de fichier a archiver leurs nombre et les eventuelles options a appliquer*/
void archive(char *liste[],int taille,int options[],char *nouvelleRacine,int entreesStandar){
	struct stat st;
	struct flock fl;
	int archive,i,mode,verrou,sln,first=1,newRoot=0;
	uid_t user;
	gid_t group;
	user = getuid(); 
	group = getgid();

	if(entreesStandar==0){
		if((archive = open(liste[0],O_WRONLY | O_CREAT | O_TRUNC,0666)) < 0)
		err(1,"x impossible d ouvrie l archive %s \n",liste[0]);
		if((verrou = open(liste[0],O_WRONLY | O_CREAT | O_TRUNC,0666))<0){
			warn(" x impossible de poser un verrou %s \n",liste[0]);
			return;
		}
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_CUR;
		fl.l_start=0;
		fl.l_len=0;
		fl.l_pid = getpid();
		if( fcntl(verrou, F_SETLKW, &fl) == -1 ){
			err(1," verrou probleme %s \n",liste[0]);
		}
	}else{
		archive =1;
	}
	if(options[0]==1){
		sln = 3;
	}else sln = 2;
	if(options[3]==1){
		newRoot = 1;
		if(entreesStandar==0){
			if((lstat(liste[0],&st) != 0)){
				warn("x impossible d archiver %s \n",liste[0]);
			}
			st.st_mode =0777;
			ecrit_rep(nouvelleRacine,archive,st,nouvelleRacine,1);
		}else{

			time(&(st.st_mtime));
			time(&(st.st_atime));
			st.st_mode =0777;

			ecrit_rep(nouvelleRacine,archive,st,nouvelleRacine,1);		
		}
		
		
	}
	for(mode=0; mode<sln; mode++){

		for(i=1;i<taille;i++){
			if(mode!=2){

				if((lstat(liste[i],&st) != 0)){
					warn("x impossible d archiver %s \n",liste[i]);
					continue;
				}
				else{
					if(S_ISREG(st.st_mode)&& mode==1){
						archive_file(liste[i],archive,options[2],nouvelleRacine,options[3]);
					}
					else if(S_ISLNK(st.st_mode)&& mode==2){
						archive_slink(liste[i],archive,nouvelleRacine,options[3]);
					}
					else if(S_ISDIR(st.st_mode) ){
						
						if(liste[i][strlen(liste[i])-1]=='/')
							liste[i][strlen(liste[i])-1]='\0';
						archive_rep(liste[i],archive,user,group,mode,nouvelleRacine,options[2],first,newRoot);
						
					}
				}
			}
			else if(mode==2){
				if((lstat(liste[i],&st) != 0)){
					warn("x impossible d archiver %s \n",liste[i]);
					continue;
				}
				else{
					if(S_ISREG(st.st_mode)&& mode==1){
						archive_file(liste[i],archive,options[2],nouvelleRacine,options[3]);
					}
					else if(S_ISLNK(st.st_mode)&& mode==2){
							archive_slink(liste[i],archive,nouvelleRacine,options[3]);
					}
					else if(S_ISDIR(st.st_mode)){
						if(liste[i][strlen(liste[i])-1]=='/')
							liste[i][strlen(liste[i])-1]='\0';
						archive_rep(liste[i],archive,user,group,mode,nouvelleRacine,options[2],first,newRoot);
						if(first==1)first =0;
					}
				}
			}	
		}
		if(first==1)first =0;
	}
	if(entreesStandar==0)
		close(verrou);
	close(archive);
}

