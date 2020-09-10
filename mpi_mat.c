//mpicc mpi_mat.c -o mat
//mpiexec -n 4 mat, donde 4 es la cantidad de procesos qu eusaremos
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#define M 2048
#define N 2048
#define P 1

int main(int argc,char*argv[]){
 int rank,size;
 int order;
 //order=2048;
 MPI_Status status;
 MPI_Init(&argc,&argv);
 MPI_Comm_size(MPI_COMM_WORLD,&size); //determina el tamaño del grupo asociado con el comuicdor
 MPI_Comm_rank(MPI_COMM_WORLD,&rank);//rango del proceso que lo llama
 int A[M][N],B[N][P],C[M][P]; 
 //int A[order][order],B[order][order],C[order][order];
 int i,j,k;

 
 if(rank == 0){
 /** LEEMOS LA MATRIZ A Y B EN EL PROCESO 0*/
 int count = 0;
 for (i=0;i<M;i++){
     for (j=0;j<N;j++){
         A[i][j] = rand () % 11 + 1;
    }}
 for (i=0;i<N;i++){
    for (j=0;j<P;j++){
        B[i][j] = rand () % 11 + 1;
     }}
 }
 MPI_Barrier(MPI_COMM_WORLD); // sincronización entre procesos
 double t0 = MPI_Wtime();
 int n_Columns[size],n_Rows[size],colums,rows,indices[size];
 /** MANDAMOS LA MATRIZ B A TODOS LOS PROCESOS*/
 MPI_Bcast(B,N*P,MPI_INT,0,MPI_COMM_WORLD); //transmire un mensaje del mproceso con ango raiz a los demás pocesos
 /** DESCOMPOSICION DE LA MATRIZ  A */
 rows = ( rank < M%size )?(M/size+1)*N:(M/size)*N;
 indices[0] = 0 ;
 n_Columns[0] = ( 0<M%size )?(M/size+1)*N:(M/size)*N;
 for(i=1;i<size;i++){
  n_Columns[i] = (i < M%size ) ? (M/size+1)*N:(M/size)*N;
  indices[i] = n_Columns[i-1]+indices[i-1];
 }
 MPI_Barrier(MPI_COMM_WORLD);//sincroniza
 MPI_Scatterv(A,n_Columns,indices,MPI_INT,A,rows,MPI_INT,0,MPI_COMM_WORLD);//divide o disersa un buffer en partes para todos los procesos
 MPI_Barrier(MPI_COMM_WORLD);//sincroniza
 /** MULTIPLICACIÃ“N DE LA MATRIZ B con Bloques de A */
 for(i=0;i<rows/N;i++)
  for(j=0;j<P;j++){
   C[i][j] = 0;
   for(k=0;k<N;k++)
    C[i][j] += A[i][k]*B[k][j];
  }

 /** AGRUPAMOS LOS RESULTADOS OBTENIDOS EN C */
 colums = ( rank < M%size )?(M/size+1)*P : M/size*P;
 n_Rows[0] = ( 0 < M%size )?(M/size+1)*P : M/size*P;
 indices[0] = 0;
 for(i=1;i<size;i++){
  n_Rows[i] = (i < M%size )?(M/size+1)*P:(M/size)*P;
  indices[i] = n_Rows[i-1]+indices[i-1];
 }
 MPI_Barrier(MPI_COMM_WORLD);//sicroniza
 MPI_Gatherv(C,colums,MPI_INT,C,n_Rows,indices,MPI_INT,0,MPI_COMM_WORLD);//distribuye
 MPI_Barrier(MPI_COMM_WORLD);//sincroniza
 double t1 = MPI_Wtime();
 
 if( rank == 0 ){
  printf(" MATRIX A[%d][%d]XB[%d][%d]=C[%d][%d]\n",M,N,N,P,M,P);
  /*for(i=0;i<M;i++){
   for(j=0;j<P;j++)
    printf(" %d ",C[i][j]);
   printf("\n");
  }
  
 */
printf("time %f!\n",t1-t0);
}
 MPI_Barrier(MPI_COMM_WORLD);
 MPI_Finalize();
 return 0;
}