#include <stdio.h>
    #include <stdlib.h>
    #include <mpi.h>
    #include <time.h>

    #define COLSa 10
    #define ROWSa 10

    #define COLSb 10
    #define ROWSb 10
    #define s 2

   int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char i;
    char j;

    char a[ROWSa*COLSa];
    char b[ROWSb*COLSb];
    char c[ROWSa*COLSb];  // c=a*b

    const int NPROWS=s;  /* number of rows in _decomposition_ */
    const int NPCOLS=s;  /* number of cols in _decomposition_ */

    const int BLOCKROWSa = ROWSa/NPROWS;  /* number of rows in _block_ */
    const int BLOCKCOLSa = COLSa/NPCOLS; /* number of cols in _block_ */

    const int BLOCKROWSb = ROWSb/NPROWS;  /* number of rows in _block_ */
    const int BLOCKCOLSb= COLSb/NPCOLS; /* number of cols in _block_ */

    if (rank == 0) {

    for (int ii=0; ii<ROWSa*COLSa; ii++) {
         a[ii]=rand() %10 ;
    }

    for (int ii=0; ii<ROWSb*COLSb; ii++) {
         b[ii]=rand() %10 ;

       }
    }

    char BLa[BLOCKROWSa*BLOCKCOLSa];
    for (int ii=0; ii<BLOCKROWSa*BLOCKCOLSa; ii++)
    BLa[ii] = 0;

    char BLb[BLOCKROWSb*BLOCKCOLSb];
    for (int ii=0; ii<BLOCKROWSb*BLOCKCOLSb; ii++)
    BLb[ii] = 0;

    char BLc[BLOCKROWSa*BLOCKCOLSb];
    for (int ii=0; ii<BLOCKROWSa*BLOCKCOLSb; ii++)
    BLc[ii] = 0;

    MPI_Datatype blocktype;
    MPI_Datatype blocktype2;

    MPI_Type_vector(BLOCKROWSa, BLOCKCOLSa, COLSa, MPI_CHAR, &blocktype2);
    MPI_Type_vector(BLOCKROWSb, BLOCKCOLSb, COLSb, MPI_CHAR, &blocktype2);

    MPI_Type_create_resized( blocktype2, 0, sizeof(char), &blocktype);
    MPI_Type_commit(&blocktype);

    int dispsa[NPROWS*NPCOLS];
    int countsa[NPROWS*NPCOLS];
    int dispsb[NPROWS*NPCOLS];
    int countsb[NPROWS*NPCOLS];

    //*******************************Start Time Record****************//

    clock_t t;
    t=clock();

    for (int ii=0; ii<NPROWS; ii++) {
    for (int jj=0; jj<NPCOLS; jj++) {
    dispsa[ii*NPCOLS+jj] = ii*COLSa*BLOCKROWSa+jj*BLOCKCOLSa;
    countsa [ii*NPCOLS+jj] = 1;
        }
    }

    MPI_Scatterv(a, countsa, dispsa, blocktype, BLa, BLOCKROWSa*BLOCKCOLSa, MPI_CHAR, 0,   MPI_COMM_WORLD);


    for (int ii=0; ii<NPROWS; ii++) {
    for (int jj=0; jj<NPCOLS; jj++) {
    dispsb[ii*NPCOLS+jj] = ii*COLSb*BLOCKROWSb+jj*BLOCKCOLSb;
    countsb [ii*NPCOLS+jj] = 1;
         }
    }

    MPI_Scatterv(b, countsb, dispsb, blocktype, BLb, BLOCKROWSb*BLOCKCOLSb, MPI_CHAR, 0, MPI_COMM_WORLD);

     for (int proc=0; proc<p; proc++) {
        if (proc == rank) {

          printf("Rank = %d\n", rank);

                if (rank == 0) {
                  printf("Global matrix A : \n");

                   for (int ii=0; ii<ROWSa; ii++) {
                     for (int jj=0; jj<COLSa; jj++) {
                       printf("%3d ",(int)a[ii*COLSa+jj]);
                }
                        printf("\n");
            }
                 printf("\n");
            printf("Global matrix B : \n");

           for (int ii=0; ii<ROWSb; ii++) {
             for (int jj=0; jj<COLSb; jj++) {
              printf("%3d ",(int)b[ii*COLSb+jj]);
                }
         printf("\n");
            }
        printf("\n");
                  printf("Local Matrix A:\n");
              for (int ii=0; ii<BLOCKROWSa; ii++) {
                for (int jj=0; jj<BLOCKCOLSa; jj++) {
            printf("%3d ",(int)BLa[ii*BLOCKCOLSa+jj]);

                }

             printf("\n");
            }

           printf("\n");
              printf("Local Matrix B:\n");
                for (int ii=0; ii<BLOCKROWSb; ii++) {
                   for (int jj=0; jj<BLOCKCOLSb; jj++) {
                       printf("%3d ",(int)BLb[ii*BLOCKCOLSb+jj]);

                }

          printf("\n");
            }
                }


            printf("Local Matrix A:\n");
                    for (int ii=0; ii<BLOCKROWSa; ii++) {
                   for (int jj=0; jj<BLOCKCOLSa; jj++) {
                       printf("%3d ",(int)BLa[ii*BLOCKCOLSa+jj]);
                  }

             printf("\n");
            }

          printf("Local Matrix B:\n");
            for (int ii=0; ii<BLOCKROWSb; ii++) {
                for (int jj=0; jj<BLOCKCOLSb; jj++) {
                   printf("%3d ",(int)BLb[ii*BLOCKCOLSb+jj]);
                }

        printf("\n");
            }
            printf("\n");

  //**********************Multiplication***********************//
       for (int i = 0; i < BLOCKROWSa; i++) {
          for (j = 0; j < BLOCKCOLSb; j++) {
            for (int k = 0; k < BLOCKCOLSb; k++) {  //I am considering square matrices with the same sizes
               BLc[i + j*BLOCKROWSa] += BLa[i + k*BLOCKROWSa]*BLb[k + BLOCKCOLSb*j];
               printf("%3d ",(int)BLc[i+j*BLOCKROWSa]);
             }
             printf("\n");
           }
           printf("\n");
         }
       }

     MPI_Barrier(MPI_COMM_WORLD);

    }
    MPI_Gatherv(BLc, BLOCKROWSb*BLOCKCOLSb,MPI_CHAR, c, countsb, dispsb,blocktype, 0, MPI_COMM_WORLD);

if (rank == 0) {
   printf("Global matrix C : \n");

   for (int ii=0; ii<ROWSa; ii++) {
       for (int jj=0; jj<COLSa; jj++) {
           printf("%3d ",(int)c[ii*COLSa+jj]);
       }
       printf("\n");
   }
}
   MPI_Finalize();

   //**********************End Time Record************************//

    t=clock()-t;
     //printf("It took %f seconds (%d clicks).\n",t,((float)t)/CLOCKS_PER_SEC);


       return 0;
     }
