CC=gcc
CFLAGS=-g -c -Wall -std=c11
LDFLAGS=`sdl-config --libs` -lSDL2_net -lprotobuf-c
PROTOBUF= protobufs/wrapper.pb-c.c protobufs/input.pb-c.c protobufs/handshake.pb-c.c
SHARED_SOURCES=$(PROTOBUF) net.c client_funcs.c game.c
SERVER_SOURCES=$(SHARED_SOURCES) server.c
CLIENT_SOURCES=$(SHARED_SOURCES) server_conn.c client.c
SERVER_OBJECTS=$(SERVER_SOURCES:%.c=%.o)
CLIENT_OBJECTS=$(CLIENT_SOURCES:%.c=%.o)
SERVER_EXECUTABLE=server
CLIENT_EXECUTABLE=client

all: $(CLIENT_SOURCES) $(SERVER_SOURCES) $(SERVER_EXECUTABLE) $(CLIENT_EXECUTABLE)

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@
$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@

clean:
	rm *.o protobugs/*.o client server

.c.o:
	$(CC) $(CFLAGS) $< -o $@


