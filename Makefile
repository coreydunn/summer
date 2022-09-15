CFLAGS=  -Wall -Wfatal-errors
LDFLAGS= -pthread -lpthread -lm
OBJS=    summer singlethread

all: $(OBJS)
summer: main.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
singlethread: singlethread.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)
clean:
	$(RM) *.o $(OBJS)
