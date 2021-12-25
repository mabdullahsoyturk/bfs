#include "utils.h"
#include <mpi.h>

int mpi_vertex_dist(graph_t *graph, int start_vertex, int *result) {
  /*
    Each process has: 
    v_length = [2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3]
    v_begin  = [0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34]
    v_adj_list = [0,2,0,3,4,3,2,1,7,6,4,1,4,9,9,10,7,11,5,6,12,11,10,8,13,8,16,15,13,17,13,17,16,17,14,8,15]
  */

  int num_vertices = graph->num_vertices;
  fill_n(result, num_vertices, MAX_DIST);

  int depth = 0;
  result[start_vertex] = depth;

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int local_num_vertices = (num_vertices / size) + 1;
  int local_num_edges = (graph->num_edges / size) + 1;

  int vertex_recv_size;
  if(rank != size - 1) {
      vertex_recv_size = local_num_vertices;
  }else {
      vertex_recv_size = num_vertices - (local_num_vertices * (size - 1));
  }
  
  int* v_adj_begin = new int[vertex_recv_size];
  int* v_adj_length = new int[vertex_recv_size];

  if(rank == 0) {
    int vertex_displacements[size];
    int vertex_recv_sizes[size];

    for(int i = 0; i < size; i++) {
      vertex_recv_sizes[i] = local_num_vertices;
      vertex_displacements[i] = local_num_vertices * i;
    }
    vertex_recv_sizes[size - 1] = num_vertices - (local_num_vertices * (size - 1));

    MPI_Scatterv(graph->v_adj_begin, vertex_recv_sizes, vertex_displacements, MPI_INT, v_adj_begin, vertex_recv_sizes[rank], MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(graph->v_adj_length, vertex_recv_sizes, vertex_displacements, MPI_INT, v_adj_length, vertex_recv_sizes[rank], MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, v_adj_begin, vertex_recv_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, v_adj_length, vertex_recv_size, MPI_INT, 0, MPI_COMM_WORLD);
  }

  int edge_recv_size;
  if(rank != size - 1) {
      edge_recv_size = local_num_edges;
  }else {
      edge_recv_size = graph->num_edges - (local_num_edges * (size - 1));
  }

  int* v_adj_list = new int[edge_recv_size];
  if(rank == 0) {
    int edge_displacements[size];
    int edge_recv_sizes[size];

    for(int i = 0; i < size; i++) {
      edge_recv_sizes[i] = local_num_edges;
      edge_displacements[i] = local_num_edges * i;
    }
    edge_recv_sizes[size - 1] = graph->num_edges - (local_num_edges * (size - 1));

    MPI_Scatterv(graph->v_adj_list, edge_recv_sizes, edge_displacements, MPI_INT, v_adj_list, edge_recv_sizes[rank], MPI_INT, 0, MPI_COMM_WORLD);
  } else {
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, v_adj_list, edge_recv_size, MPI_INT, 0, MPI_COMM_WORLD);
  }
  
  /*
    After distrubiton process 0 has:
      v_length = [2,2,2,2,2]
      v_begin  = [0,2,4,6,8]
      v_adj_list = [0,2,0,3,4,3,2,1,7,6]

    After distribution process size - 1 has:
      v_length = [2,2,3]
      v_begin  = [30,32,34]
      v_adj_list = [13,17,16,17,14,8,15]
  */

  auto start_time = Time::now();
  int keep_going = true;
  while (keep_going) {
    keep_going = false;

    for (int vertex = 0; vertex < vertex_recv_size; vertex++) {
      if (result[vertex] == depth) {
        for (int n = graph->v_adj_begin[vertex]; n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
          int neighbor = graph->v_adj_list[n];

          if (result[neighbor] > depth + 1) {
            result[neighbor] = depth + 1;
            keep_going = true;
          }
        }
      }
    }

    depth++;
  }

  // print_result(graph, result, depth);
  return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}