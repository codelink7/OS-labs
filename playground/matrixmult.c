#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

// Global matrices
int **matrixA, **matrixB, **matrixC_per_matrix, **matrixC_per_row, **matrixC_per_element;
int rowsA, colsA, rowsB, colsB;

// Structure for per-row thread data
typedef struct {
    int row;
} RowData;

// Structure for per-element thread data
typedef struct {
    int row;
    int col;
} ElementData;

// Function to read matrix from file
int** readMatrix(const char* filename, int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }

    // Read dimensions
    if (fscanf(file, "row=%d col=%d\n", rows, cols) != 2) {
        printf("Error reading dimensions from %s\n", filename);
        fclose(file);
        exit(1);
    }

    // Allocate memory for matrix
    int** matrix = (int**)malloc(*rows * sizeof(int*));
    for (int i = 0; i < *rows; i++) {
        matrix[i] = (int*)malloc(*cols * sizeof(int));
    }

    // Read matrix elements
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                printf("Error reading matrix element at %d,%d from %s\n", i, j, filename);
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);
    return matrix;
}

// Function to write matrix to file
void writeMatrix(const char* filename, int** matrix, int rows, int cols) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s for writing\n", filename);
        exit(1);
    }

    // Write dimensions
    fprintf(file, "row=%d col=%d\n", rows, cols);

    // Write matrix elements
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Function to free matrix memory
void freeMatrix(int** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Thread function for per-matrix method
void* multiplyPerMatrix(void* arg) {
    // Compute the entire resulting matrix
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            matrixC_per_matrix[i][j] = 0;
            for (int k = 0; k < colsA; k++) {
                matrixC_per_matrix[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    return NULL;
}

// Thread function for per-row method
void* multiplyPerRow(void* arg) {
    RowData* data = (RowData*)arg;
    int row = data->row;

    // Compute one row of the resulting matrix
    for (int j = 0; j < colsB; j++) {
        matrixC_per_row[row][j] = 0;
        for (int k = 0; k < colsA; k++) {
            matrixC_per_row[row][j] += matrixA[row][k] * matrixB[k][j];
        }
    }

    free(data); // Free the allocated structure
    return NULL;
}

// Thread function for per-element method
void* multiplyPerElement(void* arg) {
    ElementData* data = (ElementData*)arg;
    int row = data->row;
    int col = data->col;

    // Compute one element of the resulting matrix
    matrixC_per_element[row][col] = 0;
    for (int k = 0; k < colsA; k++) {
        matrixC_per_element[row][col] += matrixA[row][k] * matrixB[k][col];
    }

    free(data); // Free the allocated structure
    return NULL;
}

int main(int argc, char* argv[]) {
    char input1[100] = "a.txt";
    char input2[100] = "b.txt";
    char output[100] = "c";
    
    // Parse command line arguments
    if (argc == 4) {
        strcpy(input1, argv[1]);
        strcat(input1, ".txt");
        strcpy(input2, argv[2]);
        strcat(input2, ".txt");
        strcpy(output, argv[3]);
    }

    // Read input matrices
    matrixA = readMatrix(input1, &rowsA, &colsA);
    matrixB = readMatrix(input2, &rowsB, &colsB);

    // Check if matrices can be multiplied
    if (colsA != rowsB) {
        printf("Error: Matrices cannot be multiplied. Incompatible dimensions.\n");
        freeMatrix(matrixA, rowsA);
        freeMatrix(matrixB, rowsB);
        return 1;
    }

    // Allocate memory for result matrices
    matrixC_per_matrix = (int**)malloc(rowsA * sizeof(int*));
    matrixC_per_row = (int**)malloc(rowsA * sizeof(int*));
    matrixC_per_element = (int**)malloc(rowsA * sizeof(int*));
    
    for (int i = 0; i < rowsA; i++) {
        matrixC_per_matrix[i] = (int*)malloc(colsB * sizeof(int));
        matrixC_per_row[i] = (int*)malloc(colsB * sizeof(int));
        matrixC_per_element[i] = (int*)malloc(colsB * sizeof(int));
    }

    struct timeval start, stop;
    pthread_t thread;
    int thread_count;

    // Method 1: A thread per matrix
    printf("\nMethod 1: A thread per matrix\n");
    thread_count = 1;
    
    gettimeofday(&start, NULL);
    pthread_create(&thread, NULL, multiplyPerMatrix, NULL);
    pthread_join(thread, NULL);
    gettimeofday(&stop, NULL);
    
    printf("Number of threads: %d\n", thread_count);
    printf("Seconds taken: %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    // Write result to file
    char output_filename[256];
    sprintf(output_filename, "%s_per_matrix.txt", output);
    writeMatrix(output_filename, matrixC_per_matrix, rowsA, colsB);

    // Method 2: A thread per row
    printf("\nMethod 2: A thread per row\n");
    thread_count = rowsA;
    pthread_t row_threads[rowsA];
    
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < rowsA; i++) {
        RowData* data = (RowData*)malloc(sizeof(RowData));
        data->row = i;
        pthread_create(&row_threads[i], NULL, multiplyPerRow, (void*)data);
    }
    
    for (int i = 0; i < rowsA; i++) {
        pthread_join(row_threads[i], NULL);
    }
    
    gettimeofday(&stop, NULL);
    
    printf("Number of threads: %d\n", thread_count);
    printf("Seconds taken: %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    // Write result to file
    sprintf(output_filename, "%s_per_row.txt", output);
    writeMatrix(output_filename, matrixC_per_row, rowsA, colsB);

    // Method 3: A thread per element
    printf("\nMethod 3: A thread per element\n");
    thread_count = rowsA * colsB;
    pthread_t** element_threads = (pthread_t**)malloc(rowsA * sizeof(pthread_t*));
    for (int i = 0; i < rowsA; i++) {
        element_threads[i] = (pthread_t*)malloc(colsB * sizeof(pthread_t));
    }
    
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            ElementData* data = (ElementData*)malloc(sizeof(ElementData));
            data->row = i;
            data->col = j;
            pthread_create(&element_threads[i][j], NULL, multiplyPerElement, (void*)data);
        }
    }
    
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            pthread_join(element_threads[i][j], NULL);
        }
    }
    
    gettimeofday(&stop, NULL);
    
    printf("Number of threads: %d\n", thread_count);
    printf("Seconds taken: %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    // Write result to file
    sprintf(output_filename, "%s_per_element.txt", output);
    writeMatrix(output_filename, matrixC_per_element, rowsA, colsB);

    // Free element_threads memory
    for (int i = 0; i < rowsA; i++) {
        free(element_threads[i]);
    }
    free(element_threads);

    // Free matrix memory
    freeMatrix(matrixA, rowsA);
    freeMatrix(matrixB, rowsB);
    freeMatrix(matrixC_per_matrix, rowsA);
    freeMatrix(matrixC_per_row, rowsA);
    freeMatrix(matrixC_per_element, rowsA);

    return 0;
}

*/

void matMult(int **matA, int **matB, int** result, int ROWA, int COLA, int ROWB, int COLB);
int main(int argc, char *argv[])
{



    FILE* file = fopen("a.txt", "r");

    



    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    

    fscanf(file, );



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
   
   
   
   // matMult(matA,matB,array);







    return 0;
}

// void matMult(int matA[3][3], int matB[3][3], int **result)
// {

//     for (int i = 0; i < 3; i++)
//     {

//         for (int j = 0; j < 3; j++)
//         {

//             result[i][j] = 0;
//             for (int k = 0; k < 3; k++)
//             {

//                 result[i][j] += matA[i][k] * matB[k][j];
//             }
//         }
//     }
// }