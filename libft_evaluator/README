
*This project has been created as part of the 42 curriculum by tisilva.*

# Libft Evaluator

## Description

Libft Evaluator is a personal C testing tool designed to help students test their implementations of the Libft project from the 42 curriculum.

It runs tests against the functions from Part 1, Part 2, and Part 3. It reports individual failures and provides a final summary that lists the functions with errors without repeating a function when multiple tests fail.

The goal is to make debugging easier and help identify which functions require further investigation.

This is an unofficial personal project and is not an evaluator developed or endorsed by 42.

## Instructions

### Requirements

- A C compiler, such as `cc`.
- The `libft.h` header.
- The Libft source files being tested.

### Compilation

If the evaluator and the Libft source files are in the same directory:

```bash
cc -Wall -Wextra -Werror libft_evaluator.c ft_*.c -o libft_evaluator
```

If the evaluator is in a separate directory, adjust the source paths and header include path accordingly.

Do not include another source file containing a `main` function in the compilation command.

### Execution

Run all test sections:

```bash
./libft_evaluator
```

Run a specific part:

```bash
./libft_evaluator 1
./libft_evaluator 2
./libft_evaluator 3
```

Choose the output language:

```bash
./libft_evaluator all pt
./libft_evaluator all en
```

### Available arguments

| Argument | Description |
|---|---|
| `1` | Run Part 1 tests |
| `2` | Run Part 2 tests |
| `3` | Run Part 3 tests |
| `all` | Run all parts |
| `todos` | Alternative to `all` |
| `pt` | Portuguese output |
| `en` | English output |
| `utf8` | Enable UTF-8 output |
| `nocolor` | Disable colored output |
| `no-color` | Alternative to `nocolor` |
| `help` | Display usage information |
| `h` | Alternative to `help` |

Arguments can be combined:

```bash
./libft_evaluator 1 pt utf8 nocolor
```

When no language is specified and the evaluator is run interactively in a terminal, it asks the user to choose a language.

## Test Coverage

### Part 1 — Libc functions

- `ft_isalpha`
- `ft_isdigit`
- `ft_isalnum`
- `ft_isascii`
- `ft_isprint`
- `ft_memset`
- `ft_bzero`
- `ft_memcpy`
- `ft_memmove`
- `ft_memchr`
- `ft_memcmp`
- `ft_strlen`
- `ft_strlcpy`
- `ft_strlcat`
- `ft_toupper`
- `ft_tolower`
- `ft_strchr`
- `ft_strrchr`
- `ft_strncmp`
- `ft_strnstr`
- `ft_atoi`
- `ft_calloc`
- `ft_strdup`

### Part 2 — Additional functions

- `ft_substr`
- `ft_strjoin`
- `ft_strtrim`
- `ft_split`
- `ft_itoa`
- `ft_strmapi`
- `ft_striteri`
- `ft_putchar_fd`
- `ft_putstr_fd`
- `ft_putendl_fd`
- `ft_putnbr_fd`

### Part 3 — Linked lists

- `ft_lstnew`
- `ft_lstadd_front`
- `ft_lstsize`
- `ft_lstlast`
- `ft_lstadd_back`
- `ft_lstdelone`
- `ft_lstclear`
- `ft_lstiter`
- `ft_lstmap`

## Understanding the Results

The evaluator displays individual test failures and a final summary with the number of tests executed, passed, and failed.

The final summary also lists the functions that encountered errors. Each function appears only once, even if several tests for that function fail.

Use the individual failure messages to identify the cases that need further investigation.

Passing all tests means that the implementation passed the tests included in this evaluator. It does not guarantee that the project satisfies every requirement of the official Libft subject.

## Resources

- [Linux manual pages](https://man7.org/linux/man-pages/)
- [C reference — cppreference](https://en.cppreference.com/w/c)
- [GCC documentation](https://gcc.gnu.org/onlinedocs/)
- The Libft subject and materials provided by the 42 curriculum.

### AI usage

AI tools were used exclusively to translate the project's documentation.
