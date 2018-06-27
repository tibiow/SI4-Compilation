REPONSES A LA QUESTION 4 (TRACES)
=================================

Consignes: 
  - Essayez de ne pas trop modifier la stucture de ce fichier
  - Vous pouvez copier/coller les parties *significatives* de vos
    modifications (ne copiez des fichiers entiers!!!)
  - le code c'est bien, mais s'il est expliqué c'est mieux. 


Sous-question 1: Principe de fonctionnement
-------------------------------------------

Ici on ne cherche qu'à récupérer la trace du programme que l'on éxecute. Il suffit donc de mettre des print dans le code au bon endroit pour avoir les informations que l'on cherche. Il faut donc changer la production de code pour faire apparaitre tous les logs voulu. Il faut faire attention à ce que la variable debug_mode soit bien à 1 pour produire le code supplémentaire à injecter à chaque fois qu'un appel à une fonction est effectué. 
Il va falloir vérifier le type de retour de la fonction à chaque fois. En effet les fonctions ENTER_VOID, LEAVE_VOID et RETURN sont construites pour les fonctions qui ne retournent rien alors que les fonction ENTER_FUNC, LEAVE_FUNC et RETURN_VALUE sont faites pour toutes les autres fonctions. 
 



Sous-question 2: Fichier(s) modifiés
------------------------------------



### lexical.l: (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'





### syntax.y:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'





### ast.c/ast.h:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________

'PAS DE MODIFICATION'





### analysis.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


'PAS DE MODIFICATION'




### prodcode.c:  (indiquer 'PAS DE MODIFICATION' si le fichier n'est pas modifié)
_______________________________________________________________________________


Il y a 2 fonctions à changer:
-produce_code_return_statement
-produce_code_call

pour le return:

on s'occuppe du return et du leave pour les 2 types de fonctions

  if(debug_mode){
    if(AST_TYPE(method)==void_type){
      emit("RETURN()");
      emit("LEAVE_VOID()");

    }
    else{
      emit("RETURN_VALUE(");code_expr_cast(n->expr); emit(")");
      emit("LEAVE_FUNC(");code(AST_TYPE(method)); emit(")");
    }
  }
pour le function:

on s'occupe du enter pour les 2 types de fonctions.
	




### Modifications dans le runtime? (indiquer 'PAS DE MODIFICATION' si non modifié)
_______________________________________________________________________________



'PAS DE MODIFICATION'




### Autres modifications? (indiquer 'PAS DE MODIFICATIONS' dans le cas contraire)
_______________________________________________________________________________

'PAS DE MODIFICATION'




