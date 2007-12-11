CC=gcc
CFLAGS=-ansi -Wall -g
LDFLAGS=-lri
PROJ=proj6
OBJ=proj6.o draw.o
SDR=tile.sdr

$(PROJ).tif: $(PROJ).rib
	rndr $(PROJ).rib
	eog $(PROJ).tif

$(PROJ).rib: $(PROJ)
	./$(PROJ)

$(PROJ): $(OBJ) $(SDR)
	$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(PROJ)

-include $(OBJ:.o=.d)

%.d: %.c
	$(CC) $(CFLAGS) -M $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.sdr: %.sl
	sdrc $< -o $@

clean:
	-rm -f $(PROJ).tif $(PROJ).rib $(PROJ) $(OBJ) $(SDR) $(OBJ:.o=.d)
