echo "Parallel version with 1 process"
mpirun -np 1 ../bfs ../graphs/NotreDame_www.mtx -1 1

echo "Parallel version with 2 processes"
mpirun -np 2 ../bfs ../graphs/NotreDame_www.mtx -1 1

echo "Parallel version with 4 processes"
mpirun -np 4 ../bfs ../graphs/NotreDame_www.mtx -1 1

echo "Parallel version with 8 processes"
mpirun -np 8 ../bfs ../graphs/NotreDame_www.mtx -1 1

echo "Parallel version with 16 processes"
mpirun -np 16 ../bfs ../graphs/NotreDame_www.mtx -1 1
