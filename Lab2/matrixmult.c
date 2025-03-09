#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void matMult(int matA[3][3], int matB[3][3], int** result);
int main(int argc, char *argv[])
{



    // Step 1: Allocate memory for the row pointers
    int **array = (int **)malloc(3 * sizeof(int *));
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Step 2: Allocate memory for each row
    for (int i = 0; i < 3; i++) {
        array[i] = (int *)malloc(3 * sizeof(int));
        if (array[i] == NULL) {
            printf("Memory allocation failed for row %d!\n", i);
            // Free previously allocated memory before exiting
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            exit(1);
        }
    }


    

    int matA[3][3] = {
        {1,2,3},
        {4,5,6},
        {2,3,0},
    };

    int matB[3][3] = {
        {1,2,3},
        {4,5,6},
        {2,3,0},
    };
 



    /*
    for(int i = 0; i < 3 ; i++){


    for(int j = 0; j < 3; j++){
        
    
    result[i][j] = 0;
    for (int k = 0; k < 3; k++){
        
    result[i][j] += matA[i][k]*matB[k][j];
    }
    
    }
    
    }
    */
   
   
   
   matMult(matA,matB,array);





    for(int i = 0; i < 3 ; i++){


        for(int j = 0; j < 3; j++){

            

      

         printf("%d,\t", array[i][j]);  
            
            
            
        }

        printf("\n");

    }





    return 0;
}







void matMult(int matA[3][3], int matB[3][3],int **result ){



    for(int i = 0; i < 3 ; i++){


        for(int j = 0; j < 3; j++){

            
            result[i][j] = 0;
            for (int k = 0; k < 3; k++){

                result[i][j] += matA[i][k] * matB[k][j];
            }
           
        }

    }


   
}