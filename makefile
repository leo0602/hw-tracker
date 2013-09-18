all: hw

hw: hw.c hw_client.c
	gcc -g -Wall -o hw-track -l sqlite3 hw.c hw_client.c

clean:
	rm -rf hw-track hw_client hw.db
