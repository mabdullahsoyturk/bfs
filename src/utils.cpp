#include "utils.h"

void print_queue(std::queue<int> q) {
  while (!q.empty()) {
    std::cout << q.front() << " ";
    q.pop();
  }
  std::cout << std::endl;
}

void print_result(graph_t *graph, int *result, int max_depth) {
  for (int i = 0; i < max_depth; i++) {
    int *lvl = new int[graph->num_vertices];
    int idx = 0;
    for (int v = 0; v < graph->num_vertices; v++) {
      if (result[v] == i) {
        lvl[idx] = v;
        idx++;
      }
    }
    std::cout << "Depth " << i << "| Size " << idx << ": ";

    for (int lv = 0; lv < idx - 1; lv++) {
      std::cout << lvl[lv] << ", ";
    }
    std::cout << lvl[idx - 1] << std::endl << std::endl;

    for (int lv = 0; lv < idx; lv++) {
      int v = lvl[lv];
      int n = graph->v_adj_begin[v];
      std::cout << v << " | size = " << graph->v_adj_length[v] << ": ";
      while (n < graph->v_adj_begin[v] + graph->v_adj_length[v] - 1) {
        int neighbor = graph->v_adj_list[n];
        std::cout << neighbor << ", ";
        n++;
      }
      std::cout << graph->v_adj_list[n] << std::endl;
    }
    std::cout << std::endl;
  }
}