all: chess.out
	./chess.out
chess.out:
	gcc -o chess.out chess.c coord.c initialize.c moves.c print_functions.c

clean:
	rm *.out