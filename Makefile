all: server client test

server: src/server.c src/string_manipulation.c src/system_gen.c src/manual.c src/connections.c src/item.c
	gcc src/server.c src/string_manipulation.c src/system_gen.c src/manual.c src/connections.c src/item.c -o bin/server -lm -lpthread -ldl -std=gnu99

client: src/client.c src/manual.c src/string_manipulation.c
	gcc src/client.c src/manual.c src/string_manipulation.c -o bin/client -lm -lncurses -lpthread -std=gnu99

test: src/test_client.c src/manual.c src/string_manipulation.c
	gcc src/test_client.c src/manual.c src/string_manipulation.c -o bin/test_client -lm -lpthread -std=gnu99 
