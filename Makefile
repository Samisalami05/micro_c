CC := gcc
NAME := main
SOURCEDIR := src
BUILDDIR := build

SRCS := $(shell find $(SOURCEDIR) -name '*.c')
OBJS := $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

CFLAGS := -Wall

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/*.d
	rm -rf $(BUILDDIR)/*.o
	rm $(NAME)

-include $(OBJS:.o=.d)