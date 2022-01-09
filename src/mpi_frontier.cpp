#include "utils.h"

int mpi_frontier(graph_t* graph, int start_vertex, int* result) {
  int num_vertices = graph->num_vertices;
  fill_n(result, num_vertices, MAX_DIST);

  auto start_time = Time::now();

  int depth = 0;
  result[start_vertex] = depth;

  int* frontier_in = new int[num_vertices];
  int* frontier_out = new int[num_vertices];
  frontier_in[0] = start_vertex;
  int front_in_size = 1;
  int front_out_size = 0;

  int my_rank, num_ranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  while (front_in_size != 0) {
    front_out_size = 0;

    int my_start_vertex, my_end_vertex;
    if(front_in_size < num_ranks) {
        if(my_rank < front_in_size) {
            my_start_vertex = my_rank;
            my_end_vertex = my_rank + 1;
        }else {
            my_start_vertex = 0;
            my_end_vertex = 0;
        }
    }else {
        if (my_rank != num_ranks - 1) {
          int work = (front_in_size / num_ranks) + 1;
          my_start_vertex = my_rank * work;
          my_end_vertex = my_start_vertex + work;
      } else {
          int remaining_work = front_in_size % num_ranks;
          my_start_vertex = front_in_size - remaining_work;
          my_end_vertex = front_in_size;
      }
    }
    //printf("Rank: %d, front_in_size: %d, my_start_vertex: %d, my_end_vertex: %d\n", my_rank, front_in_size, my_start_vertex, my_end_vertex);
    //MPI_Barrier(MPI_COMM_WORLD);

    std::vector<int> send_indices;
    for (int v = my_start_vertex; v < my_end_vertex; v++) {
      int vertex = frontier_in[v];

      for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
        int neighbor = graph->v_adj_list[n];

        if (result[neighbor] > depth + 1) {
          result[neighbor] = depth + 1;
          frontier_out[front_out_size] = neighbor;
          send_indices.push_back(neighbor);
          front_out_size++;
        }
      }
    }

    int num_send = send_indices.size();
    int depths[num_send];
    for(int i = 0; i < send_indices.size(); i++){
        depths[i] = result[send_indices[i]];
    }

    int num_sends[num_ranks];
    //printf("Rank: %d, before allgather num_send: %d\n", my_rank, num_send);
    MPI_Allgather(&num_send, 1, MPI_INT, num_sends, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    /*for(int i = 0; i < num_ranks; i++) {
        printf("Rank: %d, after allgather num_send: %d\n", i, num_sends[i]);
    }*/

    /*printf("send_indices size: %ld\n", send_indices.size());
    for(int i = 0; i < send_indices.size(); i++) {
        printf("Rank: %d, indice to send: %d\n", my_rank, send_indices[i]);
    }
    MPI_Barrier(MPI_COMM_WORLD);*/
    
    int recv_size = 0;
    int displs[num_ranks];
    for(int i = 0; i < num_ranks; i++) {
        displs[i] = recv_size;
        recv_size += num_sends[i];
    }

    int *recv_indices = new int[recv_size];
    int *recv_depths = new int[recv_size];
    MPI_Allgatherv(send_indices.data(), num_send, MPI_INT, recv_indices, num_sends, displs, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    /*for(int i = 0; i < recv_size; i++) {
        printf("Rank: %d, recv_indices %d: %d\n", my_rank, i, recv_indices[i]);
    }*/
    MPI_Allgatherv(depths, num_send, MPI_INT, recv_depths, num_sends, displs, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    for(int i = 0; i < recv_size; i++) {
        result[recv_indices[i]] = recv_depths[i];
    }

    MPI_Allreduce(MPI_IN_PLACE, &front_out_size, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    front_in_size = front_out_size;
    int* temp = frontier_in;
    frontier_in = frontier_out;
    frontier_out = temp;
    depth++;

    delete[] recv_indices;
    delete[] recv_depths;
  }

  return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}
