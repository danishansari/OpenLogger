CX = g++ 
CFLAGS = -Wall -g #-O6 

SRCDIR = src
OBJDIR = obj
BINDIR = bin
LIBDIR = lib

SRCS = OpenLogger.cpp 
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

INCFLAGS = -Iinclude
LDFLAGS = -L./$(LIBDIR)/ -lOpenLogger -lpthread

TRGT = $(LIBDIR)/libOpenLogger

all: $(TRGT).a #$(TRGT).so

$(TRGT).a: $(OBJS) 
	@mkdir -p $(@D)
	ar rs $@ $^

TEST_BIN = $(BINDIR)/testLogger

Test: $(TEST_BIN)

$(TEST_BIN):
	@mkdir -p $(@D)
	$(CX) test/main.cpp -o $@ $(INCFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CX) $(CFLAGS) -c $(INCFLAGS) $< -o $@

clean:
	$(RM) $(TEST_BIN)  $(OBJDIR)/* 

distclean:
	$(RM) $(TRGT).a $(TRGT).so $(TEST_BIN) $(OBJDIR)/* 
