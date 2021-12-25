#include "graph.h"

graph_t *read_graph(string filename, int my_rank) {
  ifstream infile(filename);

  if (!infile.is_open() && my_rank == 0) {
    printf("File did not open\n");
    exit(1);
  }

  int from, to;
  int num_edges = 0;

  map<int, int> index_map;
  int next_index = 0;
  string line;
  // skip the comments, num vertices and edges line
  while (infile.get() == '%') getline(infile, line);
  getline(infile, line);

  while (infile >> from >> to) {
    if (!index_map.count(from)) {
      index_map[from] = next_index++;
    }

    if (!index_map.count(to)) {
      index_map[to] = next_index++;
    }

    num_edges++;
  }

  int num_vertices = next_index;

  if (my_rank == 0) printf("Input file has %d vertices and %i edges\n", num_vertices, num_edges);

  // Build adajacency lists (still reading file)
  infile.clear();
  infile.seekg(0, ios::beg);
  // skip the comments, num vertices and edges line
  while (infile.get() == '%') getline(infile, line);
  getline(infile, line);

  int *v_adj_begin = new int[num_vertices];
  int *v_adj_length = new int[num_vertices];
  vector<int> *v_adj_lists = new vector<int>[num_vertices]();
  int *v_adj_list = new int[num_edges];

  int max_degree = 0;

  while (infile >> from >> to) {
    v_adj_lists[index_map[from]].push_back(index_map[to]);
    max_degree = max(max_degree, (int)v_adj_lists[index_map[from]].size());
  }

  // Show degree distribution
  // printf("Compute out-degree histogram\n");
  int *degree_histogram = new int[max_degree + 1]();
  unsigned long long total_degree = 0;

  for (int i = 0; i < num_vertices; i++) {
    degree_histogram[v_adj_lists[i].size()]++;
    total_degree += v_adj_lists[i].size();
  }

  double avg_degree = total_degree / (double)num_vertices;
  double degree_variance = 0.0;

  for (int i = 0; i < num_vertices; i++) {
    degree_variance += (avg_degree - v_adj_lists[i].size()) * (avg_degree - v_adj_lists[i].size());
  }
  degree_variance /= num_vertices;

  double degree_stddev = sqrt(degree_variance);

  // Compute median
  int *degs = new int[num_vertices];

  for (int i = 0; i < num_vertices; i++) {
    degs[i] = v_adj_lists[i].size();
  }

  // sort(degs, degs + num_vertices);

  if (my_rank == 0) printf("avg deg = %f, deg stddev = %f, median = %i\n", avg_degree, degree_stddev, degs[num_vertices / 2]);

  /*
  printf("Histogram for Vertex Degrees\n");

  for (int i = 0; i < max_degree + 1; i++)
  {
      printf("deg %i        %i\n", i, degree_histogram[i]);
  }
  */

  // Generate data structure
  // printf("Build ajacency lists\n");
  int next_offset = 0;

  for (int i = 0; i < num_vertices; i++) {
    int list_size = v_adj_lists[i].size();

    /*printf("\nvertex %d | begin = %d | size = %d :", i, next_offset,
    list_size); for (int j = 0; j < list_size; j++)
    {
        printf(" %d", v_adj_lists[i][j]);
    }*/

    v_adj_begin[i] = next_offset;
    v_adj_length[i] = list_size;

    memcpy(v_adj_list + next_offset, &v_adj_lists[i][0], list_size * sizeof(int));
    next_offset += list_size;
  }

  /*for(int i = 0; i < num_vertices; i++) {
    printf("v_adj_begin[%d]: %d\n", i, v_adj_begin[i]);
  }

  for(int i = 0; i < num_vertices; i++) {
    printf("v_adj_length[%d]: %d\n", i, v_adj_length[i]);
  }

  for(int i = 0; i < num_edges; i++) {
    printf("v_adj_list[%d]: %d\n", i, v_adj_list[i]);
  }*/

  graph_t *graph = new graph_t;
  graph->v_adj_list = v_adj_list;
  graph->v_adj_begin = v_adj_begin;
  graph->v_adj_length = v_adj_length;
  graph->num_vertices = num_vertices;
  graph->num_edges = num_edges;
  return graph;
}