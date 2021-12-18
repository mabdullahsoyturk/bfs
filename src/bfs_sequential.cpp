#include "utils.h"

int bfs_queue(graph_t *graph, int start_vertex, int *result) {
  int num_vertices = graph->num_vertices;
  bool *visited = new bool[num_vertices];
  fill_n(result, num_vertices, MAX_DIST);
  fill_n(visited, num_vertices, 0);

  auto start_time = Time::now();

  visited[start_vertex] = true;
  result[start_vertex] = 0;

  queue<int> next_vertices;
  next_vertices.push(start_vertex);
  int max_depth = 0;

  while (!next_vertices.empty()) {
    int vertex = next_vertices.front();
    next_vertices.pop();

    for (int n = graph->v_adj_begin[vertex];
         n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
      int neighbor = graph->v_adj_list[n];

      if (!visited[neighbor]) {
        visited[neighbor] = true;
        result[neighbor] = result[vertex] + 1;
        if (result[neighbor] > max_depth)
          max_depth = result[neighbor];
        next_vertices.push(neighbor);
      }
    }
  }

  // print_result(graph, result, max_depth+1);
  return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}

int bfs_naive(graph_t *graph, int start_vertex, int *result) {
  int num_vertices = graph->num_vertices;
  fill_n(result, num_vertices, MAX_DIST);

  auto start_time = Time::now();

  int depth = 0;
  result[start_vertex] = depth;

  int keep_going = true;

  while (keep_going) {
    keep_going = false;

    for (int vertex = 0; vertex < num_vertices; vertex++) {
      if (result[vertex] == depth) {
        for (int n = graph->v_adj_begin[vertex];
             n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex];
             n++) {
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

int bfs_frontier(graph_t *graph, int start_vertex, int *result) {
  int num_vertices = graph->num_vertices;
  fill_n(result, num_vertices, MAX_DIST);

  auto start_time = Time::now();

  int depth = 0;
  result[start_vertex] = depth;

  int *frontier_in = new int[num_vertices];
  int *frontier_out = new int[num_vertices];
  frontier_in[0] = start_vertex;
  int front_in_size = 1;
  int front_out_size = 0;

  while (front_in_size != 0) {
    front_out_size = 0;

    for (int v = 0; v < front_in_size; v++) {
      int vertex = frontier_in[v];

      for (int n = graph->v_adj_begin[vertex];
           n < graph->v_adj_begin[vertex] + graph->v_adj_length[vertex]; n++) {
        int neighbor = graph->v_adj_list[n];

        if (result[neighbor] > depth + 1) {
          result[neighbor] = depth + 1;
          frontier_out[front_out_size] = neighbor;
          front_out_size++;
        }
      }
    }

    front_in_size = front_out_size;
    int *temp = frontier_in;
    frontier_in = frontier_out;
    frontier_out = temp;
    depth++;
  }

  // print_result(graph, result, depth);
  return std::chrono::duration_cast<us>(Time::now() - start_time).count();
}