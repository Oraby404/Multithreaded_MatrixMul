#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

int **matrix_1, **matrix_2, **result_matrix;
int rows_m1, columns_m1;
int rows_m2, columns_m2;
//for row by row multiplication
int current_row = 0;
//for element by element multiplication
int current_thread = 0;

void *mul_rowByRow(void *arg) {
    int row = current_row++;
    for (int i = 0; i < columns_m2; i++)
        for (int j = 0; j < columns_m1; j++)
            result_matrix[row][i] += matrix_1[row][j] * matrix_2[j][i];
    return NULL;
}

void *mul_elementByElement(void *arg) {
    //covert 1d to 2d
    int row = current_thread / columns_m2;
    int col = current_thread % columns_m2;
    current_thread++;
    for (int j = 0; j < columns_m1; j++)
        result_matrix[row][col] += matrix_1[row][j] * matrix_2[j][col];
    return NULL;
}

int main() {

    char filename[255];
    printf("Enter the file Name : ");
    fflush(stdin);
    fgets(filename,255,stdin);
    filename[strlen(filename)-1]='\0';

    FILE *fp;
    fp = fopen(filename, "a+");

    if(!fp){
        printf("file doesn't exist!");
        exit(0);
    }

    //get the first matrix
    fscanf(fp, "%d %d", &rows_m1, &columns_m1);
    matrix_1 = malloc(sizeof(int *) * rows_m1);
    for (int i = 0; i < rows_m1; i++) {
        matrix_1[i] = malloc(sizeof(int) * columns_m1);
        for (int j = 0; j < columns_m1; j++) {
            fscanf(fp, "%d", &matrix_1[i][j]);
        }
    }

    //get the 2nd matrix
    fscanf(fp, "%d %d", &rows_m2, &columns_m2);
    matrix_2 = malloc(sizeof(int *) * rows_m2);
    for (int i = 0; i < rows_m2; i++) {
        matrix_2[i] = malloc(sizeof(int) * columns_m2);
        for (int j = 0; j < columns_m2; j++) {
            fscanf(fp, "%d", &matrix_2[i][j]);
        }
    }

    //allocate the output matrix
    if (columns_m1 == rows_m2) {
        result_matrix = malloc(sizeof(int *) * rows_m1);
        for (int i = 0; i < rows_m1; i++)
            result_matrix[i] = malloc(sizeof(int) * columns_m2);
    } else {
        printf("Wrong Dimensions!");
        exit(0);
    }

///////////////////////////////////////////////////////////////////////////////////////////////
    //row by row multiplication
    clock_t start = clock();
    pthread_t threads[rows_m1];

    for (int i = 0; i < rows_m1; i++)
        pthread_create(&threads[i], NULL, &mul_rowByRow, NULL);

    for (int i = 0; i < rows_m1; i++)
        pthread_join(threads[i], NULL);

    clock_t end = clock() - start;

    //print the output
    printf("Row By Row");
    for (int i = 0; i < rows_m1; i++) {
        printf("\n");
        for (int j = 0; j < columns_m2; j++)
            printf("%d ", result_matrix[i][j]);
    }
    printf("\nTime Taken : %lf sec", (double) ((double) end / CLOCKS_PER_SEC));

///////////////////////////////////////////////////////////////////////////////////////////////

    //clear the output matrix
    for (int i = 0; i < rows_m1; i++) {
        for (int j = 0; j < columns_m2; j++)
            result_matrix[i][j] = 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////


    printf("\n\nElement By Element");

    int output_size = rows_m1 * columns_m2;

    start = clock();
    pthread_t threads_ebe[output_size];
    for (int i = 0; i < output_size; i++)
        pthread_create(&threads_ebe[i], NULL, &mul_elementByElement, NULL);

    for (int i = 0; i < output_size; i++)
        pthread_join(threads_ebe[i], NULL);

    end = clock() - start;

    //print the output matrix
    for (int i = 0; i < rows_m1; i++) {
        printf("\n");
        for (int j = 0; j < columns_m2; j++)
            printf("%d ", result_matrix[i][j]);
    }
    printf("\nTime Taken : %lf sec", (double) ((double) end / CLOCKS_PER_SEC));

///////////////////////////////////////////////////////////////////////////////////////////////

    printf("\n\nNormal n^3 multiplication");

    start = clock();

    for (int i = 0; i < rows_m1; i++)
        for (int j = 0; j < columns_m2; j++) {
            result_matrix[i][j] = 0;
            for (int k = 0; k < rows_m2; k++)
                result_matrix[i][j] += matrix_1[i][k] * matrix_2[k][j];
        }

    end = clock() - start;

    //print the output matrix
    for (int i = 0; i < rows_m1; i++) {
        printf("\n");
        for (int j = 0; j < columns_m2; j++)
            printf("%d ", result_matrix[i][j]);
    }
    printf("\nTime Taken : %lf sec", (double) ((double) end / CLOCKS_PER_SEC));

    fclose(fp);

    return 0;
}
