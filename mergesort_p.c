#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000  
int *arr;
int thread_count;

typedef struct {
    int left;
    int right;
} ThreadArgs;

void merge(int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L); free(R);
}

void merge_sort(int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(left, mid);
        merge_sort(mid + 1, right);
        merge(left, mid, right);
    }
}

void* parallel_merge_sort(void* arg) {
    ThreadArgs *args = (ThreadArgs*) arg;
    merge_sort(args->left, args->right);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    arr = (int*)malloc(N * sizeof(int));
    
    for (int i = 0; i < N; i++) arr[i] = rand();
    
    int thread_counts[] = {2, 4, 6, 8, 12};
    for (int t = 0; t < 5; t++) {
        thread_count = thread_counts[t];
        pthread_t threads[thread_count];
        ThreadArgs args[thread_count];
        clock_t start = clock();
        
        int segment = N / thread_count;
        for (int i = 0; i < thread_count; i++) {
            args[i].left = i * segment;
            args[i].right = (i == thread_count - 1) ? (N - 1) : ((i + 1) * segment - 1);
            pthread_create(&threads[i], NULL, parallel_merge_sort, &args[i]);
        }
        
        for (int i = 0; i < thread_count; i++) pthread_join(threads[i], NULL);
        
        for (int size = segment; size < N; size *= 2) {
            for (int left = 0; left < N - size; left += 2 * size) {
                int mid = left + size - 1;
                int right = (left + 2 * size - 1 < N) ? (left + 2 * size - 1) : (N - 1);
                merge(left, mid, right);
            }
        }
        
        clock_t end = clock();
        printf("Threads: %d, Total sorting time: %f seconds\n", thread_count, ((double)(end - start)) / CLOCKS_PER_SEC);
    }
    
    free(arr);
    return 0;
}
