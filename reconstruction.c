#include "variables.h"
#include "utiles.h"


/*on compare le chemin du fichier lu avec celui donne par racine*/
int verifie_extraction(entete et,char *racine){
	int i,profondeur_courant,profondeur_racine;
	profondeur_courant = profondeur_chemin(et.path);
	profondeur_racine = profondeur_chemin(racine);
	/*	si la profondeur du repertoire donne est inferieur a celle de la racine alors cela signifie 
		que l elements ne se trouve pas dans l arborescence l element doit donc etre ignore */
	
	if(profondeur_courant < profondeur_racine) return 0;
	else if(profondeur_courant >= profondeur_racine){
		/*si les profondeures sont les memes alors on verifie que le chemin donnee par la racine est le meme */
		i=0;
		while(i<(int)strlen(racine)){
			if(et.path[i]!=racine[i]){
				/*il ne sagit pas de la meme racine on ne valide pas */
				return 0;
			}
			else
				i = i + 1;
		}
		if((int)strlen(racine)==(int)strlen(et.path)){
			printf("fichier supprime : %s fichier lu : %s \n",racine,et.path);
			return 1;
		}
		else{
			if(et.path[strlen(racine)]=='/'){
				printf("fichier supprime : %s fichier lu : %s \n",racine,et.path);
				return 1;
			}
		}
		/*la racine est commune on peut donc validee l extraction */
		
		return 0;
	}
	return 0;
}
/*permet la reconstruction de l archive */
void reecriture(int archive,off_t delta,entete et){
	char buffeur[BUFSIZE];
	int courant,lu;
	off_t decrm;
	if(et.file_length>BUFSIZE){
		courant = BUFSIZE;
		decrm = et.file_length - BUFSIZE;
	}
	else{
		decrm = 0;
		courant = (int)et.file_length;
	}
	while(1){
		if((lu = read(archive,buffeur,(courant)))< 0){
			fprintf(stderr, "erreur lecture de l archive  \n");
			exit(EXIT_FAILURE);
		}
		/*calcul de la delta de la tete */
		/*delta pour ecire */
		lseek(archive,((delta + courant)*(-1)),SEEK_CUR);
		safeWrite(archive,buffeur,courant);
		/*redelta pour lire */
		lseek(archive,(delta),SEEK_CUR);
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
}

/*modifie l archive */
void supprime_elements(char *liste[],int taille){
	entete et;
	int archive,lu,verrou;
	struct flock fl;
	off_t retour;
	if((archive = open(liste[0],O_RDWR)) < 0)
		err(1,"impossible d ouvrie l archive %s \n",liste[0]);
	if((verrou = open(liste[0],O_RDWR))<0){
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
	lu=1;
	if(taille>=1){

		retour = 0;
		while(lu!=0){
			lu=0;
			lu = safeRead(archive,&et,liste[0]);

			if(lu==0) break;
			
			/* si verifie extraction renvoie un alors on calcule l espace a suprimmer et on avance de taille du champ file*/
			if(verifie_extraction(et,liste[1])==1){
				
				if(et.file_length!=0){//cela signifie que nous somme sur un fichier 
					retour = retour + tailleentete(et) + et.file_length;
					lseek(archive,et.file_length,SEEK_CUR);
				}
				else{//c est un repertoire car les repertoires ont pour taille 0
					retour = retour + tailleentete(et);
				}
			}
			else if(retour > 0){

				if(et.file_length!=0){//cela signifie que nous somme sur un fichier 
					/*retour en a la position originale + le hedear puisque il vient d etre lu il faut le soustraire maintenant*/
					lseek(archive,((retour+tailleentete(et)) *(-1)),SEEK_CUR);
					/*on ecrit donc a l ancienne position */

					safeWrite_entete(archive,et);
					lseek(archive,((retour)),SEEK_CUR);
					/*repositionement pour la prochaine lecture a la place plus le delta */
					reecriture(archive,(retour),et);
				}
				else{//c est un repertoire car les repertoires ont pour taille 0
					lseek(archive,((retour+tailleentete(et)) *(-1)),SEEK_CUR);
					safeWrite_entete(archive,et);
					lseek(archive,retour,SEEK_CUR);
				}
			}
			else{
				if(et.file_length!=0)//cela signifie que nous somme sur un fichier 
					lseek(archive,et.file_length,SEEK_CUR);
			}
		}
		/*on enleve le surplu pour reduire la taille du fichier */
		ftruncate(archive,lseek(archive,0,SEEK_CUR)-retour);
	}
	close(verrou);
	close(archive);
}
