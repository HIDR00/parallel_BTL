#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000  
int *arr_asc; 
int *arr_desc; 
int thread_count;

typedef struct {
    int left;
    int right;
    int ascending;
    int *arr; 
} ThreadArgs;

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
    int mid = (start + end) / 2;
    int leftCount = mid - start + 1;
    int rightCount = end - mid;

    if (end == start) {
        return;
    } else {
        sort(Arr, start, mid, ascending);
        sort(Arr, mid + 1, end, ascending);
        merge(Arr + start, leftCount, Arr + mid + 1, rightCount, ascending);
    }
}

void* parallel_sort(void* arg) {
    ThreadArgs *args = (ThreadArgs*) arg;
    sort(args->arr, args->left, args->right, args->ascending);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    int *arr_orig = (int*)malloc(N * sizeof(int)); 
    arr_asc = (int*)malloc(N * sizeof(int));    
    arr_desc = (int*)malloc(N * sizeof(int));     
    
    // Tạo dữ liệu ban đầu
    for (int i = 0; i < N; i++) arr_orig[i] = rand();
    
    int thread_counts[] = {2, 4, 6, 8, 12};
    for (int t = 0; t < 5; t++) {
        thread_count = thread_counts[t];
        pthread_t threads_asc[thread_count];  // Threads cho tăng dần
        pthread_t threads_desc[thread_count]; // Threads cho giảm dần
        ThreadArgs args_asc[thread_count];    // Đối số cho tăng dần
        ThreadArgs args_desc[thread_count];   // Đối số cho giảm dần
        
        // Sao chép dữ liệu gốc vào cả hai mảng
        for (int i = 0; i < N; i++) {
            arr_asc[i] = arr_orig[i];
            arr_desc[i] = arr_orig[i];
        }
        
        clock_t start = clock();
        
        int segment = N / thread_count;
        // Tạo threads cho sắp xếp tăng dần
        for (int i = 0; i < thread_count; i++) {
            args_asc[i].left = i * segment;
            args_asc[i].right = (i == thread_count - 1) ? (N - 1) : ((i + 1) * segment - 1);
            args_asc[i].ascending = 1; // Tăng dần
            args_asc[i].arr = arr_asc;
            pthread_create(&threads_asc[i], NULL, parallel_sort, &args_asc[i]);
        }
        
        // Tạo threads cho sắp xếp giảm dần
        for (int i = 0; i < thread_count; i++) {
            args_desc[i].left = i * segment;
            args_desc[i].right = (i == thread_count - 1) ? (N - 1) : ((i + 1) * segment - 1);
            args_desc[i].ascending = 0; // Giảm dần
            args_desc[i].arr = arr_desc;
            pthread_create(&threads_desc[i], NULL, parallel_sort, &args_desc[i]);
        }
        
        // Đợi tất cả threads hoàn thành
        for (int i = 0; i < thread_count; i++) {
            pthread_join(threads_asc[i], NULL);
            pthread_join(threads_desc[i], NULL);
        }
        
        // Hợp nhất các đoạn đã sắp xếp cho tăng dần
        for (int size = segment; size < N; size *= 2) {
            for (int left = 0; left < N - size; left += 2 * size) {
                int mid = left + size;
                int right = (left + 2 * size - 1 < N) ? (left + 2 * size - 1) : (N - 1);
                int leftCount = mid - left;
                int rightCount = right - mid + 1;
                merge(arr_asc + left, leftCount, arr_asc + mid, rightCount, 1);
            }
        }
        
        // Hợp nhất các đoạn đã sắp xếp cho giảm dần
        for (int size = segment; size < N; size *= 2) {
            for (int left = 0; left < N - size; left += 2 * size) {
                int mid = left + size;
                int right = (left + 2 * size - 1 < N) ? (left + 2 * size - 1) : (N - 1);
                int leftCount = mid - left;
                int rightCount = right - mid + 1;
                merge(arr_desc + left, leftCount, arr_desc + mid, rightCount, 0);
            }
        }
        
        clock_t end = clock();
        printf("Threads: %d, Total sorting time (both ascending and descending): %f seconds\n", 
               thread_count, ((double)(end - start)) / CLOCKS_PER_SEC);
    
    }
    
    free(arr_orig);
    free(arr_asc);
    free(arr_desc);
    return 0;
}
