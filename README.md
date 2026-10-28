*This project has been created as part of the 42 curriculum by tisilva.*

# Libft

## Description

Libft is a C programming project from the 42 curriculum. Its goal is to create a personal C library by reimplementing standard C library functions and developing additional utility functions.

The project focuses on memory management, pointers, strings, character manipulation, and linked lists. The resulting static library, `libft.a`, can be reused in other C projects.

## Instructions

### Requirements

- A C compiler, such as `gcc` or `clang`.
- GNU Make.

### Compilation

Run the following command in the project directory:

```bash
make
```

This generates the static library `libft.a`.

Available Makefile rules:

```bash
make all
make clean
make fclean
make re
```

- `make all`: compiles the library.
- `make clean`: removes object files.
- `make fclean`: removes object files and the library.
- `make re`: rebuilds the library.

### Using the library

Include the header in your C source file:

```c
#include "libft.h"
```

Compile your program with the library:

```bash
cc -Wall -Wextra -Werror main.c libft.a -o program
```

Run the executable:

```bash
./program
```

## Library Description

### Part 1 — Libc functions

Reimplementations of standard C functions.

**Character classification**

- `ft_isalpha`: checks whether a character is alphabetic.
- `ft_isdigit`: checks whether a character is a decimal digit.
- `ft_isalnum`: checks whether a character is alphanumeric.
- `ft_isascii`: checks whether a value belongs to the ASCII range.
- `ft_isprint`: checks whether a character is printable.

**Memory manipulation**

- `ft_memset`: fills a memory area with a specified byte.
- `ft_bzero`: sets a memory area to zero.
- `ft_memcpy`: copies bytes between non-overlapping memory areas.
- `ft_memmove`: copies bytes while supporting overlapping memory areas.
- `ft_memchr`: searches for a byte in a memory area.
- `ft_memcmp`: compares two memory areas.

**String manipulation**

- `ft_strlen`: calculates the length of a string.
- `ft_strlcpy`: copies a string with a specified destination size.
- `ft_strlcat`: appends a string with a specified destination size.
- `ft_strchr`: finds the first occurrence of a character.
- `ft_strrchr`: finds the last occurrence of a character.
- `ft_strncmp`: compares strings up to a specified number of characters.
- `ft_strnstr`: searches for a substring within a limited length.

**Character conversion**

- `ft_toupper`: converts a lowercase letter to uppercase.
- `ft_tolower`: converts an uppercase letter to lowercase.

**Conversion and allocation**

- `ft_atoi`: converts a string to an integer.
- `ft_calloc`: allocates zero-initialized memory.
- `ft_strdup`: creates a dynamically allocated copy of a string.

### Part 2 — Additional functions

Functions for string manipulation, conversion, and file-descriptor output.

- `ft_substr`: extracts a substring.
- `ft_strjoin`: concatenates two strings into a new allocation.
- `ft_strtrim`: removes specified characters from the beginning and end of a string.
- `ft_split`: splits a string using a delimiter.
- `ft_itoa`: converts an integer into a string.
- `ft_strmapi`: creates a new string by applying a function to each character.
- `ft_striteri`: applies a function to each character of a string.
- `ft_putchar_fd`: writes a character to a file descriptor.
- `ft_putstr_fd`: writes a string to a file descriptor.
- `ft_putendl_fd`: writes a string followed by a newline.
- `ft_putnbr_fd`: writes an integer to a file descriptor.

### Part 3 — Linked lists

Functions for creating, traversing, modifying, and deleting linked lists using the `t_list` structure defined in `libft.h`.

- `ft_lstnew`: creates a new node.
- `ft_lstadd_front`: adds a node at the beginning of a list.
- `ft_lstsize`: counts the nodes in a list.
- `ft_lstlast`: returns the last node.
- `ft_lstadd_back`: adds a node at the end of a list.
- `ft_lstdelone`: deletes a node using a supplied deletion function.
- `ft_lstclear`: deletes a list and releases its nodes.
- `ft_lstiter`: applies a function to each node's content.
- `ft_lstmap`: creates a new list by applying a function to each node's content.

## Resources

- [Linux manual pages](https://man7.org/linux/man-pages/)
- The Libft subject and materials provided by the 42 curriculum.

## Disclaimer

This repository contains an educational implementation of Libft. Compliance with the applicable subject, Norminette requirements, memory-management requirements, and official evaluation tests must be checked separately.
