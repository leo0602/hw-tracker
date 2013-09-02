all: hw

hw: hw.c
	gcc -Wall -o hw -l sqlite3 hw.c

clean:
	rm -rf hw hw.db
