all: hw

hw: hw.c
	gcc -Wall -o hw -l sqlite3 hw.c hw_client.c

clean:
	rm -rf hw hw_client hw.db
