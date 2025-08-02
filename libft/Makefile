# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ltheveni <ltheveni@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/12 18:30:28 by ltheveni          #+#    #+#              #
#    Updated: 2025/08/02 18:52:32 by ltheveni         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
SRCS = ft_isalpha.c\
	   ft_isdigit.c\
	   ft_isalnum.c\
	   ft_isascii.c\
	   ft_isprint.c\
	   ft_strlen.c\
	   ft_memset.c\
	   ft_bzero.c\
	   ft_memcpy.c\
	   ft_memmove.c\
	   ft_strlcpy.c\
	   ft_strlcat.c\
	   ft_toupper.c\
	   ft_tolower.c\
	   ft_strchr.c\
	   ft_strrchr.c\
	   ft_strcmp.c\
	   ft_strncmp.c\
	   ft_memchr.c\
	   ft_memcmp.c\
	   ft_strnstr.c\
	   ft_atoi.c\
	   ft_calloc.c\
	   ft_strdup.c\
	   ft_substr.c\
	   ft_strjoin.c\
	   ft_strtrim.c\
	   ft_split.c\
	   ft_itoa.c\
	   ft_strmapi.c\
	   ft_striteri.c\
	   ft_putchar_fd.c\
	   ft_putstr_fd.c\
	   ft_putendl_fd.c\
	   ft_putnbr_fd.c\
	   ft_putsize_fd.c\
	   ft_putsize_base_fd.c\
	   get_next_line.c
SRCS_BONUS = ft_lstnew_bonus.c\
			 ft_lstadd_front_bonus.c\
			 ft_lstsize_bonus.c\
			 ft_lstlast_bonus.c\
			 ft_lstadd_back_bonus.c\
			 ft_lstdelone_bonus.c\
			 ft_lstclear_bonus.c\
			 ft_lstiter_bonus.c\
			 ft_lstmap_bonus.c
SRCS_PRINTF = ft_printf/ft_printf.c\
			  ft_printf/ft_utils_printf.c
OBJS = $(SRCS:.c=.o)
OBJS_BONUS = $(SRCS_BONUS:.c=.o)
OBJS_PRINTF = $(SRCS_PRINTF:.c=.o)
INCLUDE = -I . -I ft_printf

CC = gcc
DEBUG_FLAGS ?=
CFLAGS = -Wall -Werror -Wextra $(DEBUG_FLAGS)
LIB = ar rc
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS) $(OBJS_PRINTF)
	@$(LIB) $(NAME) $(OBJS) $(OBJS_PRINTF)
	@echo "\nCompiled"

bonus: $(OBJS_BONUS)
	@$(LIB) $(NAME) $(OBJS) $(OBJS_BONUS)
	@$(MAKE) printf
	@echo "\nCompiled"

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	$(RM) $(OBJS) $(OBJS_BONUS) $(OBJS_PRINTF)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re
