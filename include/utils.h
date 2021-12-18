#pragma once

#include "graph.h"
#include <iostream>
#include <queue>

#define MAX_INT 1073741824
#define MAX_DIST MAX_INT
#define MAX_SOURCES 1024

using Time = chrono::high_resolution_clock;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;

void print_queue(std::queue<int> q);
void print_result(graph_t *graph, int *result, int max_depth);