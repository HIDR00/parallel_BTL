#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 1000000

void merge(int *First, int Fsize, int *Second, int Ssize, int ascending) {
    int fi = 0, si = 0, mi = 0, i;
    int *merged;
    int Msize = Fsize + Ssize;

    merged = (int *)malloc(Msize * sizeof(int));

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
        Second[i] = merged[Fsize + i];

    free(merged);
}

void sort(int *Arr, int start, int end, int ascending) {
    if (start >= end) return;
    
    int mid = (start + end) / 2;
    int leftCount = mid - start + 1;
    int rightCount = end - mid;

    if (end - start > 5000) {  
        #pragma omp task shared(Arr)
        sort(Arr, start, mid, ascending);
        #pragma omp task shared(Arr)
        sort(Arr, mid + 1, end, ascending);
        #pragma omp taskwait
    } else {
        sort(Arr, start, mid, ascending);
        sort(Arr, mid + 1, end, ascending);
    }

    merge(Arr + start, leftCount, Arr + mid + 1, rightCount, ascending);

}

void print_array(int *Arr, int size)
{
    for (int i = 0; i < size; i++) {
        printf("%d ", Arr[i]);
    }
    printf("\n");
}


int main() {
    int* data= (int *)malloc(N*sizeof(int));
    int i;
    double seconds;
	double start,stop;

     
    for(i = 0; i < N; i++)
        data[i] = rand() % 100000;

    int thread_counts[] = {2, 4, 6, 8, 12};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    for (int i = 0; i < num_tests; i++) {
        int p = thread_counts[i];
        int* ascendingArr = (int*)malloc(N * sizeof(int));
        int* descendingArr = (int*)malloc(N * sizeof(int));
        
        for (int j = 0; j < N; j++) {
            ascendingArr[j] = data[j];
            descendingArr[j] = data[j];
        }
        
        omp_set_num_threads(p);
        start = omp_get_wtime();
        
        #pragma omp parallel
        {            
            #pragma omp single
            {
                sort(ascendingArr, 0, N - 1, 1); 
                sort(descendingArr, 0, N - 1, 0); 
            }            
        }
        
        stop = omp_get_wtime();
        seconds = stop - start;
        printf("\nArray with %d; %d processors, took %f seconds\n",N,p,seconds);
        
        // printf("\n******* Sorted in Ascending Order *******\n");
        // print_array(ascendingArr, N);  

        // printf("\n******* Sorted in Descending Order *******\n");
        // print_array(descendingArr, N);
        
        free(ascendingArr);
        free(descendingArr);
    }
    
    free(data);
    
    return 0;
}
