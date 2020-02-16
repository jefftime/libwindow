srcdir = ./src
libdir ?= ./libs
bindir ?= ./bin
inclibs = sized_types window
defs ?=

# Computed
src = $(wildcard $(srcdir)/*.c)
obj = $(addprefix $(bindir)/,$(notdir $(src:%.c=%.o)))
cflags += -fPIC $(addprefix -I,$(incdirs)) $(addprefix -D,$(defs))
incdirs = $(addsuffix /include,$(addprefix $(libdir)/,$(inclibs)))

$(bindir)/libwindow.a: $(obj)
	$(AR) rcs $@ $(obj)

$(bindir)/%.o: $(srcdir)/%.c
	$(CC) -c $(cflags) -o $@ $^

.PHONY: clean
clean:
	rm -f $(bindir)/libwindow.a $(obj)

