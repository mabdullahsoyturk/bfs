#include "utils.h"

int mpi_frontier(graph_t* graph, int start_vertex, int* result) {
  int num_vertices = graph->num_vertices;
  fill_n(result, num_vertices, MAX_DIST);
  
  int my_rank, num_ranks;
  int my_start_vertex, my_end_vertex;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

  int *recv_indices = new int[num_vertices];
  int *recv_depths = new int[num_vertices];

  int* frontier_in = new int[num_vertices];
  int* frontier_out = new int[num_vertices];
  frontier_in[0] = start_vertex;
  int front_in_size = 1;
  int front_out_size = 0;

  int num_sends[num_ranks];
  int displs[num_ranks];

  auto start_time = Time::now();

  int depth = 0;
  result[start_vertex] = depth;

  while (front_in_size != 0) {
    front_out_size = 0;

    if(front_in_size <= num_ranks) {
        if(my_rank < front_in_size) {
            my_start_vertex = my_rank;
            my_end_vertex = my_rank + 1;
        }else {
            my_start_vertex = 0;
            my_end_vertex = 0;
        }
    }else {
        int work = front_in_size / num_ranks;
        if (my_rank != num_ranks - 1) {
          my_start_vertex = my_rank * work;
          my_end_vertex = my_start_vertex + work;
      } else {
          my_start_vertex = (my_rank - 1) * work + work;
          my_end_vertex = front_in_size;
      }
    }
    // printf("Rank: %d, my_work: %d\n", my_rank, my_end_vertex - my_start_vertex);
    // printf("Rank: %d, front_in_size: %d, my_start_vertex: %d, my_end_vertex: %d\n\n", my_rank, front_in_size, my_start_vertex, my_end_vertex);
    // MPI_Barrier(MPI_COMM_WORLD);

    std::vector<int> send_indices;
    std::vector<int> depths;
    for (int v = my_start_vertex; v < my_end_vertex; v++) {
      int vertex = frontier_in[v];

      for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
        int neighbor = graph->v_adj_list[n];

        if (result[neighbor] > depth + 1) {
          result[neighbor] = depth + 1;
          frontier_out[front_out_size] = neighbor;
          send_indices.push_back(neighbor);
          depths.push_back(depth + 1);
          front_out_size++;
        }
      }
    }

    MPI_Allgather(&front_out_size, 1, MPI_INT, num_sends, 1, MPI_INT, MPI_COMM_WORLD);
    // MPI_Barrier(MPI_COMM_WORLD);

    bool send = false;

    for(int i = 0; i < num_ranks; i++) {
        if(num_sends[i] != 0) {
            send = true;
        }
    }

    if(send) {
      int recv_size = 0;
      for(int i = 0; i < num_ranks; i++) {
          displs[i] = recv_size;
          recv_size += num_sends[i];
      }

      MPI_Allgatherv(send_indices.data(), front_out_size, MPI_INT, recv_indices, num_sends, displs, MPI_INT, MPI_COMM_WORLD);
      MPI_Allgatherv(depths.data(), front_out_size, MPI_INT, recv_depths, num_sends, displs, MPI_INT, MPI_COMM_WORLD);
      // MPI_Barrier(MPI_COMM_WORLD);

      front_out_size = recv_size;
    
      for(int i = 0; i < recv_size; i++) {
          result[recv_indices[i]] = recv_depths[i];
          frontier_out[i] = recv_indices[i];
      }
    }

    front_in_size = front_out_size;
    int* temp = frontier_in;
    frontier_in = frontier_out;
    frontier_out = temp;
    depth++;
  }

  auto time = std::chrono::duration_cast<us>(Time::now() - start_time).count();

  // Free
  delete[] recv_indices;
  delete[] recv_depths;
  delete[] frontier_in;
  delete[] frontier_out;

  return time;
}
