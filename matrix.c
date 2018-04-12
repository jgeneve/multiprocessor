#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define M_SIZE 512
#define RD_MAX 500

typedef int matrix[M_SIZE][M_SIZE];

void fill_matrix(matrix m) {
    for (int i=0; i<M_SIZE; i++)
        for (int j=0; j<M_SIZE; j++)
            m[i][j] = rand() % RD_MAX;
}

void empty_matrix(matrix m) {
    for (int i=0; i<M_SIZE; i++)
        for (int j=0; j<M_SIZE; j++)
            m[i][j] = 0;
}

int calculate(matrix a, matrix b, matrix c, int i, int j){ // i=line; j=row
  int result;
  for (int k=0; k<M_SIZE; k++) {
      result = c[i][j] + a[i][k] * b[k][j];
    }
    return result;
}

void calculate_seq_time(matrix a, matrix b, matrix c){
  // Calculate C matrix SEQUENCALY
  empty_matrix(c); //Create the result matrix : empty
  clock_t c_begin = clock();

  for (int i=0; i<M_SIZE; i++) {
      for (int j=0; j<M_SIZE; j++) {
          c[i][j] = 0;
          c[i][j] = calculate(a, b, c, i, j);
          // for (int k=0; k<M_SIZE; k++) {
          //     c[i][j] = c[i][j] + a[i][k] * b[k][j];
          //}
          // printf("C[%i][%i]=%i\n", i, j, C[i][j]);
      }
  }
  clock_t c_end = clock();
  printf("A & B multiplication sequencialy in %f seconds\n", (double)(c_end - c_begin) / CLOCKS_PER_SEC);
}

void calculate_parallel_time(matrix a, matrix b, matrix c){

    empty_matrix(c); //Create the result matrix : empty

    clock_t c_begin = clock();

    for (int i=0; i<M_SIZE; i++) {
        for (int j=0; j<M_SIZE; j++) {
            c[i][j] = 0;
            c[i][j] = calculate(a, b, c, i, j);
        }
            // printf("C[%i][%i]=%i\n", i, j, C[i][j]);
    }

  clock_t c_end = clock();
  printf("A & B multiplication parrallel in %f seconds\n", (double)(c_end - c_begin) / CLOCKS_PER_SEC);
}


int main(int argc, char ** argv) {

    matrix a,b,c;

    int rank, size;
    int length;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int numslaves=size-1;
    
    if (rank == 0) {
        fill_matrix(a);
        fill_matrix(b);
        // Create 2 random matrix
        calculate_seq_time(a, b, c);
        calculate_parallel_time(a, b, c);

    }

    printf("Hello MPI: processor %d of %d\n", rank,size-1);

    MPI_Finalize();
}
