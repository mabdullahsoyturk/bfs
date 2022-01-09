#include <mpi.h>

#include "utils.h"

int mpi_vertex_dist(graph_t *graph, int start_vertex, int *result) {
    /*
      For test.mtx example, each process has:
      v_length = [2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3]
      v_begin  = [0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34]
      v_adj_list = [0,2,0,3,4,3,2,1,7,6,4,1,4,9,9,10,7,11,5,6,12,11,10,8,13,8,16,15,13,17,13,17,16,17,14,8,15]
    */
    int num_vertices = graph->num_vertices;
    fill_n(result, num_vertices, MAX_DIST);

    int updated_neighbor_index = 0;
    int *updated_neighbors_indices = new int[num_vertices];
    int *updated_values = new int[num_vertices];

    auto start_time = Time::now();

    int depth = 0;
    result[start_vertex] = depth;

    int keep_going = true;

    int my_rank, num_ranks;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

    int my_start_vertex, my_end_vertex;
    if (my_rank != num_ranks - 1) {
        my_start_vertex = my_rank * ((num_vertices / num_ranks) + 1);
        my_end_vertex = (my_rank + 1) * ((num_vertices / num_ranks) + 1);
    } else {
        my_start_vertex = my_rank * ((num_vertices / num_ranks) + 1);
        my_end_vertex = num_vertices;
    }

    while (keep_going) {
        keep_going = false;

        for (int vertex = my_start_vertex; vertex < my_end_vertex; vertex++) {
            if (result[vertex] == depth) {
                for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
                    int neighbor = graph->v_adj_list[n];

                    if (result[neighbor] > depth + 1) {
                        result[neighbor] = depth + 1;
                        updated_neighbors_indices[updated_neighbor_index] = neighbor;
                        updated_values[updated_neighbor_index] = depth + 1;
                        updated_neighbor_index++;
                        keep_going = true;
                    }
                }
            }
        }

        MPI_Allreduce(MPI_IN_PLACE, result, num_vertices, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(MPI_IN_PLACE, &keep_going, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        depth++;
    }

    delete[] updated_neighbors_indices;
    delete[] updated_values;

    // print_result(graph, result, depth);
    return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}