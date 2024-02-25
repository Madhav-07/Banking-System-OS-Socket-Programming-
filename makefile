server:
	gcc IMT2020022_server.c -o server

client:
	gcc IMT2020022_client.c -o client

clean:
	rm client server