CC=g++
CFLAGS=-Wall -I. -I/usr/include/
LIBS=-L/usr/local/lib -lm -lgsl -lgslcblas

SRC=vanderpol
OBJDIR=.
# DEPS = vanderpol.h
DEPS=""
OBJ=$(OBJDIR)/$(SRC).o

$(OBJDIR)/%.o: %.cpp $(DEPS)
	$(CC) -c $@ $< $(CFLAGS)

$(SRC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

     
.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ $(SRC)
