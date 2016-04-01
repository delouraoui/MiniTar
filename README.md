# Mini-tar

# A LIRE 


utilisation du programme

Le programme ce compile par la commande make

A l'éxécution il est possible de combiner plusieurs options entre elles </br>

énumérons les différentes possiblité du programme : </br></br>

en mode creation : </br>
	./mytar -c nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers sur la sortie standard </br>
	./mytar -c -f nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers dans l archive au format .mtr</br>
	./mytar -c -s -f nom_archive [PATH1 ... PATHN ] -> avec l'ajout de l'option -s on indique que l'on archive aussi les liens symboliques</br>
	./mytar -c -l -f nom_archive [PATH1 ... PATHN ] -> après l'archivage le contenue de l'achive est listé sous la forme ls -l</br>
	./mytar -c -v -f nom_archive [PATH1 ... PATHN ] -> permet de caculer l'empreinte md5 de chaque fichier</br>
	./mytar -c -z -f nom_archive [PATH1 ... PATHN ] -> permet de compresser l'archive au format .gz</br>
	./mytar -c -C <rep> -f nom_archive [PATH1 ... PATHN ] -> indique au que <rep> est la racine de l'archive</br>
toutes ces options peuvent être combinné entre elles par exemple  ./mytar -c -C <rep> -z -f nom_archive [PATH1 ... PATHN ]</br>
mais il est important que -c soit donné en premiere option sans quoi le programme ne connaitra pas son mode</br></br>

en mode extraction : </br>
	./mytar -x < <nom_archive> -> cette commande va archiver le fichier lu depuis l'entrée standard</br>
	./mytar -x -f nom_archive [PATH1 ... PATHN ]-> cette commande va archiver les n fichiers dans l archive au format .mtr</br>
	./mytar -x -s -f nom_archive [PATH1 ... PATHN ] -> avec l'ajout de l'option -s on indique que l'on désarchive aussi les liens symboliques</br>
	./mytar -x -l -f nom_archive [PATH1 ... PATHN ] -> après le désarchivage le contenue de l'achive est listé sous la forme ls -l</br>
	./mytar -x -v -f nom_archive [PATH1 ... PATHN ] -> permet de vérifier l'intégriter chaque fichier</br>
	./mytar -x -z -f nom_archive [PATH1 ... PATHN ] -> permet de désarchiver une archive compresser au format .gz</br>
	./mytar -x -k -f nom_archive [PATH1 ... PATHN ] -> permet de désarchiver une sans écraser les fichier éxistant</br>
	./mytar -x -C <rep> -f nom_archive [PATH1 ... PATHN ] -> indique au que <rep> sera le répertoire ou l'archive va être désarchivé </br></br>
	les combinaison sont possible à condition toujours de préciser -x comme première option</br>

./mytar -a [PATH1 ... PATHN ] -f <archive>  ajoute les n fichiers à l'archive $</br>

./mytar -a -l -f <archive> [PATH1 ... PATHN ] la meme chose mais affiche le contenu de l'archive après l'opération</br>

./mytar -a -v -f <archive> [PATH1 ... PATHN ] la meme chose et permet le calcul de l'empreinte md5
 -v et -l peuvent se combiner avec -a</br>

./mytar -l -f <archive> tout seul affiche le contenu de l'archive au format ls -l</br></br>

./mytar -l -f <archive> [pathDansLarchive] si l’on spécifie un chemin dans l’achive il affiche soit les elements du sous répertoire ou le fichier si c’est un fichier qui est spécifié</br>
./mytar -d <element> -f <archive> supprime de l'archive l'elements passé en parramètre</br>

L'ensemble du projet à été traité ainsi que l'extention de compression </br>

l'ensemble des fonctionnalités ont été testé avec valgrind</br>

