echo "Parallel version with 1 process"
mpirun -np 1 ../bfs ../graphs/m14b.mtx -1 10

echo "Parallel version with 2 processes"
mpirun -np 2 ../bfs ../graphs/m14b.mtx -1 10

echo "Parallel version with 4 processes"
mpirun -np 4 ../bfs ../graphs/m14b.mtx -1 10

echo "Parallel version with 8 processes"
mpirun -np 8 ../bfs ../graphs/m14b.mtx -1 10

echo "Parallel version with 16 processes"
mpirun -np 16 ../bfs ../graphs/m14b.mtx -1 10
