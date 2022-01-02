#include <mpi.h>

#include "utils.h"

int mpi_vertex_dist(graph_t *graph, int start_vertex, int *result) {
    /*
      Each process has:
      v_length = [2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3]
      v_begin  = [0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34]
      v_adj_list = [0,2,0,3,4,3,2,1,7,6,4,1,4,9,9,10,7,11,5,6,12,11,10,8,13,8,16,15,13,17,13,17,16,17,14,8,15]
    */

    int num_vertices = graph->num_vertices;
    fill_n(result, num_vertices, MAX_DIST);

    auto start_time = Time::now();

    int depth = 0;
    result[start_vertex] = depth;

    int keep_going = true;

    // My modifications
    int my_rank, num_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    int my_start_vertex;
    int my_end_vertex;
    if (my_rank != num_proc - 1) {
        my_start_vertex = my_rank * ((num_vertices / num_proc) + 1);
        my_end_vertex = (my_rank + 1) * ((num_vertices / num_proc) + 1);
        //printf("Rank: %d, start vertex: %d, end vertex: %d\n", my_rank, my_start_vertex, my_end_vertex);
    } else {
        my_start_vertex = my_rank * ((num_vertices / num_proc) + 1);
        my_end_vertex = num_vertices;
        //printf("Rank: %d, start vertex: %d, end vertex: %d\n", my_rank, my_start_vertex, my_end_vertex);
    }

    while (keep_going) {
        keep_going = false;

        for (int vertex = 0; vertex < num_vertices; vertex++) {
            if (result[vertex] == depth && vertex >= my_start_vertex && vertex < my_end_vertex) {
                for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
                    int neighbor = graph->v_adj_list[n];

                    if (result[neighbor] > depth + 1) {
                        result[neighbor] = depth + 1;
                        keep_going = true;
                    }
                }
            }

            printf("Rank: %d, Vertex: %d\n", my_rank, vertex);

            MPI_Allreduce(MPI_IN_PLACE, result, num_vertices, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            /*if(my_rank == 0) {
              MPI_Send(result, num_vertices, MPI_INT, 1, 0, MPI_COMM_WORLD);
            }else {
              MPI_Recv(result, num_vertices, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            }*/
        }

        depth++;
    }

    // print_result(graph, result, depth);
    return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}