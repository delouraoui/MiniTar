# Mini-tar

################################### A LIRE #############################################


utilisation du programme

Le programme ce compile par la commande make

A l'éxécution il est possible de combiner plusieurs options entre elles 

énumérons les différentes possiblité du programme : 

en mode creation : 
	./mytar -c nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers sur la sortie standard
	./mytar -c -f nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers dans l archive au format .mtr
	./mytar -c -s -f nom_archive [PATH1 ... PATHN ] -> avec l'ajout de l'option -s on indique que l'on archive aussi les liens symboliques
	./mytar -c -l -f nom_archive [PATH1 ... PATHN ] -> après l'archivage le contenue de l'achive est listé sous la forme ls -l
	./mytar -c -v -f nom_archive [PATH1 ... PATHN ] -> permet de caculer l'empreinte md5 de chaque fichier
	./mytar -c -z -f nom_archive [PATH1 ... PATHN ] -> permet de compresser l'archive au format .gz
	./mytar -c -C <rep> -f nom_archive [PATH1 ... PATHN ] -> indique au que <rep> est la racine de l'archive
toutes ces options peuvent être combinné entre elles par exemple  ./mytar -c -C <rep> -z -f nom_archive [PATH1 ... PATHN ]
mais il est important que -c soit donné en premiere option sans quoi le programme ne connaitra pas son mode

en mode extraction : 
	./mytar -x < <nom_archive> -> cette commande va archiver le fichier lu depuis l'entrée standard
	./mytar -x -f nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers dans l archive au format .mtr
	./mytar -x -s -f nom_archive [PATH1 ... PATHN ] -> avec l'ajout de l'option -s on indique que l'on désarchive aussi les liens symboliques
	./mytar -x -l -f nom_archive [PATH1 ... PATHN ] -> après le désarchivage le contenue de l'achive est listé sous la forme ls -l
	./mytar -x -v -f nom_archive [PATH1 ... PATHN ] -> permet de vérifier l'intégriter chaque fichier
	./mytar -x -z -f nom_archive [PATH1 ... PATHN ] -> permet de désarchiver une archive compresser au format .gz
	./mytar -x -k -f nom_archive [PATH1 ... PATHN ] -> permet de désarchiver une sans écraser les fichier éxistant
	./mytar -x -C <rep> -f nom_archive [PATH1 ... PATHN ] -> indique au que <rep> sera le répertoire ou l'archive va être désarchivé 
	les combinaison sont possible à condition toujours de préciser -x comme première option

./mytar -a [PATH1 ... PATHN ] -f <archive>  ajoute les n fichiers à l'archive $

./mytar -a -l -f <archive> [PATH1 ... PATHN ] la meme chose mais affiche le contenu de l'archive après l'opération

./mytar -a -v -f <archive> [PATH1 ... PATHN ] la meme chose et permet le calcul de l'empreinte md5
 -v et -l peuvent se combiner avec -a

./mytar -l -f <archive> tout seul affiche le contenu de l'archive au format ls -l

./mytar -l -f <archive> [pathDansLarchive] si l’on spécifie un chemin dans l’achive il affiche soit les elements du sous répertoire ou le fichier si c’est un fichier qui est spécifié
./mytar -d <element> -f <archive> supprime de l'archive l'elements passé en parramètre

L'ensemble du projet à été traité ainsi que l'extention de compression 

l'ensemble des fonctionnalités ont été testé avec valgrind sur le serveur lucien de l'université 

################################### A LIRE #############################################
