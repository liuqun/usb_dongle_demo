demo_programs += main
CFLAGS := -g -O0
CFLAGS += -pthread
LIBS += -ldl

.PHONY: all
.PHONY: clean
all: $(demo_programs)
clean:
	$(RM) -rf $(demo_programs)
main: main.c base_type.h skf_type.h skf.h
	$(LINK.c) -o $@ $< $(LIBS)
