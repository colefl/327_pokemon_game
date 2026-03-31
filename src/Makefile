main: main.cpp heap.c world_gen.cpp pathMaker.cpp entities/Entity.cpp entities/entity_move_q.cpp Maps.cpp point_queue.cpp
	gcc -c heap.c -o heap.o -g -Wall -Werror
	g++ main.cpp heap.o world_gen.cpp pathMaker.cpp entities/Entity.cpp entities/entity_move_q.cpp Maps.cpp point_queue.cpp -o main -g -Wall -Werror -lncurses

clean:
	rm -rf main heap.o