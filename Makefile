inclibs = sized_types window

# Computed
src = $(wildcard $(srcdir)/*.c)
obj = $(addprefix $(bindir)/,$(notdir $(src:%.c=%.o)))
cflags = -fPIC \
	-std=c90 \
	-pedantic-errors \
	-Wall \
	-Wconversion \
	$(addprefix -I,$(incdirs)) \
	--no-standard-libraries
incdirs = $(addsuffix /include,$(addprefix $(libdir)/,$(inclibs)))
srcdir = ./src
libdir ?= ./libs
bindir ?= ./bin

$(bindir)/libwindow.a: $(obj)
	$(AR) rcs $@ $(obj)

$(bindir)/%.o: $(srcdir)/%.c
	$(CC) -c $(cflags) -o $@ $^

.PHONY: clean
clean:
	rm -f $(bindir)/libwindow.a $(obj)

