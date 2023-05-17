/*------------------------------------------------------------ 
    A less bloated version of GNU Yes - output a string
		repeatedly until killed.

Author: Logan Goins
------------------------------------------------------------*/

#include <stdio.h>

int main(int argc, char** argv){

    if(argc > 1){
	while(1){
	    for(int i = 1; i < 1024; i++){
		if(argv[i] == NULL)
		    break;
		printf("%s ", argv[i]);
	    }
	    printf("\n");
	}
    }
    else{
	while(1){
	    printf("y\n");
	}
    }
    return 0;
}
