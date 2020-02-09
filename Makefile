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

green = '\033[1;32m'
nocolor = '\033[0m'

$(bindir)/libwindow.a: $(obj)
# @echo -e $(green)Link $(notdir $@)$(nocolor)
	$(AR) rcs $@ $(obj)

$(bindir)/%.o: $(srcdir)/%.c
# @echo $(notdir $@)...
	$(CC) -c $(cflags) -o $@ $^

.PHONY: clean
clean:
	rm -f $(bindir)/libwindow.a $(obj)

