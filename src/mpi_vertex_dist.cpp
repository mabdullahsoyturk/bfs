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

        int *num_sends = new int[num_ranks];
        std::vector<int> send_indices;

        for (int vertex = my_start_vertex; vertex < my_end_vertex; vertex++) {
            if (result[vertex] == depth) {
                for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
                    int neighbor = graph->v_adj_list[n];

                    if (result[neighbor] > depth + 1) {
                        result[neighbor] = depth + 1;
                        send_indices.push_back(neighbor);
                        keep_going = true;
                    }
                }
            }
        }

        int num_send = send_indices.size();
        int depths[num_send];
        for(int i = 0; i < send_indices.size(); i++){
            depths[i] = result[send_indices[i]];
        }

        MPI_Allgather(&num_send, 1, MPI_INT, num_sends, 1, MPI_INT, MPI_COMM_WORLD);
        int recv_size = 0;

        int *displs = new int[num_ranks];
        for(int i = 0; i < num_ranks; i++) {
            displs[i] = recv_size;
            recv_size += num_sends[i];
        }

        int *recv_indices = new int[recv_size];
        int *recv_depths = new int[recv_size];
        MPI_Allgatherv(send_indices.data(), num_send, MPI_INT, recv_indices, num_sends, displs, MPI_INT, MPI_COMM_WORLD);
        MPI_Allgatherv(depths, num_send, MPI_INT, recv_depths, num_sends, displs, MPI_INT, MPI_COMM_WORLD);

        for(int i = 0; i < recv_size; i++) {
            result[recv_indices[i]] = recv_depths[i];
        }
    
        MPI_Allreduce(MPI_IN_PLACE, &keep_going, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        delete[] num_sends;
        delete[] displs;
        delete[] recv_indices;
        delete[] recv_depths;

        depth++;
    }

    // print_result(graph, result, depth);
    return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}