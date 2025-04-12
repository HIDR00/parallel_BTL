#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define N 1000000

int * merge(int *First, int Fsize, int *Second, int Ssize,int ascending) {
	int fi = 0, si = 0, mi = 0, i;
	int *merged;
	int Msize = Fsize+Ssize;

	merged = (int *)malloc(Msize*sizeof(int));

	while ((fi < Fsize) && (si < Ssize)) {
		if (ascending) {
			if (First[fi] <= Second[si]) {
				merged[mi] = First[fi];
				mi++; fi++;
			} else {
				merged[mi] = Second[si];
				mi++; si++;
			}
		} else {
			if (First[fi] >= Second[si]) {
				merged[mi] = First[fi];
				mi++; fi++;
			} else {
				merged[mi] = Second[si];
				mi++; si++;
			}
		}
	}
	if (fi >= Fsize)
		for (i = mi; i < Msize; i++, si++)
			merged[i] = Second[si];
	else if (si >= Ssize)
		for (i = mi; i < Msize; i++, fi++)
			merged[i] = First[fi];

	for (i = 0; i < Fsize; i++)
		First[i] = merged[i];
	for (i = 0; i < Ssize; i++)
		Second[i] = merged[Fsize+i];

	return merged;
}

void sort(int *Arr, int start, int end, int ascending)
{
	int *sortedArr;
	int mid = (start+end)/2;
	int leftCount = mid - start + 1;
	int rightCount = end - mid;

	if (end == start) {
		return;
	} else {
		sort(Arr, start, mid,ascending);
		sort(Arr, mid+1, end,ascending);
        sortedArr = merge(Arr + start, leftCount, Arr + mid + 1, rightCount, ascending);
	}
}

void printElements(int rank, int *t, int n)
{
	int i;
	printf("rank %d: ",rank);
	for(i=0;i<n;i++)
        printf("%d ",t[i]);
    printf("\n\n");
}

int main(int argc, char **argv)
{
	int * data;
	int *local_data_asc;
    int *local_data_desc;
	int * otherArr;
	int q = N;
	int my_rank,comm_sz;
	int local_n = 0;
	int i;
	int step;
	double start,stop;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);

	start = clock();
	if(my_rank == 0)
	{
		int r;
		local_n = N/comm_sz;
		r = N%comm_sz;
		int size = N+local_n-r;
		
		data = (int *)malloc((size)*sizeof(int));
		for(i=0;i<N;i++)
			data[i] = random() % 1000;

		if(r!=0)
		{
			for(i=N;i<size;i++)
				data[i]=0;
			local_n=local_n+1;
		}

		MPI_Bcast(&local_n,1,MPI_INT,0,MPI_COMM_WORLD);

		local_data_asc = (int *)malloc(local_n * sizeof(int));
		local_data_desc = (int *)malloc(local_n * sizeof(int));

		MPI_Scatter(data, local_n, MPI_INT, local_data_asc, local_n, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Scatter(data, local_n, MPI_INT, local_data_desc, local_n, MPI_INT, 0, MPI_COMM_WORLD);


		sort(local_data_asc, 0, local_n - 1, 1);
		sort(local_data_desc, 0, local_n - 1, 0);
		free(data);
	}
	else
	{
		MPI_Bcast(&local_n,1,MPI_INT,0,MPI_COMM_WORLD);
		local_data_asc = (int *)malloc(local_n * sizeof(int));
        local_data_desc = (int *)malloc(local_n * sizeof(int));

		MPI_Scatter(data, local_n, MPI_INT, local_data_asc, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(data, local_n, MPI_INT, local_data_desc, local_n, MPI_INT, 0, MPI_COMM_WORLD);

		sort(local_data_asc, 0, local_n - 1, 1);
        sort(local_data_desc, 0, local_n - 1, 0);
	}

	step = 1;
	while (step < comm_sz) {
        if (my_rank % (2 * step) == 0 && my_rank + step < comm_sz) {
            MPI_Recv(&q, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, &status);
            otherArr = (int *)malloc(q * sizeof(int));

            MPI_Recv(otherArr, q, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, &status);
            local_data_asc = merge(local_data_asc, local_n, otherArr, q, 1);

            MPI_Recv(otherArr, q, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, &status);
            local_data_desc = merge(local_data_desc, local_n, otherArr, q, 0);

            local_n = local_n + q;
            free(otherArr);
        } else if (my_rank % (2 * step) == step) {

            int dest = my_rank - step;
            MPI_Send(&local_n, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(local_data_asc, local_n, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(local_data_desc, local_n, MPI_INT, dest, 0, MPI_COMM_WORLD);
			
        }
        step = step * 2;
        MPI_Barrier(MPI_COMM_WORLD);
    }
	stop = clock();

	if(my_rank == 0)
	{
        printf("\nArray with %d; %d processors, took %f second\n\n",local_n, comm_sz,(stop-start)/CLOCKS_PER_SEC);
		// printf("Ascending order:\n");
        // printElements(my_rank, local_data_asc, local_n);
        // printf("Descending order:\n");
        // printElements(my_rank, local_data_desc, local_n);
	}

	free(local_data_asc);
    free(local_data_desc);

    MPI_Finalize();
}
