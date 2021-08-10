#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <fstream>
#include <bits/stdc++.h>
#include <sstream>
using namespace std;
typedef long long int ll;

double calculate(int start, int end)
{
    if(start == 0 || end == 0)
        return 0;

    double sum = 0;
    for(int i=start; i<=end; i++)
    {
        sum = sum + (1.0/(double(i)*double(i)));
    }
    // cout<<"Start "<<start<<" end "<<end<<" result "<<sum<<endl;
    return sum;
}

int main( int argc, char **argv ) {
    int rank, numprocs;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */
    if(rank == 0) {
        double result = 1.0;
        ifstream infile(argv[1]);
        FILE* outfile = fopen(argv[2],"w");
        string fline;
        getline(infile, fline);
        int N = stoll(fline,nullptr, 10);
        if(numprocs == 1)
        {
            result = calculate(1,N);
        }
        else
        {
            double a = double(N)/double(numprocs-1);
            int window = floor(a);
            int start = 2;
            int end = 0;
            if(window == 0)
            {
                for(int i=1; i<numprocs; i++)
                {
                    double temp_sum = 0;
                    if(i == 1)
                    {
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&N,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Recv(&temp_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        result = result + temp_sum;
                    }
                    else
                    {
                        int t = 0;
                        double t_temp = 0.0;
                        MPI_Send(&t,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&t,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Recv(&t_temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    }
                }
            }
            else
            {
                double temp_sum = 0;
                for(int i=1;i<numprocs;i++)
                {
                    end = start+window;
                    // cout<<"Start "<<start<<" end "<<end<<" result "<<temp_sum<<endl;
                    if(end <= N)
                    {
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&end,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Recv(&temp_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        result = result + temp_sum;
                    }
                    else
                    {
                        end = N;
                        MPI_Send(&start,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Send(&end,1,MPI_INT,i,0,MPI_COMM_WORLD);
                        MPI_Recv(&temp_sum, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        result = result + temp_sum;
                    }
                    start = end+1;
                }
            }
        }
        fprintf(outfile,"%0.6f",result);
        fclose(outfile);
    }
    else
    {
        int start = 0;
        int end = 0;
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double sum = 0.0;
        sum = calculate(start,end);
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}