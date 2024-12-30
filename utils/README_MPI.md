MPI version:
    make deti_coins_intel
    rm deti_coins_vault.txt
    mpirun -np 4 ./deti_coins_intel -s8 2m

    sort deti_coins_vault.txt | wc
    sort deti_coins_vault.txt | uniq -c

To run the previous ones (OpenMPI for instance):
    make deti_coins_intel
    ./deti_coins_intel -s7 8m