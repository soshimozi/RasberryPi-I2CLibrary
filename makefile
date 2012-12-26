#directories
BUILD=obj

#tools
RANLIB=ranlib

AR=ar
ARFLAGS=cru
libi2c_a_AR=$(AR) $(ARFLAGS)

CCLIB=cc

CC=gcc
CFLAGS=-c -Wall -O2

#dependencies
DEPS=pi_i2c.h

_OBJ=pi_i2c.o
OBJ=$(patsubst %,$(BUILD)/%,$(_OBJ))

all: libi2c.so

libi2c.a: $(OBJ)
        -rm -f libi2c.a
        $(libi2c_a_AR) libi2c.a $(OBJ)
        $(RANLIB) libi2c.a

libi2c.so: libi2c.a
        $(CCLIB) -shared $(BUILD)/pi_i2c.o -o $@

$(BUILD)/%.o: %.c $(DEPS)
        $(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
        rm -f $(BUILD)/*.o *~ core
        rm -f *.so
        rm -f *.a
