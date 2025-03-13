IDIR = ../common
VK_SDK_PATH = C:\VulkanSDK\1.3.296.0
CC = gcc
CFLAGS = -I$(IDIR) -I$(VK_SDK_PATH)\include

ODIR = obj
LDIR = ../lib

_DEPS = gbsp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = gbspmake.o gbspfunc.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 