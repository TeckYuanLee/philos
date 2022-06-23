NAME	=	philo
CC		=	gcc
CFLAGS	=	-Wall -Werror -Wextra -pthread -I.#-fsanitize=thread -g3 -I.
ARFLAGS	= 	rcs

SRCS	=	$(wildcard srcs/*.c)
OBJS	=	$(SRCS:srcs/%.c=obj/%.o)

obj/%.o:	srcs/%.c
		@mkdir -p obj
		$(CC) $(CFLAGS) -c $< -o $@ -Iincludes

all: $(NAME)

$(NAME):		$(OBJS) 
		$(CC) $(CFLAGS) $(SRCS) -o $(NAME)

clean:
		@rm -f $(OBJS)

fclean:	clean
			@rm -f $(NAME)

re:		fclean all

.PHONY: all clean fclean re