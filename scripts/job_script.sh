#!/bin/bash
#
# You should only work under the /scratch/users/<username> directory.
#
# Example job submission script
#

#SBATCH --job-name=bfs-jobs
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --partition=mid
#SBATCH --time=16:00:00
#SBATCH --output=bfs-job.out

################################################################################
##################### !!! DO NOT EDIT ABOVE THIS LINE !!! ######################
################################################################################
# Set stack size to unlimited
echo "Setting stack size to unlimited..."
ulimit -s unlimited
ulimit -l unlimited
ulimit -a

echo "Running Job...!"
echo "==============================================================================="
echo "Running compiled binary..."

lscpu
module load mpich/3.2.1

echo "VSP STARTS!!!"
echo "Parallel version with 1 process"
mpirun -np 1 ../bfs ../graphs/vsp_msc10848_300sep_100in_1Kout.mtx -1 10

echo "Parallel version with 2 processes"
mpirun -np 2 ../bfs ../graphs/vsp_msc10848_300sep_100in_1Kout.mtx -1 10

echo "Parallel version with 4 processes"
mpirun -np 4 ../bfs ../graphs/vsp_msc10848_300sep_100in_1Kout.mtx -1 10

echo "Parallel version with 8 processes"
mpirun -np 8 ../bfs ../graphs/vsp_msc10848_300sep_100in_1Kout.mtx -1 10

echo "Parallel version with 16 processes"
mpirun -np 16 ../bfs ../graphs/vsp_msc10848_300sep_100in_1Kout.mtx -1 10

echo "VSP ENDS!!!"

echo "MYCIELSKIAN STARTS!!!"
echo "Parallel version with 1 process"
mpirun -np 1 ../bfs ../graphs/mycielskian14.mtx -1 10

echo "Parallel version with 2 processes"
mpirun -np 2 ../bfs ../graphs/mycielskian14.mtx -1 10

echo "Parallel version with 4 processes"
mpirun -np 4 ../bfs ../graphs/mycielskian14.mtx -1 10

echo "Parallel version with 8 processes"
mpirun -np 8 ../bfs ../graphs/mycielskian14.mtx -1 10

echo "Parallel version with 16 processes"
mpirun -np 16 ../bfs ../graphs/mycielskian14.mtx -1 10

echo "MYCIELSKIAN ENDS!!!"

echo "M14B STARTS!!!"

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

echo "M14B ENDS!!!"

echo "DELAUNAY STARTS!!!"

echo "Parallel version with 1 process"
mpirun -np 1 ../bfs ../graphs/delaunay_n17.mtx -1 10

echo "Parallel version with 2 processes"
mpirun -np 2 ../bfs ../graphs/delaunay_n17.mtx -1 10

echo "Parallel version with 4 processes"
mpirun -np 4 ../bfs ../graphs/delaunay_n17.mtx -1 10

echo "Parallel version with 8 processes"
mpirun -np 8 ../bfs ../graphs/delaunay_n17.mtx -1 10

echo "Parallel version with 16 processes"
mpirun -np 16 ../bfs ../graphs/delaunay_n17.mtx -1 10

echo "DELAUNAY ENDS!!!"
