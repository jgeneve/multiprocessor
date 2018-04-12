#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define M_SIZE 512
#define RD_MAX 500

typedef double matrix[M_SIZE][M_SIZE];

void fill_matrix(matrix m) {
  int i,j;
    for (i=0; i<M_SIZE; i++)
        for (j=0; j<M_SIZE; j++)
            m[i][j] = rand() % RD_MAX;
}

void empty_matrix(matrix m) {
    int i,j;
    for (i=0; i<M_SIZE; i++)
        for (j=0; j<M_SIZE; j++)
            m[i][j] = 0.00;
}

int main(int argc, char ** argv) {

    matrix a,b,c;
    int rank, size, offset, rows, destination, source, numworkers, i, j, k;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    numworkers=size-1;

    // -------------------------- Master ----------------------------
    if (rank == 0) {
      // Create 2 random matrix
      fill_matrix(a);
      fill_matrix(b);

      clock_t c_begin = clock(); //Start timer
      if(numworkers == 0) { //Only 1 processor
        //Matrix multiplication
        for (i=0; i<M_SIZE; i++) {
            for (j=0; j<M_SIZE; j++) {
                c[i][j] = 0;
                for (k=0; k<M_SIZE; k++) {
                    c[i][j] = c[i][j] + a[i][k] * b[k][j];
                }
            }
        }
      }else{
        rows = M_SIZE/numworkers;
        offset = 0;

        //Send all data to the processors
        for(destination=1; destination<=numworkers; destination++){
          MPI_Send(&offset, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
          MPI_Send(&rows, 1, MPI_INT, destination, 1, MPI_COMM_WORLD);
          MPI_Send(&a[offset][0], rows*M_SIZE, MPI_DOUBLE,destination,1, MPI_COMM_WORLD);
          MPI_Send(&b, M_SIZE*M_SIZE, MPI_DOUBLE, destination, 1, MPI_COMM_WORLD);
          offset = offset + rows;
        }

        //Wait the answers of the processors
        for (i=1; i<=numworkers; i++)
        {
          source=i;
          MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          MPI_Recv(&c[offset][0], rows*M_SIZE, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

      }

      clock_t c_end = clock();

      // printf("Here is the a matrix:\n");
      // for (i=0; i<M_SIZE; i++) {
      //   for (j=0; j<M_SIZE; j++)
      //     printf("%6.2f   ", a[i][j]);
      //   printf ("\n");
      // }
      //
      // printf("Here is the b matrix:\n");
      // for (i=0; i<M_SIZE; i++) {
      //   for (j=0; j<M_SIZE; j++)
      //     printf("%6.2f   ", b[i][j]);
      //   printf ("\n");
      // }
      //
      // printf("Here is the result matrix:\n");
      // for (i=0; i<M_SIZE; i++) {
      //   for (j=0; j<M_SIZE; j++)
      //     printf("%6.2f   ", c[i][j]);
      //   printf ("\n");
      // }
      printf("A & B multiplication in parrallel took %f seconds\n", (double)(c_end - c_begin) / CLOCKS_PER_SEC);

    } else if(rank > 0) { // -------------------------- Workers ----------------------------
      source = 0;
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&a, rows*M_SIZE, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&b, M_SIZE*M_SIZE, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      //Matrix multiplication
      for (i=0; i<rows; i++) {
          for (j=0; j<M_SIZE; j++) {
              c[i][j] = 0;
              for (k=0; k<M_SIZE; k++) {
                  c[i][j] = c[i][j] + a[i][k] * b[k][j];
              }
            }
          }

      MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
      MPI_Send(&c, rows*M_SIZE, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }

    //printf("Hello MPI: processor %d of %d\n", rank,size-1);
    MPI_Finalize();
}
