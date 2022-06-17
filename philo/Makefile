NAME	=	philo
CC		=	gcc
CFLAGS	=	-Wall -Werror -Wextra -pthread -I. -c
ARFLAGS	= 	rcs

SRCS	=	$(wildcard srcs/*.c)
OBJS	=	$(SRCS:srcs/%.c=obj/%.o)

obj/%.o:	srcs/%.c
		@mkdir -p obj
		@gcc $(CFLAGS) -c $< -o $@ -Iincludes

all: $(NAME)

$(NAME):		$(OBJS) 
		$(CC) $(SRCS) -o $(NAME)
		@$(MAKE) clean -sC

clean:
		@rm -f $(OBJS)

fclean:	clean
			@rm -f $(NAME)

re:		fclean all

.PHONY: all clean fclean re