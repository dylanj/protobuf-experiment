CC=gcc
CFLAGS=-g -c -std=c11
LDFLAGS=`sdl2-config --libs` -lprotobuf-c -lenet
CLIENT_LDFLAGS=$(LDFLAGS) -lGL
PROTOBUF= protobufs/wrapper.pb-c.c protobufs/input.pb-c.c protobufs/handshake.pb-c.c protobufs/lobby.pb-c.c protobufs/client.pb-c.c
SHARED_SOURCES=$(PROTOBUF) net.c client_funcs.c game.c input.c
SERVER_SOURCES=$(SHARED_SOURCES) server.c
CLIENT_SOURCES=$(SHARED_SOURCES) client.c window.c
SERVER_OBJECTS=$(SERVER_SOURCES:%.c=%.o)
CLIENT_OBJECTS=$(CLIENT_SOURCES:%.c=%.o)
SERVER_EXECUTABLE=server
CLIENT_EXECUTABLE=client

all: $(CLIENT_SOURCES) $(SERVER_SOURCES) $(SERVER_EXECUTABLE) $(CLIENT_EXECUTABLE)

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@
$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS)
	$(CC) $(CLIENT_LDFLAGS) $(CLIENT_OBJECTS) -o $@

.PHONY: clean protobufs
clean:
	rm *.o protobufs/*.o client server

protobufs:
	protoc-c protobufs/*.proto --c_out=protobufs/ --proto_path=protobufs/

.c.o:
	$(CC) $(CFLAGS) $< -o $@


