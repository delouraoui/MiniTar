# Mini-tar

# A LIRE 


utilisation du programme

Le programme se compile par la commande make

A l'éxécution il est possible de combiner plusieurs options entre elles </br>

énumérons les différentes possiblités du programme : </br></br>

en mode creation : </br>
<ul>
	<li>./mytar -c nom_archive [PATH1 ... PATHN ]-> </br>cette commande va archiver les n fichiers sur la sortie standard </li>
	<li>./mytar -c -f nom_archive [PATH1 ... PATHN ]-> </br>cette commande va archiver les n fichiers dans l archive au format .mtr</li>
	<li>./mytar -c -s -f nom_archive [PATH1 ... PATHN ] -> </br>avec l'ajout de l'option -s on indique que l'on archive aussi les liens symboliques</li>
	<li>./mytar -c -l -f nom_archive [PATH1 ... PATHN ] -> </br>après l'archivage le contenue de l'achive est listé sous la forme ls -l</li>
	<li>./mytar -c -v -f nom_archive [PATH1 ... PATHN ] -> </br>permet de caculer l'empreinte md5 de chaque fichier</li>
	<li>./mytar -c -z -f nom_archive [PATH1 ... PATHN ] -> </br>permet de compresser l'archive au format .gz</li>
	<li>./mytar -c -C [rep] -f nom_archive [PATH1 ... PATHN ] -> </br>indique un repertoire qui est la racine de l'archive
toutes ces options peuvent être combinné entre elles par exemple  </li>
	<li>./mytar -c -C [rep] -z -f nom_archive [PATH1 ... PATHN ]</li>
	</ul>	
mais il est important que -c soit donné en premiere option sans quoi le programme ne connaitra pas son mode</br></br>

en mode extraction : 
<ul>
	<li>./mytar -x < [nom_archive] -> </br>cette commande va archiver le fichier lu depuis l'entrée standard</li>
	<li>./mytar -x -f nom_archive [PATH1 ... PATHN ]-> </br>
	cette commande va archiver les n fichiers dans l archive au format .mtr</li>
	<li>./mytar -x -s -f nom_archive [PATH1 ... PATHN ] -> </br>
	avec l'ajout de l'option -s on indique que l'on désarchive aussi les liens symboliques</li>
	<li>./mytar -x -l -f nom_archive [PATH1 ... PATHN ] -> </br>
	après le désarchivage le contenue de l'achive est listé sous la forme ls -l</li>
	<li>./mytar -x -v -f nom_archive [PATH1 ... PATHN ] -></br> 
	permet de vérifier l'intégriter chaque fichier</li>
	<li>./mytar -x -z -f nom_archive [PATH1 ... PATHN ] -></br> 
	permet de désarchiver une archive compresser au format .gz</li>
	<li>./mytar -x -k -f nom_archive [PATH1 ... PATHN ] -> </br>
	permet de désarchiver une archive sans écraser les fichiers éxistants</li>
	<li>./mytar -x -C [rep] -f nom_archive [PATH1 ... PATHN ] -> </br>
	indique au que [rep] sera le répertoire ou l'archive va être désarchivé 
</ul>	
	</br>
	les combinaisons sont possibles à condition toujours de préciser -x comme première option</br>
<ul>
<li>./mytar -a [PATH1 ... PATHN ] -f <archive>  ajoute les n fichiers à l'archive </li>

<li>./mytar -a -l -f [archive] [PATH1 ... PATHN ] la meme chose mais affiche le contenu de l'archive après l'opération</li>

<li>./mytar -a -v -f [archive] [PATH1 ... PATHN ] la meme chose et permet le calcul de l'empreinte md5
 -v et -l peuvent se combiner avec -a </li>

<li>./mytar -l -f [archive] tout seul affiche le contenu de l'archive au format ls -l </li>

<li>./mytar -l -f [archive] [pathDansLarchive] si l’on spécifie un chemin dans l’achive il affiche soit les elements du sous répertoire ou</br> le fichier si c’est un fichier qui est spécifié</li>
<li>./mytar -d [element] -f [archive] supprime de l'archive l'element passé en paramètre</li>
</ul>

l'ensemble des fonctionnalités ont été testé avec valgrind</br>

