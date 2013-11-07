CFLAGS=-I../mely/includes -g -O2
LDFLAGS=-lpthread -lrt -lpapi -lmely ../mely/.libs/libmely.so
APPLICATION_DEPENDS=../mely/libmely.la

PREC := $(wildcard *.C)
TARGETS := $(PREC:%.C=%)

all : $(TARGETS)

$(TARGETS) : % : %.C ${APPLICATION_DEPENDS}
	${CXX} $(CFLAGS) -o $@ $< $(LDFLAGS)

clean :
	rm -f $(TARGETS)
	rm -f *.o
	rm -f core*
