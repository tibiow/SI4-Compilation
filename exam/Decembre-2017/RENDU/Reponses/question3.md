REPONSES A LA QUESTION 3 (CONSTANTES)
==============================================

Consignes: 
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux. 


Sous-question 1: Principe de la fonctionnement de l'extension
-------------------------------------------------------------

Ce qu'il faut comprendre c'est qu'un const n'est rien de plus que l'initialisation d'une variable que l'on ne peut modifier. 
Il faut donc instancier la nouvelle variable en reconnaissant que cette une constante de manière à interdire son utilisation avec certains opérateurs.
Entre autre les "++", "--", "=","read" quand la varaible se trouve du coté gauche de l'opérateur...

Pour ce faire on va d'abord rajouter au syntaxique et au lexical le nouveau mot clé "const". 
Ensuite il faudra changer l'ast pour rajouter un champ permettant de déterminer si la variable est constante ou non.
Puis l'analyser devra vérifier qu'aucune opérations ne peut altérer la variable.



Sous-question 2: Fichier(s) modifiés
------------------------------------



### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


On rajoute le "const" au lexical pour qu'il puisse reconnaitre le mot clé.
"const"         return KCONST;





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

On rajoute une statement correspondant au const. suite à une déclaration de variable classique. 





### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


On rajoute dans les identifiants un nouveau champ dans la struct var_decl permettant de spécifié si la variable est accessible en lecture ou en lecture/écriture.
Il faut alors compléter ce nouveau champ dans les fonctions désignés pour l'enregistrement en mémoire des variables.
Pour cela on recrée de nouvelle fonctions qui 
C'est à ce moment que l'on définie si notre variable est "writable" ou "readable" en fonction de ce que nous renvoie le syntaxique.
Si un "const" était devant alors la déclaration se ferra en "readable" sinon elle sera "writable".




### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


Puisque dans l'ast on s'est occupé de mettre en place notre nouveau champ pour l'écriture ou non de la variable, il en vient à notre analyseur d'interdire les actions d'écriture sur la variable.

Je me suis bloqué à partir d'ici. Je n'arrive pas à voir comment récupérer ce que j'ai crée dans mon ast pour voir si ma variable est writable ou non.

Dans le cas où j'arrive à récupérer cette information il me suffit dans la fonction analysis_expression() de mettre une condition à l'endroit où on fait les tests pour les opérateurs "++", "--" (que ce soit pour la post incrémentation ou la pré incrémentation) et "=" ... 

condition :

if(!AST_XXX(op1)){
	error_msg(node, "first operand is const and can't be changed");
}





### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


Une seule fonction est rajouté dans la production de code.

void produce_code_const(ast_node *node) {
  emit("const");
}




### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'






### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________


'PAS DE MODIFICATION'



