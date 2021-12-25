CC = mpic++
INCLUDE = include
TARGET = bfs

build: $(TARGET)

bfs: src/main.cpp src/graph.cpp src/utils.cpp
	$(CC) $^ -o $@ -I$(INCLUDE)

run:
	./bfs graphs/test.mtx 0 1 1
#	./bfs <graph_path> <start_vertex> <runs> <method>

clean:
	rm $(TARGET)
