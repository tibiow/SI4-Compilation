REPONSES A LA QUESTION 1 (Chaînes de caractères)
================================================

Consignes: 
  - Essayez de ne pas trop modifier la structure de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux. 


Sous-question 1: Explications sommaires sur les modifications apportées
-----------------------------------------------------------------------

Globalement le changement va être de gérer les string non plus une par une mais par plusieurs. Il faut donc manipuler des listes pour pouvoir utiliser la concaténation statique. 





Sous-question 2: Fichier(s) modifiés
------------------------------------


### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________



Pour la concaténation statique il  n'est actuellement pas possible de mettre 2 String bout à bout. Je change donc mon syntax.y de manière à ce que je puisse rajouter une ou plusieurs String. Pour ce faire je change dans mon non-terminal "expr" "STRING" pour l'emmener vers un nouveau non-terminal "stringconc" pour pouvoir lui faire prendre 1 ou plusieurs strings. 
De cette manière je peux récupérer une liste de string qui me donnera la possibilité de concaténer le tout grâce à la production de code.



### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


maintenant une string prend en compte plusieurs string bout à bout et donc il faut que je gère une liste de string. Il faut que je change les méthodes pour pouvoir en gérer plusieurs plutot qu'une. 


### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'



### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


Ici on gérait seulement le cas d'une string. Maintenant il faut juste rajouter au même niveau un enchainement de _my_concat (voir runtime) pour pouvoir former la string dont j'ai besoin.
Je concat donc mon 1er élément avec mon 2 ème, que je concatère avec le 3ème etc ...




### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________



Je rajoute la _my_concat(char* s1,char* s2) qui permet de rajouter à la première string le contenu de la deuxième string.




### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________

'PAS DE MODIFICATION'




