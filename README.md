*** Biên dịch chương trình c: 
    gcc merge_sort.c -o s 



*** Biên dịch chương trình Pthread: 
    clang -o p merge_sort_p.c -pthread



*** Biên dịch và chạy chương trình OMP: 
    + clang -Xpreprocessor -fopenmp mergesort_o.c -o o $LDFLAGS -lomp $CPPFLAGS
    + ./o



*** Biên dịch và chạy chương trình MPI: 
    + Biên dịch chương trình MPI: mpicc -o hello_world_mpi hello_world_mpi.c
    + Chạy chương trình MP với 4 tiến trình: mpiexec -np 4 ./hello_world_mpi# parallel_BTL
