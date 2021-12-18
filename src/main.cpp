#include <mpi.h>

#include <chrono>

#include "graph.h"
#include "bfs_sequential.cpp"
#include "mpi_frontier.cpp"
#include "mpi_vertex_dist.cpp"
#include "utils.h"

typedef int (*bfs_func)(graph_t *, int, int *);
const bfs_func bfs_functions[] = {&bfs_queue, &bfs_naive, &bfs_frontier, &mpi_vertex_dist, &mpi_frontier};

string bfs_names[] = {"bfs_queue", "bfs_naive", "bfs_frontier", "mpi_vertex_dist", "mpi_frontier"};

int run_bfs(bfs_func func, graph_t *graph, int start_vertex, int *expected, int runs) {
  int *result = new int[graph->num_vertices];

  int runtime = MAX_INT;

  for (int i = 0; i < runs; i++) {
    // Reset result array
    MPI_Barrier(MPI_COMM_WORLD);
    int next_time = func(graph, start_vertex, result);
    MPI_Barrier(MPI_COMM_WORLD);

    runtime = min(next_time, runtime);

    if (!equal(result, result + graph->num_vertices, expected)) {
      /*for (int m = 0; m < graph->num_vertices; m++)
      {
          if (expected[m] != result[m]) printf("%d: %d vs %d\n", m, expected[m],
      result[m]);
      }*/
      // Wrong result
      return -1;
    }
  }

  free(result);

  return runtime;
}

void run_all_mpi(graph_t *graph, int start_vertex, string filename, int my_rank, int runs) {
  int num_bfs = sizeof(bfs_functions) / sizeof(*bfs_functions);
  long *runtime = new long[num_bfs]();
  bool *wrong_result = new bool[num_bfs]();

  int range_from, range_to;
  if (start_vertex < 0) {
    // Run for all start vertices
    range_from = 0;
    range_to = min(graph->num_vertices, MAX_SOURCES);
  } else {
    range_from = start_vertex;
    range_to = start_vertex + 1;
  }

  for (int vertex = range_from; vertex < range_to; vertex++) {
    int *expected = new int[graph->num_vertices];
    bfs_queue(graph, vertex, expected);

    for (int i = 0; i < num_bfs; i++) {
      int next_runtime = run_bfs(bfs_functions[i], graph, vertex, expected, runs);
      if (next_runtime == -1) {
        // Wrong result
        wrong_result[i] = true;
        if (my_rank == 0) printf("%d vertex led to WRONG_RESULT\n", vertex);
      } else {
        runtime[i] += (long)next_runtime;
      }
    }

    free(expected);
  }

  for (int i = 0; i < num_bfs; i++) {
    double avg_runtime = runtime[i] / (range_to - range_from);

    if (!wrong_result[i]) {
      if (my_rank == 0) printf("%s,%s,total=%ld,avg=%f\n", filename.c_str(), bfs_names[i].c_str(), runtime[i], avg_runtime);
    } else {
      if (my_rank == 0) printf("%s,%s,WRONG_RESULT\n", filename.c_str(), bfs_names[i].c_str());
    }
  }
}

int main(int argc, char *argv[]) {
  int my_rank, num_proc;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

  if (argc < 4 && my_rank == 0) {
    printf("Usage: %s filename(path) vertex_id(int) runs(int) method(int)\n", argv[0]);
    exit(1);
  }

  string filename = argv[1];
  int start_vertex = atoi(argv[2]);
  int runs = atoi(argv[3]);
  int METHOD = -1;
  if (argc > 4) METHOD = atoi(argv[4]);

  auto t1 = Time::now();
  graph_t *graph = read_graph(filename, my_rank);
  auto t2 = Time::now();

  if (filename.find("/") >= 0) filename = filename.substr(filename.rfind("/") + 1);
  if (my_rank == 0) printf("Loading %s took %ld us\n\n", filename.c_str(), chrono::duration_cast<us>(t2 - t1).count());
  MPI_Barrier(MPI_COMM_WORLD);

  if (METHOD == -1) {
    run_all_mpi(graph, start_vertex, filename, my_rank, runs);
  } else {
    int *expected = new int[graph->num_vertices];
    bfs_queue(graph, start_vertex, expected);
    MPI_Barrier(MPI_COMM_WORLD);

    if (METHOD >= 0) {
      t1 = Time::now();
      int next_runtime = run_bfs(bfs_functions[METHOD], graph, start_vertex, expected, runs);
      MPI_Barrier(MPI_COMM_WORLD);
      t2 = Time::now();

      // double avg_runtime = runtime[i] / (range_to - range_from);
      printf("%s took %d us on processor %d\n", bfs_names[METHOD].c_str(), next_runtime, my_rank);
      if (my_rank == 0 && next_runtime > 0 && num_proc > 1) printf("%s + synchronization took %ld us on all processors\n", bfs_names[METHOD].c_str(), chrono::duration_cast<us>(t2 - t1).count());
    }
  }

  MPI_Finalize();
}