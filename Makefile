
COLOR_RESET  = \033[0m
COLOR_RED    = \033[91m
COLOR_GREEN  = \033[92m
COLOR_YELLOW = \033[93m
COLOR_BOLD   = \033[1m

MAKEFLAGS += --no-print-directory

NAME = lem_in

OBJ_DIR = obj
SRCS = main.c utils.c algo.c interface.c utils_libft.c animation.c
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

CC = cc
CFLAGS = -g
CPPFLAGS = -I include

RAYLIB_DIR = $(HOME)/raylib_src
RAYLIB_LIB = $(RAYLIB_DIR)/src/libraylib.a

PRINTF_DIR = printf
PRINTF = $(PRINTF_DIR)/printf.a

CPPFLAGS += -I$(RAYLIB_DIR)/src -I$(PRINTF_DIR)

LDFLAGS = -L$(RAYLIB_DIR)/src
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

RM = rm -f
DIR_DUP = mkdir -p $(@D)

all: $(NAME)

$(PRINTF):
	$(MAKE) -C $(PRINTF_DIR)

$(NAME): $(OBJS) $(PRINTF)
	$(CC) $(OBJS) $(PRINTF) -o $(NAME) $(LDFLAGS) $(LDLIBS)
	@printf "$(COLOR_GREEN)$(COLOR_BOLD)%s compile avec succes !$(COLOR_RESET)\n" "$(NAME)"

$(OBJ_DIR)/%.o: %.c main.h
	@$(DIR_DUP)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	@$(RM) $(OBJS)
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	@$(RM) $(NAME)
	@$(MAKE) -C $(PRINTF_DIR) fclean

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re

.PHONY: all clean fclean re
