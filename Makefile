CC = mpic++
INCLUDE = include
TARGET = bfs

build: $(TARGET)

bfs: src/main.cpp src/graph.cpp src/utils.cpp
	$(CC) $^ -o $@ -I$(INCLUDE)

clean:
	rm $(TARGET)
