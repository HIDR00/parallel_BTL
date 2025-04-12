*** Biên dịch chương trình c: 
    gcc mergesort_s.c -o s  


*** Biên dịch chương trình Pthread: 
    clang -o p mergesort_p.c -pthread



*** Biên dịch và chạy chương trình OMP: 
    + clang -Xpreprocessor -fopenmp mergesort_o.c -o o $LDFLAGS -lomp $CPPFLAGS
    + ./o


*** Biên dịch và chạy chương trình MPI: 
    + Biên dịch chương trình MPI: mpicc -o m mergesort_m.c 
    + Chạy chương trình MP với 4 tiến trình: mpiexec -np 4 ./m
