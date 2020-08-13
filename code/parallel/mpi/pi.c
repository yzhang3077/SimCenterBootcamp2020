#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int long numSteps = 1000000000;

int main(int argc, char **argv) {

  int numP, pid;

  //
  // the usual mpi initialization
  //

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numP);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);

  //
  // start timer
  //

  clock_t start_t = clock();

  //
  // init some variable
  //

  double pi = 0;
  double dx = 1./numSteps;
  int start = pid * numSteps / numP;
  int end = (pid+1) * numSteps / numP;
  double x  = (start + 0.5)*dx;

  if (pid == numP) 
    end = numSteps;

  //
  // compute processors contribution to pi
  //

  for (int i=start; i<end; i++) {
    pi += 4./(1.+x*x);
    x += dx;
  }
  pi *= dx;

  //
  // gather contributions on P0 & sum
  //

  double *globalSum = 0;
  if (pid == 0) {
    globalSum = (double *)malloc(numP * sizeof(double) );
  }
  MPI_Gather(&pi, 1, MPI_DOUBLE, globalSum, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (pid == 0) {
    for (int i=1; i<numP; i++) // 1 as pi already has p0 contribution
      pi += globalSum[i];
  }

  // 
  // end timer
  //

  clock_t end_t = clock();
  double time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

  if (pid == 0)
    printf("PI = %16.8f, duration: %f s\n",pi, time);

  // 
  // usual termination for MPI
  //

  MPI_Finalize();
  return 0;
}

