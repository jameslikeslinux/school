CC=gcc
CFLAGS=-ansi -Wall -g
LDFLAGS=-lri
PROJ=proj1
OBJ=proj1.o

$(PROJ): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(PROJ)

-include $(OBJ:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -M $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(PROJ) $(OBJ) $(OBJ:.o=.d)
