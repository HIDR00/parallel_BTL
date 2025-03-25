#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 100


void merge(int *First, int Fsize, int *Second, int Ssize,int ascending);
void sort(int *Arr, int start, int end,int ascending);
void print_array(int *Arr, int size);

void merge(int *First, int Fsize, int *Second, int Ssize, int ascending) {
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

	free(merged);
}


void sort(int *Arr, int start, int end, int ascending)
{
	int mid = (start+end)/2;
	int leftCount = mid - start + 1;
	int rightCount = end - mid;

	if (end == start) {
		return;
	} else {
		sort(Arr, start, mid,ascending);
		sort(Arr, mid+1, end,ascending);
        merge(Arr + start, leftCount, Arr + mid + 1, rightCount, ascending);
	}
}

void print_array(int *Arr, int size)
{
    for (int i = 0; i < size; i++) {
        printf("%d ", Arr[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
	int * data;
	int * reverse_data;
	int i;
	double seconds;
	double start,stop;

	data = (int *)malloc(N*sizeof(int));
	for(i=0;i<N;i++)
		data[i] = random() % 100;
	
	reverse_data = (int *)malloc(N * sizeof(int));
    for (i = 0; i < N; i++) 
        reverse_data[i] = data[i]; 

	start = clock();
	sort(data, 0, N - 1, 1);
	sort(reverse_data, 0, N - 1, 0);
	stop = clock();

    seconds = (stop-start)/CLOCKS_PER_SEC;
    printf("\nArray with %d; %d processors, took %f seconds\n",N,1,seconds);

    printf("\n******* Sorted in Ascending Order *******\n");
    print_array(data, N);  

	printf("\n******* Sorted in Descending Order *******\n");
    print_array(reverse_data, N); 

	free(data);
    free(reverse_data);
}
