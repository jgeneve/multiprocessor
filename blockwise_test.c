#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define M_SIZE 6
#define RD_MAX 30

typedef int matrix[M_SIZE][M_SIZE];

void fill_matrix(matrix m) {
  int i,j;
  int index=0;
  for (i=0; i<M_SIZE; i++)
  for (j=0; j<M_SIZE; j++)
  //m[i][j] = rand() % RD_MAX;
  m[i][j] = index++;

}

void empty_matrix(matrix m) {
  int i,j;
  for (i=0; i<M_SIZE; i++)
  for (j=0; j<M_SIZE; j++)
  m[i][j] = 0;
}

void split_matrix_h(matrix m, int nbrows, int nbcols, int size){
  int offset_rows=0;
  int offset_cols=0;
  int i, j, p;
  for (p=0; p<size;p++){
    int sub_m[nbrows*nbcols];
    int index=0;
    for(i=0;i<M_SIZE;i++){
      for(j=0;j<M_SIZE;j++){
        if(i>=offset_rows && i<(offset_rows+nbrows) && j>=offset_cols && j<(offset_cols+nbcols)){
          sub_m[index]=m[i][j];
          index++;
        }
      }
    }
    printf("SUB MATRIX A IS\n");
    for (i=0; i<sizeof(sub_m)/sizeof(int); i++) {
      printf("%d   ", sub_m[i]);
    }
    printf("\n------------\n");

    if(offset_cols<M_SIZE){
      offset_cols=offset_cols+nbcols;
    }

    if(offset_cols==M_SIZE){
      offset_cols=0;
      offset_rows=offset_rows+nbrows;
    }
  }
}

int main(int argc, char ** argv) {

  matrix a,b,c;
  int rank, size, nbrows, nbcols, i, j, k;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  // ------------ Calcul numbers of rows and numbers of columns in blocks ---------- //
  if(size <= 2){
    nbrows=M_SIZE;
    nbcols=M_SIZE/size;
  }else {
    nbrows=M_SIZE/2;
    nbcols=M_SIZE/(size/2);
  }

  if (rank == 0) {
    // Fill our matrix with randoms numbers
    fill_matrix(a);
    fill_matrix(b);
  }

  // -------- Broadcast the matrix --------- //
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&(a[0][0]), M_SIZE*M_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  printf("-------------------------------\nI'm in processor %d\n", rank);
  printf("Here is the A matrix:\n");
  for (i=0; i<M_SIZE; i++) {
    for (j=0; j<M_SIZE; j++)
    printf("%d   ", a[i][j]);
    printf ("\n");
  }

  // Split the matrix in blocks
  split_matrix_h(a, nbrows, nbcols, size);

  // ---------------------- Calculation -----------------

  MPI_Finalize();
}
