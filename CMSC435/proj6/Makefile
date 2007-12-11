CC=gcc
CFLAGS=-ansi -Wall -g
LDFLAGS=-lri
PROJ=proj1
OBJ=proj1.o draw.o

$(PROJ).tif: $(PROJ).rib
	rndr $(PROJ).rib
	eog $(PROJ).tif

$(PROJ).rib: $(PROJ)
	./$(PROJ)

$(PROJ): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(PROJ)

-include $(OBJ:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -M $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f $(PROJ).tif $(PROJ).rib $(PROJ) $(OBJ) $(OBJ:.o=.d)
