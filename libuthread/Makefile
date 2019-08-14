# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o

CC := gcc
CFLAGS := -Wall -Werror
CFLAGS += -g
PANDOC := pandoc

ifneq ($(V),1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(objs)
	ar rcs libuthread.a $(objs)

%.o: %.c
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	$(Q)rm -f $(targets) $(objs) $(deps)
