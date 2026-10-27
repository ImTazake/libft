#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include "libft.h"

#include <stdarg.h>

#define GREEN "\033[32m"
#define RED   "\033[31m"
#define BOLD  "\033[1m"
#define RESET "\033[0m"

#define BUF   32     
#define GUARD 0xAA   

static int	g_total = 0;
static int	g_ok = 0;
static int	g_fail = 0;
static const char	*g_failed_funcs[128];
static int	g_failed_func_count = 0;

static int	g_en = 0;     
static int	g_utf8 = 0;   
static int	g_color = 1;  

static const char	*tr(const char *pt);

static const char	*g_ascii_map[4] = {
	"AAAAAAACEEEEIIII", "DNOOOOO*OUUUUYTs",
	"aaaaaaaceeeeiiii", "dnooooo/ouuuuyty"
};

static void	out_write(const char *s, size_t n)
{
	size_t			i;
	size_t			j;
	unsigned char	c;
	unsigned char	d;

	i = 0;
	while (i < n)
	{
		c = (unsigned char)s[i];
		if (c == 0x1B && i + 1 < n && s[i + 1] == '[')
		{
			j = i + 2;
			while (j < n && s[j] != 'm')
				j++;
			if (j < n && g_color)
				fwrite(s + i, 1, j - i + 1, stdout);
			i = j + 1;
		}
		else if (c < 0x80 || g_utf8)
		{
			putchar(c);
			i++;
		}
		else if ((c == 0xC3 || c == 0xC2) && i + 1 < n
			&& ((unsigned char)s[i + 1] & 0xC0) == 0x80)
		{
			d = (unsigned char)s[i + 1];
			if (c == 0xC3)
				putchar(g_ascii_map[(d - 0x80) / 16][(d - 0x80) % 16]);
			else if (d == 0xBA || d == 0xB0)
				putchar('o');
			else if (d == 0xAA)
				putchar('a');
			else
				putchar(' ');
			i += 2;
		}
		else
		{
			putchar('?');
			i++;
			while (i < n && ((unsigned char)s[i] & 0xC0) == 0x80)
				i++;
		}
	}
}

static void	out_printf(const char *fmt, ...)
{
	va_list	ap;
	va_list	ap2;
	int		len;
	char	*buf;

	va_start(ap, fmt);
	va_copy(ap2, ap);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	if (len >= 0)
	{
		buf = malloc((size_t)len + 1);
		if (buf)
		{
			vsnprintf(buf, (size_t)len + 1, fmt, ap2);
			out_write(buf, (size_t)len);
			free(buf);
		}
	}
	va_end(ap2);
}

static int	report(const char *func, const char *desc, int ok)
{
	g_total++;
	if (ok)
	{
		g_ok++;
		out_printf("[" GREEN "OK" RESET "]   %-16s %s\n", func, desc);
	}
	else
	{
		int i;

		g_fail++;
		i = 0;
		while (i < g_failed_func_count
			&& strcmp(g_failed_funcs[i], func) != 0)
			i++;
		if (i == g_failed_func_count && g_failed_func_count < 128)
			g_failed_funcs[g_failed_func_count++] = func;
		out_printf("[" RED "%s" RESET "] %-16s %s\n",
			g_en ? "FAIL" : "ERRO", func, desc);
	}
	return (ok);
}

static void	section(const char *name)
{
	out_printf("\n" BOLD "=== %s ===" RESET "\n", name);
}


static void	check_long(const char *func, const char *desc,
				long got, long expected)
{
	if (!report(func, desc, got == expected))
		out_printf(tr("         obtido: %ld | esperado: %ld\n"), got, expected);
}

static int	sign(int n)
{
	return ((n > 0) - (n < 0));
}

static void	init_pattern(unsigned char *b, size_t n)
{
	size_t	i;

	for (i = 0; i < n; i++)
		b[i] = (unsigned char)(i * 13 + 1);
}

static int	ref_isalpha(int c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static int	ref_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

static int	ref_isalnum(int c)
{
	return (ref_isalpha(c) || ref_isdigit(c));
}

static int	ref_isascii(int c)
{
	return (c >= 0 && c <= 127);
}

static int	ref_isprint(int c)
{
	return (c >= 32 && c <= 126);
}

static int	range_ok(int (*fn)(int), int (*ref)(int),
				int from, int to, int *bad)
{
	int	c;

	for (c = from; c <= to; c++)
	{
		if ((fn(c) != 0) != (ref(c) != 0))
		{
			*bad = c;
			return (0);
		}
	}
	return (1);
}

static void	test_char_func(const char *name, int (*fn)(int), int (*ref)(int))
{
	int		bad = 0;
	int		outside[3] = {-1, 256, 1000};
	int		outside_ok = 1;
	int		i;

	section(name);
	if (!report(name, tr("todos os valores 0-127 (inclui os limites)"),
			range_ok(fn, ref, 0, 127, &bad)))
		out_printf(tr("         primeiro valor errado: %d\n"), bad);
	if (!report(name, tr("todos os valores 128-255"),
			range_ok(fn, ref, 128, 255, &bad)))
		out_printf(tr("         primeiro valor errado: %d\n"), bad);
	for (i = 0; i < 3; i++)
	{
		if ((fn(outside[i]) != 0) != (ref(outside[i]) != 0))
		{
			outside_ok = 0;
			bad = outside[i];
		}
	}
	if (!report(name, tr("valores fora do intervalo (-1, 256, 1000)"), outside_ok))
		out_printf(tr("         valor errado: %d\n"), bad);
}

static void	test_memset_case(const char *desc, size_t off, int c, size_t n)
{
	unsigned char	a[BUF];
	unsigned char	b[BUF];
	void			*ret;
	int				same;
	int				ret_ok;

	memset(a, GUARD, BUF);
	memset(b, GUARD, BUF);
	ret = ft_memset(a + off, c, n);
	memset(b + off, c, n);
	same = (memcmp(a, b, BUF) == 0);
	ret_ok = (ret == (void *)(a + off));
	if (!report("ft_memset", desc, same && ret_ok))
	{
		if (!same)
			out_printf(tr("         bytes diferentes do esperado "
				"(ou bytes fora da zona alterados)\n"));
		if (!ret_ok)
			out_printf(tr("         o endereço devolvido não é o de dest\n"));
	}
}

static void	test_memset(void)
{
	section("ft_memset");
	test_memset_case(tr("preencher 10 bytes no início com 'A'"), 0, 'A', 10);
	test_memset_case(tr("preencher no meio (offset 5, 8 bytes) com 0"), 5, 0, 8);
	test_memset_case(tr("n = 0 não altera nada"), 5, 'X', 0);
	test_memset_case(tr("preencher o buffer inteiro"), 0, 'Q', BUF);
	test_memset_case(tr("preencher só o último byte"), BUF - 1, 'Z', 1);
	test_memset_case(tr("valor 321 convertido para unsigned char"), 3, 321, 10);
	test_memset_case(tr("valor -1 convertido para 0xFF"), 3, -1, 10);
	test_memset_case(tr("valor 255"), 0, 255, 8);
}

static void	test_bzero_case(const char *desc, size_t off, size_t n)
{
	unsigned char	a[BUF];
	unsigned char	b[BUF];

	memset(a, GUARD, BUF);
	memset(b, GUARD, BUF);
	ft_bzero(a + off, n);
	memset(b + off, 0, n);
	if (!report("ft_bzero", desc, memcmp(a, b, BUF) == 0))
		out_printf(tr("         bytes diferentes do esperado "
			"(ou bytes fora da zona alterados)\n"));
}

static void	test_bzero(void)
{
	section("ft_bzero");
	test_bzero_case(tr("zerar 10 bytes no início"), 0, 10);
	test_bzero_case(tr("zerar no meio (offset 4, 10 bytes)"), 4, 10);
	test_bzero_case(tr("n = 0 não altera nada"), 6, 0);
	test_bzero_case(tr("zerar o buffer inteiro"), 0, BUF);
	test_bzero_case(tr("zerar só o primeiro byte"), 0, 1);
	test_bzero_case(tr("zerar só o último byte"), BUF - 1, 1);
}

static void	test_memcpy_case(const char *desc, size_t off, size_t n)
{
	unsigned char	src[BUF];
	unsigned char	src_copy[BUF];
	unsigned char	a[BUF];
	unsigned char	b[BUF];
	void			*ret;
	int				same;
	int				ret_ok;
	int				src_ok;

	init_pattern(src, BUF);
	src[5] = 0;
	memcpy(src_copy, src, BUF);
	memset(a, GUARD, BUF);
	memset(b, GUARD, BUF);
	ret = ft_memcpy(a + off, src, n);
	memcpy(b + off, src, n);
	same = (memcmp(a, b, BUF) == 0);
	ret_ok = (ret == (void *)(a + off));
	src_ok = (memcmp(src, src_copy, BUF) == 0);
	if (!report("ft_memcpy", desc, same && ret_ok && src_ok))
	{
		if (!same)
			out_printf(tr("         bytes diferentes do esperado "
				"(ou bytes fora da zona alterados)\n"));
		if (!ret_ok)
			out_printf(tr("         o endereço devolvido não é o de dest\n"));
		if (!src_ok)
			out_printf(tr("         a origem foi alterada\n"));
	}
}

static void	test_memcpy(void)
{
	char	dst[20];
	void	*ret;

	section("ft_memcpy");
	test_memcpy_case(tr("n = 0 não altera nada"), 0, 0);
	test_memcpy_case(tr("copiar 1 byte"), 0, 1);
	test_memcpy_case(tr("copiar 10 bytes (passa por um byte 0)"), 0, 10);
	test_memcpy_case(tr("copiar para o meio do destino (offset 5)"), 5, 10);
	test_memcpy_case(tr("copiar o buffer inteiro"), 0, BUF);
	test_memcpy_case(tr("copiar só para o último byte"), BUF - 1, 1);
	memset(dst, GUARD, sizeof(dst));
	ret = ft_memcpy(dst, "Hello, 42!", 11);
	report("ft_memcpy", tr("copiar uma string com o '\\0' final"),
		strcmp(dst, "Hello, 42!") == 0 && ret == (void *)dst);
}

static void	test_memmove_case(const char *desc, size_t dst_off,
				size_t src_off, size_t n)
{
	unsigned char	a[BUF];
	unsigned char	b[BUF];
	void			*ret;
	int				same;
	int				ret_ok;

	init_pattern(a, BUF);
	init_pattern(b, BUF);
	ret = ft_memmove(a + dst_off, a + src_off, n);
	memmove(b + dst_off, b + src_off, n);
	same = (memcmp(a, b, BUF) == 0);
	ret_ok = (ret == (void *)(a + dst_off));
	if (!report("ft_memmove", desc, same && ret_ok))
	{
		if (!same)
			out_printf(tr("         bytes diferentes do esperado\n"));
		if (!ret_ok)
			out_printf(tr("         o endereço devolvido não é o de dest\n"));
	}
}

static void	test_memmove(void)
{
	section("ft_memmove");
	test_memmove_case(tr("sobreposição: destino depois da origem"), 5, 0, 15);
	test_memmove_case(tr("sobreposição: destino antes da origem"), 0, 5, 15);
	test_memmove_case(tr("sobreposição de 1 byte (dest = src + 1)"), 1, 0, 10);
	test_memmove_case(tr("sobreposição de 1 byte (dest = src - 1)"), 0, 1, 10);
	test_memmove_case(tr("sobreposição máxima (dest = src + 1)"), 1, 0, BUF - 1);
	test_memmove_case(tr("sobreposição máxima (dest = src - 1)"), 0, 1, BUF - 1);
	test_memmove_case(tr("dest igual a src"), 5, 5, 10);
	test_memmove_case(tr("zonas adjacentes sem sobreposição (frente)"), 10, 0, 10);
	test_memmove_case(tr("zonas adjacentes sem sobreposição (trás)"), 0, 10, 10);
	test_memmove_case(tr("zonas disjuntas"), 16, 0, 16);
	test_memmove_case(tr("n = 0 não altera nada"), 3, 8, 0);
	test_memmove_case(tr("mover o buffer inteiro para ele próprio"), 0, 0, BUF);
}

static void	test_memchr_case(const char *desc, const void *s, int c, size_t n)
{
	void	*got;
	void	*expected;

	got = ft_memchr(s, c, n);
	expected = memchr(s, c, n);
	if (!report("ft_memchr", desc, got == expected))
		out_printf(tr("         endereço obtido: %p | esperado: %p\n"),
			got, expected);
}

static void	test_memchr(void)
{
	static const unsigned char	data[10] = {10, 20, 30, 0, 40, 50, 200, 30,
			60, 70};

	section("ft_memchr");
	test_memchr_case(tr("byte existente (devolve a 1ª ocorrência)"), data, 30, 10);
	test_memchr_case(tr("procurar o byte 0 no meio dos dados"), data, 0, 10);
	test_memchr_case(tr("byte 0 fora do limite n"), data, 0, 3);
	test_memchr_case(tr("byte >= 128 (unsigned char)"), data, 200, 10);
	test_memchr_case(tr("c = 456 é convertido para 200"), data, 456, 10);
	test_memchr_case(tr("c = -56 é convertido para 200"), data, -56, 10);
	test_memchr_case(tr("byte inexistente"), data, 99, 10);
	test_memchr_case(tr("último byte incluído em n"), data, 70, 10);
	test_memchr_case(tr("último byte fora de n"), data, 70, 9);
	test_memchr_case(tr("byte logo depois do limite n"), data, 60, 8);
	test_memchr_case(tr("byte exatamente no limite n"), data, 60, 9);
	test_memchr_case(tr("n = 0 devolve NULL"), data, 10, 0);
	test_memchr_case(tr("n = 1 com o 1º byte"), data, 10, 1);
	test_memchr_case(tr("procurar numa string"), "Hello, 42!", 'l', 10);
}

static void	test_memcmp_case(const char *desc, const void *a,
				const void *b, size_t n)
{
	int	got;
	int	expected;

	got = ft_memcmp(a, b, n);
	expected = memcmp(a, b, n);
	if (!report("ft_memcmp", desc, sign(got) == sign(expected)))
		out_printf(tr("         sinal obtido: %d | esperado: %d\n"),
			sign(got), sign(expected));
}

static void	test_memcmp(void)
{
	static const unsigned char	a1[5] = {1, 2, 3, 4, 5};
	static const unsigned char	a2[5] = {1, 2, 3, 4, 5};
	static const unsigned char	a3[5] = {1, 2, 9, 4, 5};
	static const unsigned char	a4[5] = {1, 2, 3, 4, 6};
	static const unsigned char	z1[3] = {1, 0, 3};
	static const unsigned char	z2[3] = {1, 0, 4};
	static const unsigned char	z3[3] = {1, 0, 3};
	static const unsigned char	h1[1] = {0x80};
	static const unsigned char	h2[1] = {0x7F};
	static const unsigned char	h3[1] = {0xFF};
	static const unsigned char	h4[1] = {0x01};

	section("ft_memcmp");
	test_memcmp_case(tr("blocos iguais"), a1, a2, 5);
	test_memcmp_case(tr("a < b (diferem no meio)"), a1, a3, 5);
	test_memcmp_case(tr("a > b (diferem no meio)"), a3, a1, 5);
	test_memcmp_case(tr("diferença depois do limite n devolve 0"), a1, a3, 2);
	test_memcmp_case(tr("diferença no último byte"), a1, a4, 5);
	test_memcmp_case(tr("diferença no último byte, mas fora de n"), a1, a4, 4);
	test_memcmp_case(tr("n = 0 devolve 0"), a1, a3, 0);
	test_memcmp_case(tr("não pára no byte 0 (a < b)"), z1, z2, 3);
	test_memcmp_case(tr("não pára no byte 0 (iguais)"), z1, z3, 3);
	test_memcmp_case(tr("comparação sem sinal: 0x80 vs 0x7F"), h1, h2, 1);
	test_memcmp_case(tr("comparação sem sinal: 0x7F vs 0x80"), h2, h1, 1);
	test_memcmp_case(tr("comparação sem sinal: 0xFF vs 0x01"), h3, h4, 1);
	test_memcmp_case(tr("comparação sem sinal: 0x01 vs 0xFF"), h4, h3, 1);
	test_memcmp_case(tr("strings iguais"), "abc", "abc", 4);
	test_memcmp_case(tr("strings diferentes"), "abc", "abd", 3);
}

static void	test_strlen_case(const char *desc, const char *s)
{
	check_long("ft_strlen", desc, (long)ft_strlen(s), (long)strlen(s));
}

static void	test_strlen(void)
{
	char	big[1001];

	memset(big, 'x', 1000);
	big[1000] = '\0';
	section("ft_strlen");
	test_strlen_case(tr("string vazia"), "");
	test_strlen_case(tr("1 caractere"), "a");
	test_strlen_case(tr("string normal"), "Hello, 42 Lisboa!");
	test_strlen_case(tr("string com espaços e \\t \\n"), "a b\tc\nd");
	test_strlen_case(tr("pára no primeiro '\\0'"), "ab\0cd");
	test_strlen_case(tr("bytes >= 128 (UTF-8)"), "caf\xc3\xa9");
	test_strlen_case(tr("string longa (1000 caracteres)"), big);
}

static void	test_strlcpy_case(const char *desc, const char *src, size_t size,
				size_t exp_ret, const char *exp_buf)
{
	char	buf[BUF];
	char	expected[BUF];
	size_t	ret;
	int		ret_ok;
	int		buf_ok;

	memset(buf, 'Z', BUF);
	memset(expected, 'Z', BUF);
	if (exp_buf)
		memcpy(expected, exp_buf, strlen(exp_buf) + 1);
	ret = ft_strlcpy(buf, src, size);
	ret_ok = (ret == exp_ret);
	buf_ok = (memcmp(buf, expected, BUF) == 0);
	if (!report("ft_strlcpy", desc, ret_ok && buf_ok))
	{
		if (!ret_ok)
			out_printf(tr("         retorno obtido: %zu | esperado: %zu\n"),
				ret, exp_ret);
		if (!buf_ok)
			out_printf(tr("         conteúdo do buffer diferente do esperado "
				"(esperado: \"%s\")\n"), exp_buf ? exp_buf : "<intacto>");
	}
}

static void	test_strlcpy(void)
{
	section("ft_strlcpy");
	test_strlcpy_case(tr("buffer grande"), "Hello", 32, 5, "Hello");
	test_strlcpy_case(tr("tamanho = strlen + 1 (cabe justo)"), "Hello", 6, 5,
		"Hello");
	test_strlcpy_case(tr("tamanho = strlen (trunca 1 char)"), "Hello", 5, 5,
		"Hell");
	test_strlcpy_case(tr("buffer pequeno (size 3)"), "Hello", 3, 5, "He");
	test_strlcpy_case(tr("size 1 só escreve '\\0'"), "Hello", 1, 5, "");
	test_strlcpy_case(tr("size 0 não escreve nada"), "Hello", 0, 5, NULL);
	test_strlcpy_case(tr("src vazia"), "", 10, 0, "");
	test_strlcpy_case(tr("src vazia com size 0"), "", 0, 0, NULL);
	test_strlcpy_case(tr("src muito maior que o buffer"),
		"0123456789abcdefghij", 4, 20, "012");
	test_strlcpy_case(tr("pára no '\\0' da src"), "ab\0cd", 10, 2, "ab");
}

static void	test_strlcat_case(const char *desc, const char *dst_init,
				const char *src, size_t size, size_t exp_ret,
				const char *exp_buf)
{
	char	buf[BUF];
	char	expected[BUF];
	size_t	ret;
	int		ret_ok;
	int		buf_ok;

	memset(buf, 'Z', BUF);
	memcpy(buf, dst_init, strlen(dst_init) + 1);
	memset(expected, 'Z', BUF);
	memcpy(expected, exp_buf, strlen(exp_buf) + 1);
	ret = ft_strlcat(buf, src, size);
	ret_ok = (ret == exp_ret);
	buf_ok = (memcmp(buf, expected, BUF) == 0);
	if (!report("ft_strlcat", desc, ret_ok && buf_ok))
	{
		if (!ret_ok)
			out_printf(tr("         retorno obtido: %zu | esperado: %zu\n"),
				ret, exp_ret);
		if (!buf_ok)
			out_printf(tr("         conteúdo do buffer diferente do esperado "
				"(esperado: \"%s\")\n"), exp_buf);
	}
}

static void	test_strlcat(void)
{
	section("ft_strlcat");
	test_strlcat_case(tr("buffer grande"), "Hello", " World", 32, 11,
		"Hello World");
	test_strlcat_case(tr("cabe justo (size 12)"), "Hello", " World", 12, 11,
		"Hello World");
	test_strlcat_case(tr("trunca 1 char (size 11)"), "Hello", " World", 11, 11,
		"Hello Worl");
	test_strlcat_case(tr("buffer pequeno (size 8)"), "Hello", " World", 8, 11,
		"Hello W");
	test_strlcat_case(tr("sem espaço para copiar (size = dlen + 1)"), "Hello",
		" World", 6, 11, "Hello");
	test_strlcat_case(tr("size = strlen(dst) não altera"), "Hello", " World", 5,
		11, "Hello");
	test_strlcat_case(tr("size < strlen(dst) não altera"), "Hello", " World", 3,
		9, "Hello");
	test_strlcat_case(tr("size 0 não altera"), "Hello", " World", 0, 6, "Hello");
	test_strlcat_case(tr("dst vazio"), "", "abc", 10, 3, "abc");
	test_strlcat_case(tr("dst vazio com size 1"), "", "abc", 1, 3, "");
	test_strlcat_case(tr("dst vazio com size 0"), "", "abc", 0, 3, "");
	test_strlcat_case(tr("src vazia"), "Hello", "", 10, 5, "Hello");
	test_strlcat_case(tr("dst e src vazios"), "", "", 1, 0, "");
}

static void	test_strchr_case(const char *desc, const char *s, int c)
{
	char	*got;
	char	*expected;

	got = ft_strchr(s, c);
	expected = strchr(s, c);
	if (!report("ft_strchr", desc, got == expected))
		out_printf(tr("         endereço obtido: %p | esperado: %p\n"),
			(void *)got, (void *)expected);
}

static void	test_strchr(void)
{
	section("ft_strchr");
	test_strchr_case(tr("1ª ocorrência de 'o'"), "Hello, World", 'o');
	test_strchr_case(tr("1º caracter da string"), "Hello, World", 'H');
	test_strchr_case(tr("último caracter da string"), "Hello, World", 'd');
	test_strchr_case(tr("caracter inexistente devolve NULL"), "Hello, World", 'z');
	test_strchr_case(tr("diferencia maiúsculas de minúsculas"), "Hello", 'h');
	test_strchr_case(tr("procurar '\\0' devolve o fim da string"), "Hello", '\0');
	test_strchr_case(tr("procurar '\\0' numa string vazia"), "", '\0');
	test_strchr_case(tr("procurar 'a' numa string vazia"), "", 'a');
	test_strchr_case(tr("caracter repetido (devolve o 1º)"), "aaa", 'a');
	test_strchr_case(tr("c = 'o' + 256 é convertido para char"), "Hello", 'o' + 256);
	test_strchr_case(tr("caracter >= 128"), "caf\xc3\xa9", 0xC3);
}

static void	test_strrchr_case(const char *desc, const char *s, int c)
{
	char	*got;
	char	*expected;

	got = ft_strrchr(s, c);
	expected = strrchr(s, c);
	if (!report("ft_strrchr", desc, got == expected))
		out_printf(tr("         endereço obtido: %p | esperado: %p\n"),
			(void *)got, (void *)expected);
}

static void	test_strrchr(void)
{
	section("ft_strrchr");
	test_strrchr_case(tr("última ocorrência de 'o'"), "Hello, World", 'o');
	test_strrchr_case(tr("última ocorrência de 'l'"), "Hello", 'l');
	test_strrchr_case(tr("ocorrência única no início"), "Hello", 'H');
	test_strrchr_case(tr("último caracter da string"), "Hello, World", 'd');
	test_strrchr_case(tr("caracter inexistente devolve NULL"), "Hello, World", 'z');
	test_strrchr_case(tr("diferencia maiúsculas de minúsculas"), "Hello", 'h');
	test_strrchr_case(tr("procurar '\\0' devolve o fim da string"), "Hello", '\0');
	test_strrchr_case(tr("procurar '\\0' numa string vazia"), "", '\0');
	test_strrchr_case(tr("procurar 'a' numa string vazia"), "", 'a');
	test_strrchr_case(tr("caracter repetido (devolve o último)"), "aXbXc", 'X');
	test_strrchr_case(tr("c = 'o' + 256 é convertido para char"), "Hello",
		'o' + 256);
	test_strrchr_case(tr("caracter >= 128"), "caf\xc3\xa9", 0xC3);
}

static void	test_strncmp_case(const char *desc, const char *s1,
				const char *s2, size_t n)
{
	int	got;
	int	expected;

	got = ft_strncmp(s1, s2, n);
	expected = strncmp(s1, s2, n);
	if (!report("ft_strncmp", desc, sign(got) == sign(expected)))
		out_printf(tr("         sinal obtido: %d | esperado: %d\n"),
			sign(got), sign(expected));
}

static void	test_strncmp(void)
{
	section("ft_strncmp");
	test_strncmp_case(tr("strings iguais"), "abc", "abc", 3);
	test_strncmp_case("s1 < s2", "abc", "abd", 3);
	test_strncmp_case("s1 > s2", "abd", "abc", 3);
	test_strncmp_case(tr("diferença depois do limite n devolve 0"), "abc", "abd",
		2);
	test_strncmp_case(tr("n = 0 devolve 0"), "abc", "xyz", 0);
	test_strncmp_case(tr("n maior que o tamanho (iguais)"), "abc", "abc", 100);
	test_strncmp_case(tr("s1 é prefixo de s2 (n = 3)"), "abc", "abcd", 3);
	test_strncmp_case(tr("s1 é prefixo de s2 (n = 4)"), "abc", "abcd", 4);
	test_strncmp_case(tr("s2 é prefixo de s1 (n = 4)"), "abcd", "abc", 4);
	test_strncmp_case(tr("duas strings vazias"), "", "", 5);
	test_strncmp_case(tr("s1 vazia"), "", "a", 1);
	test_strncmp_case(tr("s2 vazia"), "a", "", 1);
	test_strncmp_case(tr("pára no '\\0' (ignora o que vem depois)"),
		"ab\0cd", "ab\0ce", 5);
	test_strncmp_case(tr("comparação sem sinal (0x80 vs 0x7F)"), "\200", "\177", 1);
	test_strncmp_case(tr("comparação sem sinal (0x7F vs 0x80)"), "\177", "\200", 1);
	test_strncmp_case(tr("char >= 128 vs '\\0'"), "test\200", "test", 6);
}

static void	test_strnstr_case(const char *desc, const char *big,
				const char *little, size_t len, int exp_off)
{
	char	*got;
	char	*expected;

	got = ft_strnstr(big, little, len);
	expected = (exp_off < 0) ? NULL : (char *)big + exp_off;
	if (!report("ft_strnstr", desc, got == expected))
	{
		if (got == NULL)
			out_printf(tr("         obtido: NULL | esperado: posição %d\n"), exp_off);
		else if (expected == NULL)
			out_printf(tr("         obtido: posição %ld | esperado: NULL\n"),
				(long)(got - big));
		else
			out_printf(tr("         obtido: posição %ld | esperado: posição %d\n"),
				(long)(got - big), exp_off);
	}
}

static void	test_strnstr(void)
{
	section("ft_strnstr");
	test_strnstr_case(tr("encontra no fim (len = tamanho)"), "Hello World",
		"World", 11, 6);
	test_strnstr_case(tr("len 1 byte curto: não encontra"), "Hello World",
		"World", 10, -1);
	test_strnstr_case(tr("encontra no início"), "Hello World", "Hello", 5, 0);
	test_strnstr_case(tr("len curto no início: não encontra"), "Hello World",
		"Hello", 4, -1);
	test_strnstr_case(tr("1ª ocorrência de 1 caracter"), "Hello World", "o", 11,
		4);
	test_strnstr_case(tr("1 caracter fora do limite len"), "Hello World", "o", 4,
		-1);
	test_strnstr_case(tr("1 caracter exatamente no limite"), "Hello World", "o", 5,
		4);
	test_strnstr_case(tr("substring inexistente"), "Hello World", "xyz", 11, -1);
	test_strnstr_case(tr("needle vazia devolve big"), "Hello World", "", 11, 0);
	test_strnstr_case(tr("needle vazia com len 0 devolve big"), "Hello World", "",
		0, 0);
	test_strnstr_case(tr("len 0 com needle não vazia"), "Hello World", "World",
		0, -1);
	test_strnstr_case(tr("big vazia, needle não vazia"), "", "a", 5, -1);
	test_strnstr_case(tr("big e needle vazias (len 0)"), "", "", 0, 0);
	test_strnstr_case(tr("big e needle vazias (len 5)"), "", "", 5, 0);
	test_strnstr_case(tr("needle maior que big"), "abc", "abcd", 10, -1);
	test_strnstr_case(tr("needle igual a big"), "abc", "abc", 3, 0);
	test_strnstr_case(tr("needle igual a big, len curto"), "abc", "abc", 2, -1);
	test_strnstr_case(tr("len maior que a string (pára no '\\0')"), "abc", "c",
		100, 2);
	test_strnstr_case(tr("len maior que a string, needle inexistente"), "abc",
		"cd", 100, -1);
	test_strnstr_case(tr("fim da string ('lo')"), "Hello", "lo", 5, 3);
	test_strnstr_case(tr("fim da string, len curto"), "Hello", "lo", 4, -1);
	test_strnstr_case(tr("início de match falso ('aab')"), "aaabc", "aab", 5, 1);
	test_strnstr_case(tr("match falso antes do verdadeiro"), "abcabcd", "abcd", 7,
		3);
	test_strnstr_case(tr("mississippi / issip"), "mississippi", "issip", 11, 4);
	test_strnstr_case(tr("mississippi / issipi"), "mississippi", "issipi", 11, -1);
	test_strnstr_case(tr("match no limite exato ('bc', len 3)"), "abc", "bc", 3, 1);
	test_strnstr_case(tr("match 1 byte fora do limite ('bc', len 2)"), "abc", "bc",
		2, -1);
}

static void	test_atoi_case(const char *desc, const char *s)
{
	check_long("ft_atoi", desc, (long)ft_atoi(s), (long)atoi(s));
}

static void	test_atoi(void)
{
	section("ft_atoi");
	test_atoi_case(tr("zero: \"0\""), "0");
	test_atoi_case(tr("positivo: \"42\""), "42");
	test_atoi_case(tr("negativo: \"-42\""), "-42");
	test_atoi_case(tr("sinal +: \"+42\""), "+42");
	test_atoi_case(tr("zeros à esquerda: \"0042\""), "0042");
	test_atoi_case(tr("menos zero: \"-0\""), "-0");
	test_atoi_case(tr("espaços iniciais: \"   42\""), "   42");
	test_atoi_case(tr("todos os espaços (\\t\\n\\v\\f\\r e ' ') antes do número"),
		"\t\n\v\f\r 42");
	test_atoi_case(tr("espaços antes do sinal: \"  -42\""), "  -42");
	test_atoi_case(tr("espaços e + : \" \\t+13xyz\""), " \t+13xyz");
	test_atoi_case(tr("lixo depois do número: \"42abc\""), "42abc");
	test_atoi_case(tr("lixo depois do negativo: \"  -42abc\""), "  -42abc");
	test_atoi_case(tr("espaço no meio: \"12 34\""), "12 34");
	test_atoi_case(tr("só letras: \"abc\""), "abc");
	test_atoi_case(tr("letra antes do número: \"a42\""), "a42");
	test_atoi_case(tr("string vazia"), "");
	test_atoi_case(tr("só espaços"), "  ");
	test_atoi_case(tr("só o sinal '-'"), "-");
	test_atoi_case(tr("só o sinal '+'"), "+");
	test_atoi_case(tr("dois sinais: \"+-5\""), "+-5");
	test_atoi_case(tr("dois sinais: \"-+5\""), "-+5");
	test_atoi_case(tr("dois sinais: \"--5\""), "--5");
	test_atoi_case(tr("dois sinais: \"++5\""), "++5");
	test_atoi_case(tr("espaço depois do sinal: \"- 5\""), "- 5");
	test_atoi_case(tr("espaço depois do +: \"+ 5\""), "+ 5");
	test_atoi_case(tr("INT_MAX: \"2147483647\""), "2147483647");
	test_atoi_case(tr("INT_MIN: \"-2147483648\""), "-2147483648");
	test_atoi_case(tr("INT_MAX com espaços"), "  2147483647");
}

static void	dirty_heap(size_t n)
{
	unsigned char	*p;

	p = malloc(n);
	if (p)
	{
		memset(p, 0xFF, n);
		free(p);
	}
}

static int	all_zero(const unsigned char *p, size_t n)
{
	size_t	i;

	for (i = 0; i < n; i++)
	{
		if (p[i] != 0)
			return (0);
	}
	return (1);
}

static void	test_calloc_case(const char *desc, size_t count, size_t size)
{
	unsigned char	*p;
	size_t			total;
	int				ok;

	total = count * size;
	dirty_heap(total);
	p = ft_calloc(count, size);
	if (p == NULL)
	{
		report("ft_calloc", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	ok = all_zero(p, total);
	memset(p, 0xFF, total);
	free(p);
	report("ft_calloc", desc, ok);
}

static void	test_calloc_zero_case(const char *desc, size_t count, size_t size)
{
	void	*mine;
	void	*ref;
	int		ok;

	mine = ft_calloc(count, size);
	ref = calloc(count, size);
	ok = ((mine == NULL) == (ref == NULL));
	if (!report("ft_calloc", desc, ok))
		out_printf(tr("         ft_calloc devolveu %s, a calloc da libc devolveu %s\n"),
			mine ? "ponteiro" : "NULL", ref ? "ponteiro" : "NULL");
	free(mine);
	free(ref);
}

static void	test_calloc_overflow(void)
{
	size_t	count;
	void	*p;

	count = SIZE_MAX / 2 + 1;
	p = ft_calloc(count, 2);
	if (!report("ft_calloc",
			tr("overflow de count * size devolve NULL (recomendado)"), p == NULL))
		out_printf(tr("         count * size dá a volta e a memória devolvida "
			"é demasiado pequena\n"));
	free(p);
}

static void	test_calloc(void)
{
	section("ft_calloc");
	test_calloc_case(tr("5 ints a zero"), 5, sizeof(int));
	test_calloc_case(tr("1 byte a zero"), 1, 1);
	test_calloc_case(tr("100 chars a zero"), 100, sizeof(char));
	test_calloc_case(tr("1000 ints a zero"), 1000, sizeof(int));
	test_calloc_case(tr("bloco grande (4096 bytes) a zero"), 1, 4096);
	test_calloc_case(tr("10 elementos de 16 bytes a zero"), 10, 16);
	test_calloc_zero_case(tr("count = 0 e size = 0"), 0, 0);
	test_calloc_zero_case(tr("count = 0 e size = 4"), 0, 4);
	test_calloc_zero_case(tr("count = 4 e size = 0"), 4, 0);
	test_calloc_overflow();
}

static void	test_strdup_case(const char *desc, const char *text)
{
	char	src[1100];
	char	*dup;
	char	label[160];

	strcpy(src, text);
	dup = ft_strdup(src);
	if (dup == NULL)
	{
		report("ft_strdup", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	snprintf(label, sizeof(label), tr("%s - conteúdo e '\\0' final"), desc);
	report("ft_strdup", label,
		strcmp(dup, text) == 0 && strlen(dup) == strlen(text));
	snprintf(label, sizeof(label), tr("%s - memória diferente da original"), desc);
	report("ft_strdup", label, dup != src);
	dup[0] = '#';
	snprintf(label, sizeof(label), tr("%s - alterar a cópia não altera o original"),
		desc);
	report("ft_strdup", label, strcmp(src, text) == 0);
	free(dup);
}

static void	test_strdup(void)
{
	char	big[1001];

	memset(big, 'a', 1000);
	big[500] = 'b';
	big[1000] = '\0';
	section("ft_strdup");
	test_strdup_case(tr("string normal"), "Hello, World!");
	test_strdup_case(tr("string vazia"), "");
	test_strdup_case(tr("1 caracter"), "a");
	test_strdup_case(tr("espaços e caracteres especiais"), "42 Lisboa\t\n!");
	test_strdup_case(tr("string longa (1000 caracteres)"), big);
}

static int	inside_str(const void *p, const char *s)
{
	uintptr_t	a;
	uintptr_t	b;

	a = (uintptr_t)p;
	b = (uintptr_t)s;
	return (a >= b && a <= b + strlen(s));
}


static int	ref_toupper(int c)
{
	if (c >= 'a' && c <= 'z')
		return (c - 32);
	return (c);
}

static int	ref_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 32);
	return (c);
}


static int	range_eq(int (*fn)(int), int (*ref)(int),
				int from, int to, int *bad)
{
	int	c;

	for (c = from; c <= to; c++)
	{
		if (fn(c) != ref(c))
		{
			*bad = c;
			return (0);
		}
	}
	return (1);
}

static void	test_case_func(const char *name, int (*fn)(int), int (*ref)(int),
				int first, int last)
{
	int	bad;
	int	edges[4];
	int	outside[3] = {-1, 256, 1000};
	int	edges_ok;
	int	outside_ok;
	int	i;

	bad = 0;
	edges[0] = first - 1;
	edges[1] = first;
	edges[2] = last;
	edges[3] = last + 1;
	edges_ok = 1;
	outside_ok = 1;
	section(name);
	if (!report(name, tr("todos os valores 0-127 (inclui os limites)"),
			range_eq(fn, ref, 0, 127, &bad)))
		out_printf(tr("         valor %d: obtido %d | esperado %d\n"),
			bad, fn(bad), ref(bad));
	if (!report(name, tr("todos os valores 128-255 (ficam iguais)"),
			range_eq(fn, ref, 128, 255, &bad)))
		out_printf(tr("         valor %d: obtido %d | esperado %d\n"),
			bad, fn(bad), ref(bad));
	for (i = 0; i < 4; i++)
	{
		if (fn(edges[i]) != ref(edges[i]))
		{
			edges_ok = 0;
			bad = edges[i];
		}
	}
	if (!report(name, tr("limites das letras (um antes, a primeira, a última, "
				"um depois)"), edges_ok))
		out_printf(tr("         valor %d: obtido %d | esperado %d\n"),
			bad, fn(bad), ref(bad));
	for (i = 0; i < 3; i++)
	{
		if (fn(outside[i]) != ref(outside[i]))
		{
			outside_ok = 0;
			bad = outside[i];
		}
	}
	if (!report(name, tr("valores fora do intervalo (-1, 256, 1000) ficam iguais"),
			outside_ok))
		out_printf(tr("         valor %d: obtido %d | esperado %d\n"),
			bad, fn(bad), ref(bad));
}

static void	test_substr_case(const char *desc, const char *s,
				unsigned int start, size_t len, const char *expected)
{
	char	a[1100];
	char	*r;
	int		ok_content;
	int		ok_mem;
	int		ok_src;

	strcpy(a, s);
	r = ft_substr(a, start, len);
	if (r == NULL)
	{
		report("ft_substr", desc, 0);
		out_printf("%s", tr("         devolveu NULL\n"));
		return ;
	}
	ok_content = (strcmp(r, expected) == 0);
	ok_mem = !inside_str(r, a);
	ok_src = (strcmp(a, s) == 0);
	if (!report("ft_substr", desc, ok_content && ok_mem && ok_src))
	{
		if (!ok_content)
			out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"),
				r, expected);
		if (!ok_mem)
			out_printf("%s", tr("         devolveu um ponteiro para dentro de s "
					"(deve ser memória nova)\n"));
		if (!ok_src)
			out_printf("%s", tr("         a string original foi alterada\n"));
	}
	if (ok_mem)
		free(r);
}

static void	test_substr(void)
{
	section("ft_substr");
	test_substr_case(tr("do início, 5 caracteres"), "hello world", 0, 5,
		"hello");
	test_substr_case(tr("do meio até ao fim"), "hello world", 6, 5, "world");
	test_substr_case(tr("no meio da string"), "hello world", 3, 4, "lo w");
	test_substr_case(tr("1 caracter no fim"), "hello", 4, 1, "o");
	test_substr_case(tr("a string inteira"), "hello", 0, 5, "hello");
	test_substr_case(tr("len maior que o resto (copia só até ao fim)"), "hello",
		2, 100, "llo");
	test_substr_case(tr("len muito maior que a string (1000000)"), "hello", 0,
		1000000, "hello");
	test_substr_case(tr("len = 0 devolve string vazia"), "hello", 2, 0, "");
	test_substr_case(tr("start depois do fim devolve string vazia"), "hello",
		20, 5, "");
	test_substr_case(tr("start = strlen devolve string vazia"), "hello", 5, 3,
		"");
	test_substr_case(tr("start = UINT_MAX devolve string vazia"), "hello",
		UINT_MAX, 5, "");
	test_substr_case(tr("string de entrada vazia"), "", 0, 5, "");
}

static void	test_strjoin_case(const char *desc, const char *s1,
				const char *s2, const char *expected)
{
	char	a[1100];
	char	b[1100];
	char	*r;
	int		ok_content;
	int		ok_mem;
	int		ok_src;

	strcpy(a, s1);
	strcpy(b, s2);
	r = ft_strjoin(a, b);
	if (r == NULL)
	{
		report("ft_strjoin", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	ok_content = (strcmp(r, expected) == 0);
	ok_mem = (!inside_str(r, a) && !inside_str(r, b));
	ok_src = (strcmp(a, s1) == 0 && strcmp(b, s2) == 0);
	if (!report("ft_strjoin", desc, ok_content && ok_mem && ok_src))
	{
		if (!ok_content)
			out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"),
				r, expected);
		if (!ok_mem)
			out_printf(tr("         devolveu um ponteiro para dentro de s1 ou s2\n"));
		if (!ok_src)
			out_printf(tr("         s1 ou s2 foram alteradas\n"));
	}
	if (ok_mem)
		free(r);
}

static void	test_strjoin(void)
{
	char	big1[501];
	char	big2[501];
	char	bigexp[1001];

	memset(big1, 'a', 500);
	big1[500] = '\0';
	memset(big2, 'b', 500);
	big2[500] = '\0';
	memset(bigexp, 'a', 500);
	memset(bigexp + 500, 'b', 500);
	bigexp[1000] = '\0';
	section("ft_strjoin");
	test_strjoin_case(tr("duas strings normais"), "Hello, ", "World!",
		"Hello, World!");
	test_strjoin_case(tr("sem espaços"), "abc", "def", "abcdef");
	test_strjoin_case(tr("s1 vazia"), "", "abc", "abc");
	test_strjoin_case(tr("s2 vazia"), "abc", "", "abc");
	test_strjoin_case(tr("ambas vazias"), "", "", "");
	test_strjoin_case(tr("1 caracter + 1 caracter"), "a", "b", "ab");
	test_strjoin_case(tr("com espaços e \\t \\n"), "a b\t", "\nc d", "a b\t\nc d");
	test_strjoin_case(tr("strings longas (500 + 500)"), big1, big2, bigexp);
}

static void	test_strtrim_case(const char *desc, const char *s1,
				const char *set, const char *expected)
{
	char	a[1100];
	char	*r;
	int		ok_content;
	int		ok_mem;
	int		ok_src;

	strcpy(a, s1);
	r = ft_strtrim(a, set);
	if (r == NULL)
	{
		report("ft_strtrim", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	ok_content = (strcmp(r, expected) == 0);
	ok_mem = !inside_str(r, a);
	ok_src = (strcmp(a, s1) == 0);
	if (!report("ft_strtrim", desc, ok_content && ok_mem && ok_src))
	{
		if (!ok_content)
			out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"),
				r, expected);
		if (!ok_mem)
			out_printf(tr("         devolveu um ponteiro para dentro de s1 "
				"(deve ser memória nova)\n"));
		if (!ok_src)
			out_printf(tr("         s1 foi alterada\n"));
	}
	if (ok_mem)
		free(r);
}

static void	test_strtrim(void)
{
	section("ft_strtrim");
	test_strtrim_case(tr("espaços dos dois lados"), "  hello  ", " ", "hello");
	test_strtrim_case(tr("só no início"), "   hello", " ", "hello");
	test_strtrim_case(tr("só no fim"), "hello   ", " ", "hello");
	test_strtrim_case(tr("1 caracter repetido"), "xxhixx", "x", "hi");
	test_strtrim_case(tr("vários caracteres no set"), "abcHELLOcba", "abc",
		"HELLO");
	test_strtrim_case(tr("a ordem do set não importa"), "abcHIcba", "bca", "HI");
	test_strtrim_case(tr("não corta o interior da string"), "  a b  ", " ", "a b");
	test_strtrim_case(tr("whitespace variado"), "\n\t hi \t\n", " \n\t", "hi");
	test_strtrim_case(tr("nada para cortar"), "hello", "xyz", "hello");
	test_strtrim_case(tr("set vazio"), "hello", "", "hello");
	test_strtrim_case(tr("s1 vazia"), "", "abc", "");
	test_strtrim_case(tr("s1 só com caracteres do set"), "xxxx", "x", "");
	test_strtrim_case(tr("s1 igual ao set"), "hello", "hello", "");
	test_strtrim_case(tr("s1 e set vazios"), "", "", "");
	test_strtrim_case(tr("corta 't' e 'e' no fim"), "lorem ipsum dolor sit amet",
		"te", "lorem ipsum dolor sit am");
	test_strtrim_case(tr("1 caracter que fica no meio"), "xxaxx", "x", "a");
}

static void	test_split_case(const char *desc, const char *s, char c,
				const char *const *expected, size_t count)
{
	char	**r;
	size_t	i;
	int		ok;

	r = ft_split(s, c);
	if (r == NULL)
	{
		report("ft_split", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	ok = 1;
	for (i = 0; i < count && ok; i++)
	{
		if (r[i] == NULL || strcmp(r[i], expected[i]) != 0
			|| inside_str(r[i], s))
			ok = 0;
	}
	if (ok && r[count] != NULL)
		ok = 0;
	if (!report("ft_split", desc, ok))
		out_printf(tr("         palavras incorretas, memória não independente ou "
			"falta o NULL final (esperado: %zu palavras)\n"), count);
	for (i = 0; i < count && r[i] != NULL; i++)
	{
		if (!inside_str(r[i], s))
			free(r[i]);
	}
	free(r);
}

static void	test_split(void)
{
	static const char *const	e1[] = {"hello", "world"};
	static const char *const	e2[] = {"hello"};
	static const char *const	e3[] = {"a", "b", "c", "d"};
	static const char *const	e4[] = {"a", "b"};
	static const char *const	e5[] = {"abc"};
	static const char *const	e6[] = {"split", "this", "for", "me", "!"};
	static const char *const	e7[] = {"lorem", "ipsum", "dolor", "sit",
		"amet"};
	static const char *const	e8[] = {"a", "b", "c"};
	static const char *const	e9[] = {"he", "o wor", "d"};

	section("ft_split");
	test_split_case(tr("duas palavras"), "hello world", ' ', e1, 2);
	test_split_case(tr("separadores repetidos, no início e no fim"),
		"  hello   world  ", ' ', e1, 2);
	test_split_case(tr("separadores só no início"), "   hello", ' ', e2, 1);
	test_split_case(tr("separadores só no fim"), "hello   ", ' ', e2, 1);
	test_split_case(tr("uma palavra, sem separadores"), "hello", ' ', e2, 1);
	test_split_case(tr("string vazia (array só com NULL)"), "", ' ', NULL, 0);
	test_split_case(tr("só separadores (array só com NULL)"), "     ", ' ',
		NULL, 0);
	test_split_case(tr("separador ','"), "a,b,c,d", ',', e3, 4);
	test_split_case(tr("',' repetidas e nas pontas"), ",a,,b,", ',', e4, 2);
	test_split_case(tr("separador inexistente"), "abc", 'x', e5, 1);
	test_split_case(tr("separador nas duas pontas"), "xabcx", 'x', e5, 1);
	test_split_case(tr("várias palavras"), "split  this for   me  !", ' ', e6, 5);
	test_split_case(tr("frase maior"), "lorem ipsum dolor sit amet", ' ', e7, 5);
	test_split_case(tr("palavras de 1 caracter"), "a b c", ' ', e8, 3);
	test_split_case(tr("separador 'l' no meio das palavras"), "hello world", 'l',
		e9, 3);
	test_split_case(tr("separador '\\0' devolve a string inteira"), "hello", '\0',
		e2, 1);
}

static void	test_itoa_case(int n)
{
	char	expected[16];
	char	desc[48];
	char	*r;

	snprintf(expected, sizeof(expected), "%d", n);
	snprintf(desc, sizeof(desc), "n = %d", n);
	r = ft_itoa(n);
	if (r == NULL)
	{
		report("ft_itoa", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	if (!report("ft_itoa", desc, strcmp(r, expected) == 0))
		out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"), r, expected);
	free(r);
}

static void	test_itoa(void)
{
	char	*r;
	char	expected[16];
	int		n;
	int		ok;

	section("ft_itoa");
	test_itoa_case(0);
	test_itoa_case(1);
	test_itoa_case(-1);
	test_itoa_case(9);
	test_itoa_case(10);
	test_itoa_case(-10);
	test_itoa_case(42);
	test_itoa_case(-42);
	test_itoa_case(100);
	test_itoa_case(123456789);
	test_itoa_case(-123456789);
	test_itoa_case(1000000);
	test_itoa_case(INT_MAX);
	test_itoa_case(INT_MIN);
	test_itoa_case(INT_MIN + 1);
	ok = 1;
	n = -1000;
	while (n <= 1000 && ok)
	{
		snprintf(expected, sizeof(expected), "%d", n);
		r = ft_itoa(n);
		if (r == NULL || strcmp(r, expected) != 0)
			ok = 0;
		free(r);
		if (ok)
			n++;
	}
	if (!report("ft_itoa", tr("todos os valores de -1000 a 1000"), ok))
		out_printf(tr("         primeiro valor errado: %d\n"), n);
}

static unsigned int	g_calls = 0;
static char			*g_base = NULL;
static int			g_ptr_ok = 1;

static char	map_upper(unsigned int i, char c)
{
	(void)i;
	g_calls++;
	if (c >= 'a' && c <= 'z')
		return ((char)(c - 32));
	return (c);
}

static char	map_index(unsigned int i, char c)
{
	(void)c;
	g_calls++;
	return ((char)('0' + i % 10));
}

static char	map_alt(unsigned int i, char c)
{
	g_calls++;
	if (i % 2 == 0 && c >= 'a' && c <= 'z')
		return ((char)(c - 32));
	if (i % 2 == 1 && c >= 'A' && c <= 'Z')
		return ((char)(c + 32));
	return (c);
}

static void	iter_upper(unsigned int i, char *c)
{
	g_calls++;
	if (c != g_base + i)
		g_ptr_ok = 0;
	if (*c >= 'a' && *c <= 'z')
		*c = (char)(*c - 32);
}

static void	iter_index(unsigned int i, char *c)
{
	g_calls++;
	if (c != g_base + i)
		g_ptr_ok = 0;
	*c = (char)('0' + i % 10);
}

static void	iter_alt(unsigned int i, char *c)
{
	g_calls++;
	if (c != g_base + i)
		g_ptr_ok = 0;
	if (i % 2 == 0 && *c >= 'a' && *c <= 'z')
		*c = (char)(*c - 32);
	else if (i % 2 == 1 && *c >= 'A' && *c <= 'Z')
		*c = (char)(*c + 32);
}

static void	test_strmapi_case(const char *desc, const char *s,
				char (*f)(unsigned int, char), const char *expected)
{
	char	a[1100];
	char	*r;
	int		ok_content;
	int		ok_calls;
	int		ok_mem;
	int		ok_src;

	strcpy(a, s);
	g_calls = 0;
	r = ft_strmapi(a, f);
	if (r == NULL)
	{
		report("ft_strmapi", desc, 0);
		out_printf(tr("         devolveu NULL\n"));
		return ;
	}
	ok_content = (strcmp(r, expected) == 0);
	ok_calls = (g_calls == (unsigned int)strlen(s));
	ok_mem = !inside_str(r, a);
	ok_src = (strcmp(a, s) == 0);
	if (!report("ft_strmapi", desc,
			ok_content && ok_calls && ok_mem && ok_src))
	{
		if (!ok_content)
			out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"),
				r, expected);
		if (!ok_calls)
			out_printf(tr("         f chamada %u vez(es) | esperado: %zu\n"),
				g_calls, strlen(s));
		if (!ok_mem)
			out_printf(tr("         devolveu um ponteiro para dentro de s\n"));
		if (!ok_src)
			out_printf(tr("         a string original foi alterada\n"));
	}
	if (ok_mem)
		free(r);
}

static void	test_strmapi(void)
{
	char	big[1001];
	char	bigexp[1001];
	int		i;

	for (i = 0; i < 1000; i++)
	{
		big[i] = 'x';
		bigexp[i] = (char)('0' + i % 10);
	}
	big[1000] = '\0';
	bigexp[1000] = '\0';
	section("ft_strmapi");
	test_strmapi_case(tr("maiúsculas (ignora o índice)"), "hello", map_upper,
		"HELLO");
	test_strmapi_case(tr("usa o índice (0, 1, 2, ...)"), "abcdef", map_index,
		"012345");
	test_strmapi_case(tr("índice > 9 (volta a 0)"), "abcdefghijkl", map_index,
		"012345678901");
	test_strmapi_case(tr("alterna maiúsculas e minúsculas"), "hello WORLD",
		map_alt, "HeLlO WoRlD");
	test_strmapi_case(tr("1 caracter"), "a", map_upper, "A");
	test_strmapi_case(tr("string vazia (f nunca é chamada)"), "", map_upper, "");
	test_strmapi_case(tr("string longa (1000 caracteres)"), big, map_index,
		bigexp);
}

static void	test_striteri_case(const char *desc, const char *s,
				void (*f)(unsigned int, char *), const char *expected)
{
	char	a[1100];
	int		ok_content;
	int		ok_calls;

	strcpy(a, s);
	g_calls = 0;
	g_base = a;
	g_ptr_ok = 1;
	ft_striteri(a, f);
	ok_content = (strcmp(a, expected) == 0);
	ok_calls = (g_calls == (unsigned int)strlen(s));
	if (!report("ft_striteri", desc, ok_content && ok_calls && g_ptr_ok))
	{
		if (!ok_content)
			out_printf(tr("         obtido: \"%s\" | esperado: \"%s\"\n"),
				a, expected);
		if (!ok_calls)
			out_printf(tr("         f chamada %u vez(es) | esperado: %zu\n"),
				g_calls, strlen(s));
		if (!g_ptr_ok)
			out_printf(tr("         f deve receber o endereço do próprio caracter "
				"(&s[i])\n"));
	}
}

static void	test_striteri(void)
{
	char	big[1001];
	char	bigexp[1001];
	int		i;

	for (i = 0; i < 1000; i++)
	{
		big[i] = 'x';
		bigexp[i] = (char)('0' + i % 10);
	}
	big[1000] = '\0';
	bigexp[1000] = '\0';
	section("ft_striteri");
	test_striteri_case(tr("maiúsculas (altera a string original)"), "hello",
		iter_upper, "HELLO");
	test_striteri_case(tr("usa o índice (0, 1, 2, ...)"), "abcdef", iter_index,
		"012345");
	test_striteri_case(tr("índice > 9 (volta a 0)"), "abcdefghijkl", iter_index,
		"012345678901");
	test_striteri_case(tr("alterna maiúsculas e minúsculas"), "hello WORLD",
		iter_alt, "HeLlO WoRlD");
	test_striteri_case(tr("1 caracter"), "a", iter_upper, "A");
	test_striteri_case(tr("string vazia (f nunca é chamada)"), "", iter_upper, "");
	test_striteri_case(tr("string longa (1000 caracteres)"), big, iter_index,
		bigexp);
}

static int	g_pipe[2];

static int	capture_start(void)
{
	if (pipe(g_pipe) != 0)
		return (-1);
	return (g_pipe[1]);
}

static size_t	capture_end(char *buf, size_t size)
{
	size_t	total;
	ssize_t	r;

	close(g_pipe[1]);
	total = 0;
	while (total < size - 1)
	{
		r = read(g_pipe[0], buf + total, size - 1 - total);
		if (r <= 0)
			break ;
		total += (size_t)r;
	}
	close(g_pipe[0]);
	buf[total] = '\0';
	return (total);
}

static void	print_escaped(const char *s, size_t n)
{
	size_t	i;

	out_printf("\"");
	for (i = 0; i < n; i++)
	{
		if (s[i] == '\n')
			out_printf("\\n");
		else if (s[i] >= 32 && s[i] <= 126)
			out_printf("%c", s[i]);
		else
			out_printf("\\x%02X", (unsigned char)s[i]);
	}
	out_printf("\"");
}

static void	check_output(const char *func, const char *desc, const char *got,
				size_t n, const char *expected, size_t exp_len)
{
	int	ok;

	ok = (n == exp_len && memcmp(got, expected, exp_len) == 0);
	if (!report(func, desc, ok))
	{
		out_printf(tr("         obtido: "));
		print_escaped(got, n);
		out_printf(tr(" (%zu bytes)\n         esperado: "), n);
		print_escaped(expected, exp_len);
		out_printf(tr(" (%zu bytes)\n"), exp_len);
	}
}

static void	pipe_error(const char *func, const char *desc)
{
	report(func, desc, 0);
	out_printf(tr("         não foi possível criar o pipe do teste\n"));
}

static void	test_putchar_case(const char *desc, char c)
{
	char	buf[16];
	size_t	n;
	int		fd;

	fd = capture_start();
	if (fd < 0)
		return (pipe_error("ft_putchar_fd", desc));
	ft_putchar_fd(c, fd);
	n = capture_end(buf, sizeof(buf));
	check_output("ft_putchar_fd", desc, buf, n, &c, 1);
}

static void	test_putchar_fd(void)
{
	section("ft_putchar_fd");
	test_putchar_case(tr("letra 'A'"), 'A');
	test_putchar_case(tr("dígito '7'"), '7');
	test_putchar_case(tr("espaço"), ' ');
	test_putchar_case(tr("newline"), '\n');
	test_putchar_case(tr("byte 0 ('\\0') é escrito"), '\0');
	test_putchar_case(tr("byte >= 128 (0xE9)"), (char)0xE9);
	test_putchar_case(tr("byte 0xFF"), (char)0xFF);
	ft_putchar_fd('x', -1);
	report("ft_putchar_fd", tr("fd inválido (-1) não crasha"), 1);
}

static void	test_putstr_case(const char *desc, const char *text)
{
	char	a[1100];
	char	buf[2048];
	size_t	n;
	int		fd;

	strcpy(a, text);
	fd = capture_start();
	if (fd < 0)
		return (pipe_error("ft_putstr_fd", desc));
	ft_putstr_fd(a, fd);
	n = capture_end(buf, sizeof(buf));
	check_output("ft_putstr_fd", desc, buf, n, text, strlen(text));
}

static void	test_putstr_fd(void)
{
	char	big[1001];
	char	a[4];

	memset(big, 'z', 1000);
	big[1000] = '\0';
	section("ft_putstr_fd");
	test_putstr_case(tr("string normal"), "Hello, World!");
	test_putstr_case(tr("1 caracter"), "a");
	test_putstr_case(tr("string vazia (não escreve nada)"), "");
	test_putstr_case(tr("com \\n e \\t"), "42\nLisboa\t!");
	test_putstr_case(tr("só escreve até ao '\\0'"), "ab\0cd");
	test_putstr_case(tr("bytes >= 128 (UTF-8)"), "caf\xc3\xa9");
	test_putstr_case(tr("string longa (1000 caracteres)"), big);
	strcpy(a, "x");
	ft_putstr_fd(a, -1);
	report("ft_putstr_fd", tr("fd inválido (-1) não crasha"), 1);
}

static void	test_putendl_case(const char *desc, const char *text)
{
	char	a[1100];
	char	expected[1102];
	char	buf[2048];
	size_t	n;
	int		fd;

	strcpy(a, text);
	strcpy(expected, text);
	strcat(expected, "\n");
	fd = capture_start();
	if (fd < 0)
		return (pipe_error("ft_putendl_fd", desc));
	ft_putendl_fd(a, fd);
	n = capture_end(buf, sizeof(buf));
	check_output("ft_putendl_fd", desc, buf, n, expected, strlen(expected));
}

static void	test_putendl_fd(void)
{
	char	big[1001];
	char	a[4];

	memset(big, 'z', 1000);
	big[1000] = '\0';
	section("ft_putendl_fd");
	test_putendl_case(tr("string normal + '\\n'"), "Hello, World!");
	test_putendl_case(tr("1 caracter + '\\n'"), "a");
	test_putendl_case(tr("string vazia escreve só '\\n'"), "");
	test_putendl_case(tr("string que já tem '\\n' no fim"), "linha\n");
	test_putendl_case(tr("string longa (1000 caracteres)"), big);
	strcpy(a, "x");
	ft_putendl_fd(a, -1);
	report("ft_putendl_fd", tr("fd inválido (-1) não crasha"), 1);
}

static void	test_putnbr_case(int num)
{
	char	expected[16];
	char	desc[32];
	char	buf[64];
	size_t	n;
	int		fd;

	snprintf(expected, sizeof(expected), "%d", num);
	snprintf(desc, sizeof(desc), "n = %d", num);
	fd = capture_start();
	if (fd < 0)
		return (pipe_error("ft_putnbr_fd", desc));
	ft_putnbr_fd(num, fd);
	n = capture_end(buf, sizeof(buf));
	check_output("ft_putnbr_fd", desc, buf, n, expected, strlen(expected));
}

static void	test_putnbr_fd(void)
{
	section("ft_putnbr_fd");
	test_putnbr_case(0);
	test_putnbr_case(5);
	test_putnbr_case(-5);
	test_putnbr_case(10);
	test_putnbr_case(-10);
	test_putnbr_case(42);
	test_putnbr_case(-42);
	test_putnbr_case(100);
	test_putnbr_case(123456789);
	test_putnbr_case(-123456789);
	test_putnbr_case(INT_MAX);
	test_putnbr_case(INT_MIN);
	test_putnbr_case(INT_MIN + 1);
	ft_putnbr_fd(42, -1);
	report("ft_putnbr_fd", tr("fd inválido (-1) não crasha"), 1);
}

static int		g_del_calls = 0;
static void		*g_del_args[16];
static int		g_iter_calls = 0;
static void		*g_iter_args[16];
static int		g_map_calls = 0;

static t_list	*mknode(void *content, t_list *next)
{
	t_list	*n;

	n = malloc(sizeof(t_list));
	if (n == NULL)
	{
		out_printf(tr("erro: malloc falhou dentro dos testes\n"));
		exit(1);
	}
	n->content = content;
	n->next = next;
	return (n);
}

static void	free_nodes(t_list *l)
{
	t_list	*next;

	while (l)
	{
		next = l->next;
		free(l);
		l = next;
	}
}

static t_list	*build_list(int n, int *vals)
{
	t_list	*head;
	int		i;

	head = NULL;
	for (i = n - 1; i >= 0; i--)
		head = mknode(&vals[i], head);
	return (head);
}

static int	count_nodes(t_list *l)
{
	int	n;

	n = 0;
	while (l && n < 1000)
	{
		n++;
		l = l->next;
	}
	return (n);
}

static void	reset_counters(void)
{
	g_del_calls = 0;
	g_iter_calls = 0;
	g_map_calls = 0;
	memset(g_del_args, 0, sizeof(g_del_args));
	memset(g_iter_args, 0, sizeof(g_iter_args));
}

static void	del_record(void *p)
{
	if (g_del_calls < 16)
		g_del_args[g_del_calls] = p;
	g_del_calls++;
}

static void	del_free(void *p)
{
	del_record(p);
	free(p);
}

static int	del_count_of(void *p)
{
	int	i;
	int	n;

	n = 0;
	for (i = 0; i < g_del_calls && i < 16; i++)
	{
		if (g_del_args[i] == p)
			n++;
	}
	return (n);
}

static void	lst_add10(void *c)
{
	if (g_iter_calls < 16)
		g_iter_args[g_iter_calls] = c;
	g_iter_calls++;
	*(int *)c += 10;
}

static void	*lst_double(void *c)
{
	int	*r;

	g_map_calls++;
	r = malloc(sizeof(int));
	if (r)
		*r = *(int *)c * 2;
	return (r);
}

static int	list_values_ok(t_list *l, const int *expected, int n)
{
	int	i;

	for (i = 0; i < n; i++)
	{
		if (l == NULL || l->content == NULL
			|| *(int *)l->content != expected[i])
			return (0);
		l = l->next;
	}
	return (l == NULL);
}

static void	test_lstnew(void)
{
	int		x;
	char	*str;
	t_list	*n1;
	t_list	*n2;

	x = 42;
	str = "hello";
	section("ft_lstnew");
	n1 = ft_lstnew(&x);
	report("ft_lstnew", tr("devolve um nó (não NULL)"), n1 != NULL);
	report("ft_lstnew", tr("content guarda o ponteiro recebido"),
		n1 != NULL && n1->content == &x);
	report("ft_lstnew", tr("next é NULL"), n1 != NULL && n1->next == NULL);
	n2 = ft_lstnew(str);
	report("ft_lstnew", tr("content com uma string"),
		n2 != NULL && n2->content == str);
	report("ft_lstnew", tr("cada chamada devolve um nó diferente"),
		n1 != NULL && n2 != NULL && n1 != n2);
	free(n1);
	free(n2);
	n1 = ft_lstnew(NULL);
	report("ft_lstnew", tr("content NULL é aceite (next também NULL)"),
		n1 != NULL && n1->content == NULL && n1->next == NULL);
	free(n1);
}

static void	test_lstadd_front(void)
{
	int		v[3];
	t_list	*head;
	t_list	*a;
	t_list	*b;
	t_list	*c;

	v[0] = 0;
	v[1] = 1;
	v[2] = 2;
	a = mknode(&v[0], NULL);
	b = mknode(&v[1], NULL);
	c = mknode(&v[2], NULL);
	head = NULL;
	section("ft_lstadd_front");
	ft_lstadd_front(&head, a);
	report("ft_lstadd_front", tr("lista vazia: o novo nó passa a ser a cabeça"),
		head == a && a->next == NULL);
	ft_lstadd_front(&head, b);
	report("ft_lstadd_front", tr("a cabeça passa a ser o novo nó"), head == b);
	report("ft_lstadd_front", tr("o novo nó aponta para a antiga cabeça"),
		b->next == a && a->next == NULL);
	ft_lstadd_front(&head, c);
	report("ft_lstadd_front", tr("3 inserções: ordem c -> b -> a"),
		head == c && c->next == b && b->next == a && a->next == NULL);
	report("ft_lstadd_front", tr("tamanho final = 3"), count_nodes(head) == 3);
	free_nodes(head);
}

static void	test_lstsize_case(const char *desc, int n)
{
	int		vals[100];
	t_list	*l;
	int		i;

	for (i = 0; i < 100; i++)
		vals[i] = i;
	l = build_list(n, vals);
	check_long("ft_lstsize", desc, ft_lstsize(l), n);
	free_nodes(l);
}

static void	test_lstsize(void)
{
	section("ft_lstsize");
	check_long("ft_lstsize", tr("lista vazia (NULL)"), ft_lstsize(NULL), 0);
	test_lstsize_case(tr("1 nó"), 1);
	test_lstsize_case(tr("2 nós"), 2);
	test_lstsize_case(tr("5 nós"), 5);
	test_lstsize_case(tr("100 nós"), 100);
}

static void	test_lstlast(void)
{
	int		vals[5];
	t_list	*l;
	t_list	*last;
	int		i;

	for (i = 0; i < 5; i++)
		vals[i] = i;
	section("ft_lstlast");
	report("ft_lstlast", tr("lista vazia (NULL) devolve NULL"),
		ft_lstlast(NULL) == NULL);
	l = build_list(1, vals);
	report("ft_lstlast", tr("1 nó: devolve esse nó"), ft_lstlast(l) == l);
	free_nodes(l);
	l = build_list(2, vals);
	report("ft_lstlast", tr("2 nós: devolve o segundo"), ft_lstlast(l) == l->next);
	free_nodes(l);
	l = build_list(5, vals);
	last = l;
	while (last->next)
		last = last->next;
	report("ft_lstlast", tr("5 nós: devolve o último"), ft_lstlast(l) == last);
	report("ft_lstlast", tr("o último nó tem next == NULL"),
		ft_lstlast(l) != NULL && ft_lstlast(l)->next == NULL);
	report("ft_lstlast", tr("a lista não é alterada"),
		count_nodes(l) == 5 && *(int *)l->content == 0);
	free_nodes(l);
}

static void	test_lstadd_back(void)
{
	int		v[8];
	t_list	*head;
	t_list	*a;
	t_list	*b;
	t_list	*c;
	t_list	*last;
	int		i;

	for (i = 0; i < 8; i++)
		v[i] = i;
	section("ft_lstadd_back");
	a = mknode(&v[0], NULL);
	b = mknode(&v[1], NULL);
	c = mknode(&v[2], NULL);
	head = NULL;
	ft_lstadd_back(&head, a);
	report("ft_lstadd_back", tr("lista vazia: o novo nó passa a ser a cabeça"),
		head == a && a->next == NULL);
	ft_lstadd_back(&head, b);
	report("ft_lstadd_back", tr("a cabeça mantém-se"), head == a);
	report("ft_lstadd_back", tr("o novo nó fica depois do último"),
		a->next == b && b->next == NULL);
	ft_lstadd_back(&head, c);
	report("ft_lstadd_back", tr("3 inserções: ordem a -> b -> c"),
		head == a && a->next == b && b->next == c && c->next == NULL);
	report("ft_lstadd_back", tr("tamanho final = 3"), count_nodes(head) == 3);
	free_nodes(head);
	head = build_list(5, v);
	a = mknode(&v[5], NULL);
	last = head;
	while (last->next)
		last = last->next;
	ft_lstadd_back(&head, a);
	report("ft_lstadd_back", tr("lista com 5 nós: novo nó fica no fim"),
		last->next == a && a->next == NULL && count_nodes(head) == 6);
	free_nodes(head);
}

static void	test_lstdelone(void)
{
	t_list	*a;
	t_list	*b;
	t_list	*single;
	char	*s;
	int		x;
	int		y;
	int		b_ok;

	x = 7;
	y = 9;
	section("ft_lstdelone");
	s = malloc(6);
	strcpy(s, "hello");
	b = mknode(&x, NULL);
	a = mknode(s, b);
	reset_counters();
	ft_lstdelone(a, del_free);
	report("ft_lstdelone", tr("chama del exatamente 1 vez"), g_del_calls == 1);
	report("ft_lstdelone", tr("del recebe o content do nó"), g_del_args[0] == s);
	b_ok = (b->content == &x && b->next == NULL);
	report("ft_lstdelone", tr("não apaga o nó seguinte"), b_ok);
	if (b_ok)
		free(b);
	single = mknode(&y, NULL);
	reset_counters();
	ft_lstdelone(single, del_record);
	report("ft_lstdelone", tr("nó isolado: del recebe o content certo"),
		g_del_calls == 1 && g_del_args[0] == &y);
}

static void	test_lstclear(void)
{
	int		vals[4];
	t_list	*head;
	char	*strs[3];
	int		i;
	int		ok;

	for (i = 0; i < 4; i++)
		vals[i] = i;
	section("ft_lstclear");
	head = build_list(4, vals);
	reset_counters();
	ft_lstclear(&head, del_record);
	ok = (g_del_calls == 4);
	for (i = 0; i < 4; i++)
		ok = ok && (del_count_of(&vals[i]) == 1);
	report("ft_lstclear", tr("4 nós: del chamada 1 vez por cada content"), ok);
	report("ft_lstclear", tr("o ponteiro da lista fica NULL"), head == NULL);
	head = build_list(1, vals);
	reset_counters();
	ft_lstclear(&head, del_record);
	report("ft_lstclear", tr("1 nó: del chamada 1 vez e lista NULL"),
		g_del_calls == 1 && g_del_args[0] == &vals[0] && head == NULL);
	head = NULL;
	reset_counters();
	ft_lstclear(&head, del_record);
	report("ft_lstclear", tr("lista vazia: del não é chamada, lista continua NULL"),
		g_del_calls == 0 && head == NULL);
	head = NULL;
	for (i = 0; i < 3; i++)
	{
		strs[i] = malloc(8);
		strcpy(strs[i], "abc");
		head = mknode(strs[i], head);
	}
	reset_counters();
	ft_lstclear(&head, del_free);
	report("ft_lstclear", tr("conteúdos em heap: del (free) chamada 3 vezes"),
		g_del_calls == 3 && head == NULL);
}

static void	test_lstiter(void)
{
	int		vals[3];
	int		single;
	t_list	*l;
	int		expected[3];
	int		in_order;

	vals[0] = 1;
	vals[1] = 2;
	vals[2] = 3;
	expected[0] = 11;
	expected[1] = 12;
	expected[2] = 13;
	section("ft_lstiter");
	l = build_list(3, vals);
	reset_counters();
	ft_lstiter(l, lst_add10);
	report("ft_lstiter", tr("f aplicada ao content de cada nó"),
		vals[0] == 11 && vals[1] == 12 && vals[2] == 13);
	report("ft_lstiter", tr("f chamada 1 vez por nó"), g_iter_calls == 3);
	in_order = (g_iter_args[0] == &vals[0] && g_iter_args[1] == &vals[1]
			&& g_iter_args[2] == &vals[2]);
	report("ft_lstiter", tr("percorre do 1º ao último nó (por ordem)"), in_order);
	report("ft_lstiter", tr("a lista não é alterada (continua com 3 nós)"),
		count_nodes(l) == 3 && list_values_ok(l, expected, 3));
	free_nodes(l);
	reset_counters();
	ft_lstiter(NULL, lst_add10);
	report("ft_lstiter", tr("lista vazia: f não é chamada"), g_iter_calls == 0);
	single = 5;
	l = mknode(&single, NULL);
	reset_counters();
	ft_lstiter(l, lst_add10);
	report("ft_lstiter", tr("1 nó: f chamada 1 vez"), g_iter_calls == 1
		&& single == 15);
	free_nodes(l);
}

static void	free_mapped(t_list *l)
{
	t_list	*next;

	while (l)
	{
		next = l->next;
		free(l->content);
		free(l);
		l = next;
	}
}

static void	test_lstmap(void)
{
	int		vals[4];
	int		orig[4];
	int		doubled[4];
	t_list	*l;
	t_list	*res;
	t_list	*p;
	t_list	*q;
	int		distinct;

	vals[0] = 1;
	vals[1] = 2;
	vals[2] = 3;
	vals[3] = 4;
	memcpy(orig, vals, sizeof(orig));
	doubled[0] = 2;
	doubled[1] = 4;
	doubled[2] = 6;
	doubled[3] = 8;
	section("ft_lstmap");
	l = build_list(4, vals);
	reset_counters();
	res = ft_lstmap(l, lst_double, del_free);
	report("ft_lstmap", tr("devolve uma lista (não NULL)"), res != NULL);
	report("ft_lstmap", tr("nova lista com 4 nós e valores 2, 4, 6, 8 (por ordem)"),
		list_values_ok(res, doubled, 4));
	report("ft_lstmap", tr("f chamada 1 vez por nó"), g_map_calls == 4);
	report("ft_lstmap", tr("del não é chamada quando tudo corre bem"),
		g_del_calls == 0);
	report("ft_lstmap", tr("a lista original não é alterada"),
		list_values_ok(l, orig, 4) && vals[0] == 1 && vals[3] == 4);
	distinct = 1;
	for (p = res, q = l; p && q; p = p->next, q = q->next)
	{
		if (p == q || p->content == q->content)
			distinct = 0;
	}
	report("ft_lstmap", tr("os nós e contents da nova lista são independentes"),
		distinct);
	free_mapped(res);
	free_nodes(l);
	reset_counters();
	res = ft_lstmap(NULL, lst_double, del_free);
	report("ft_lstmap", tr("lista vazia devolve NULL (f não é chamada)"),
		res == NULL && g_map_calls == 0);
	l = build_list(1, vals);
	res = ft_lstmap(l, lst_double, del_free);
	report("ft_lstmap", tr("1 nó: nova lista com 1 nó (valor 2)"),
		list_values_ok(res, doubled, 1));
	free_mapped(res);
	free_nodes(l);
}



typedef struct s_tr
{
	const char	*pt;
	const char	*en;
}	t_tr;

static const t_tr	g_tr[] = {
	{"         obtido: %ld | esperado: %ld\n",
		"         got: %ld | expected: %ld\n"},
	{"todos os valores 0-127 (inclui os limites)",
		"all values 0-127 (includes the limits)"},
	{"         primeiro valor errado: %d\n",
		"         first wrong value: %d\n"},
	{"todos os valores 128-255",
		"all values 128-255"},
	{"valores fora do intervalo (-1, 256, 1000)",
		"values outside the range (-1, 256, 1000)"},
	{"         valor errado: %d\n",
		"         wrong value: %d\n"},
	{"         bytes diferentes do esperado (ou bytes fora da zona alterados)\n",
		"         bytes differ from expected (or bytes outside the area were changed)\n"},
	{"         o endereço devolvido não é o de dest\n",
		"         the returned address is not dest\n"},
	{"preencher 10 bytes no início com 'A'",
		"fill 10 bytes at the start with 'A'"},
	{"preencher no meio (offset 5, 8 bytes) com 0",
		"fill in the middle (offset 5, 8 bytes) with 0"},
	{"n = 0 não altera nada",
		"n = 0 changes nothing"},
	{"preencher o buffer inteiro",
		"fill the whole buffer"},
	{"preencher só o último byte",
		"fill only the last byte"},
	{"valor 321 convertido para unsigned char",
		"value 321 converted to unsigned char"},
	{"valor -1 convertido para 0xFF",
		"value -1 converted to 0xFF"},
	{"valor 255",
		"value 255"},
	{"zerar 10 bytes no início",
		"zero 10 bytes at the start"},
	{"zerar no meio (offset 4, 10 bytes)",
		"zero in the middle (offset 4, 10 bytes)"},
	{"zerar o buffer inteiro",
		"zero the whole buffer"},
	{"zerar só o primeiro byte",
		"zero only the first byte"},
	{"zerar só o último byte",
		"zero only the last byte"},
	{"         a origem foi alterada\n",
		"         the source was modified\n"},
	{"copiar 1 byte",
		"copy 1 byte"},
	{"copiar 10 bytes (passa por um byte 0)",
		"copy 10 bytes (goes through a 0 byte)"},
	{"copiar para o meio do destino (offset 5)",
		"copy into the middle of the destination (offset 5)"},
	{"copiar o buffer inteiro",
		"copy the whole buffer"},
	{"copiar só para o último byte",
		"copy only into the last byte"},
	{"copiar uma string com o '\\0' final",
		"copy a string with its final '\\0'"},
	{"         bytes diferentes do esperado\n",
		"         bytes differ from expected\n"},
	{"sobreposição: destino depois da origem",
		"overlap: destination after the source"},
	{"sobreposição: destino antes da origem",
		"overlap: destination before the source"},
	{"sobreposição de 1 byte (dest = src + 1)",
		"1-byte overlap (dest = src + 1)"},
	{"sobreposição de 1 byte (dest = src - 1)",
		"1-byte overlap (dest = src - 1)"},
	{"sobreposição máxima (dest = src + 1)",
		"maximum overlap (dest = src + 1)"},
	{"sobreposição máxima (dest = src - 1)",
		"maximum overlap (dest = src - 1)"},
	{"dest igual a src",
		"dest equal to src"},
	{"zonas adjacentes sem sobreposição (frente)",
		"adjacent areas without overlap (forward)"},
	{"zonas adjacentes sem sobreposição (trás)",
		"adjacent areas without overlap (backward)"},
	{"zonas disjuntas",
		"disjoint areas"},
	{"mover o buffer inteiro para ele próprio",
		"move the whole buffer onto itself"},
	{"         endereço obtido: %p | esperado: %p\n",
		"         address got: %p | expected: %p\n"},
	{"byte existente (devolve a 1ª ocorrência)",
		"existing byte (returns the 1st occurrence)"},
	{"procurar o byte 0 no meio dos dados",
		"search for byte 0 in the middle of the data"},
	{"byte 0 fora do limite n",
		"byte 0 outside the limit n"},
	{"byte >= 128 (unsigned char)",
		"byte >= 128 (unsigned char)"},
	{"c = 456 é convertido para 200",
		"c = 456 is converted to 200"},
	{"c = -56 é convertido para 200",
		"c = -56 is converted to 200"},
	{"byte inexistente",
		"non-existent byte"},
	{"último byte incluído em n",
		"last byte included in n"},
	{"último byte fora de n",
		"last byte outside n"},
	{"byte logo depois do limite n",
		"byte right after the limit n"},
	{"byte exatamente no limite n",
		"byte exactly at the limit n"},
	{"n = 0 devolve NULL",
		"n = 0 returns NULL"},
	{"n = 1 com o 1º byte",
		"n = 1 with the 1st byte"},
	{"procurar numa string",
		"search in a string"},
	{"         sinal obtido: %d | esperado: %d\n",
		"         sign got: %d | expected: %d\n"},
	{"blocos iguais",
		"equal blocks"},
	{"a < b (diferem no meio)",
		"a < b (differ in the middle)"},
	{"a > b (diferem no meio)",
		"a > b (differ in the middle)"},
	{"diferença depois do limite n devolve 0",
		"difference after the limit n returns 0"},
	{"diferença no último byte",
		"difference in the last byte"},
	{"diferença no último byte, mas fora de n",
		"difference in the last byte, but outside n"},
	{"n = 0 devolve 0",
		"n = 0 returns 0"},
	{"não pára no byte 0 (a < b)",
		"does not stop at byte 0 (a < b)"},
	{"não pára no byte 0 (iguais)",
		"does not stop at byte 0 (equal)"},
	{"comparação sem sinal: 0x80 vs 0x7F",
		"unsigned comparison: 0x80 vs 0x7F"},
	{"comparação sem sinal: 0x7F vs 0x80",
		"unsigned comparison: 0x7F vs 0x80"},
	{"comparação sem sinal: 0xFF vs 0x01",
		"unsigned comparison: 0xFF vs 0x01"},
	{"comparação sem sinal: 0x01 vs 0xFF",
		"unsigned comparison: 0x01 vs 0xFF"},
	{"strings iguais",
		"equal strings"},
	{"strings diferentes",
		"different strings"},
	{"string vazia",
		"empty string"},
	{"1 caractere",
		"1 character"},
	{"string normal",
		"normal string"},
	{"string com espaços e \\t \\n",
		"string with spaces and \\t \\n"},
	{"pára no primeiro '\\0'",
		"stops at the first '\\0'"},
	{"bytes >= 128 (UTF-8)",
		"bytes >= 128 (UTF-8)"},
	{"string longa (1000 caracteres)",
		"long string (1000 characters)"},
	{"         retorno obtido: %zu | esperado: %zu\n",
		"         return got: %zu | expected: %zu\n"},
	{"         conteúdo do buffer diferente do esperado (esperado: \"%s\")\n",
		"         buffer content differs from expected (expected: \"%s\")\n"},
	{"buffer grande",
		"large buffer"},
	{"tamanho = strlen + 1 (cabe justo)",
		"size = strlen + 1 (fits exactly)"},
	{"tamanho = strlen (trunca 1 char)",
		"size = strlen (truncates 1 char)"},
	{"buffer pequeno (size 3)",
		"small buffer (size 3)"},
	{"size 1 só escreve '\\0'",
		"size 1 only writes '\\0'"},
	{"size 0 não escreve nada",
		"size 0 writes nothing"},
	{"src vazia",
		"empty src"},
	{"src vazia com size 0",
		"empty src with size 0"},
	{"src muito maior que o buffer",
		"src much larger than the buffer"},
	{"pára no '\\0' da src",
		"stops at the '\\0' of src"},
	{"cabe justo (size 12)",
		"fits exactly (size 12)"},
	{"trunca 1 char (size 11)",
		"truncates 1 char (size 11)"},
	{"buffer pequeno (size 8)",
		"small buffer (size 8)"},
	{"sem espaço para copiar (size = dlen + 1)",
		"no room to copy (size = dlen + 1)"},
	{"size = strlen(dst) não altera",
		"size = strlen(dst) changes nothing"},
	{"size < strlen(dst) não altera",
		"size < strlen(dst) changes nothing"},
	{"size 0 não altera",
		"size 0 changes nothing"},
	{"dst vazio",
		"empty dst"},
	{"dst vazio com size 1",
		"empty dst with size 1"},
	{"dst vazio com size 0",
		"empty dst with size 0"},
	{"dst e src vazios",
		"empty dst and src"},
	{"1ª ocorrência de 'o'",
		"1st occurrence of 'o'"},
	{"1º caracter da string",
		"1st character of the string"},
	{"último caracter da string",
		"last character of the string"},
	{"caracter inexistente devolve NULL",
		"non-existent character returns NULL"},
	{"diferencia maiúsculas de minúsculas",
		"case-sensitive"},
	{"procurar '\\0' devolve o fim da string",
		"searching '\\0' returns the end of the string"},
	{"procurar '\\0' numa string vazia",
		"searching '\\0' in an empty string"},
	{"procurar 'a' numa string vazia",
		"searching 'a' in an empty string"},
	{"caracter repetido (devolve o 1º)",
		"repeated character (returns the 1st)"},
	{"c = 'o' + 256 é convertido para char",
		"c = 'o' + 256 is converted to char"},
	{"caracter >= 128",
		"character >= 128"},
	{"última ocorrência de 'o'",
		"last occurrence of 'o'"},
	{"última ocorrência de 'l'",
		"last occurrence of 'l'"},
	{"ocorrência única no início",
		"single occurrence at the start"},
	{"caracter repetido (devolve o último)",
		"repeated character (returns the last)"},
	{"n maior que o tamanho (iguais)",
		"n larger than the size (equal)"},
	{"s1 é prefixo de s2 (n = 3)",
		"s1 is a prefix of s2 (n = 3)"},
	{"s1 é prefixo de s2 (n = 4)",
		"s1 is a prefix of s2 (n = 4)"},
	{"s2 é prefixo de s1 (n = 4)",
		"s2 is a prefix of s1 (n = 4)"},
	{"duas strings vazias",
		"two empty strings"},
	{"s1 vazia",
		"empty s1"},
	{"s2 vazia",
		"empty s2"},
	{"pára no '\\0' (ignora o que vem depois)",
		"stops at '\\0' (ignores what comes after)"},
	{"comparação sem sinal (0x80 vs 0x7F)",
		"unsigned comparison (0x80 vs 0x7F)"},
	{"comparação sem sinal (0x7F vs 0x80)",
		"unsigned comparison (0x7F vs 0x80)"},
	{"char >= 128 vs '\\0'",
		"char >= 128 vs '\\0'"},
	{"         obtido: NULL | esperado: posição %d\n",
		"         got: NULL | expected: position %d\n"},
	{"         obtido: posição %ld | esperado: NULL\n",
		"         got: position %ld | expected: NULL\n"},
	{"         obtido: posição %ld | esperado: posição %d\n",
		"         got: position %ld | expected: position %d\n"},
	{"encontra no fim (len = tamanho)",
		"finds at the end (len = size)"},
	{"len 1 byte curto: não encontra",
		"len 1 byte short: not found"},
	{"encontra no início",
		"finds at the start"},
	{"len curto no início: não encontra",
		"short len at the start: not found"},
	{"1ª ocorrência de 1 caracter",
		"1st occurrence of 1 character"},
	{"1 caracter fora do limite len",
		"1 character outside the limit len"},
	{"1 caracter exatamente no limite",
		"1 character exactly at the limit"},
	{"substring inexistente",
		"non-existent substring"},
	{"needle vazia devolve big",
		"empty needle returns big"},
	{"needle vazia com len 0 devolve big",
		"empty needle with len 0 returns big"},
	{"len 0 com needle não vazia",
		"len 0 with non-empty needle"},
	{"big vazia, needle não vazia",
		"empty big, non-empty needle"},
	{"big e needle vazias (len 0)",
		"empty big and needle (len 0)"},
	{"big e needle vazias (len 5)",
		"empty big and needle (len 5)"},
	{"needle maior que big",
		"needle longer than big"},
	{"needle igual a big",
		"needle equal to big"},
	{"needle igual a big, len curto",
		"needle equal to big, short len"},
	{"len maior que a string (pára no '\\0')",
		"len larger than the string (stops at '\\0')"},
	{"len maior que a string, needle inexistente",
		"len larger than the string, non-existent needle"},
	{"fim da string ('lo')",
		"end of the string ('lo')"},
	{"fim da string, len curto",
		"end of the string, short len"},
	{"início de match falso ('aab')",
		"start of a false match ('aab')"},
	{"match falso antes do verdadeiro",
		"false match before the real one"},
	{"mississippi / issip",
		"mississippi / issip"},
	{"mississippi / issipi",
		"mississippi / issipi"},
	{"match no limite exato ('bc', len 3)",
		"match at the exact limit ('bc', len 3)"},
	{"match 1 byte fora do limite ('bc', len 2)",
		"match 1 byte outside the limit ('bc', len 2)"},
	{"zero: \"0\"",
		"zero: \"0\""},
	{"positivo: \"42\"",
		"positive: \"42\""},
	{"negativo: \"-42\"",
		"negative: \"-42\""},
	{"sinal +: \"+42\"",
		"sign +: \"+42\""},
	{"zeros à esquerda: \"0042\"",
		"leading zeros: \"0042\""},
	{"menos zero: \"-0\"",
		"minus zero: \"-0\""},
	{"espaços iniciais: \"   42\"",
		"leading spaces: \"   42\""},
	{"todos os espaços (\\t\\n\\v\\f\\r e ' ') antes do número",
		"all whitespace (\\t\\n\\v\\f\\r and ' ') before the number"},
	{"espaços antes do sinal: \"  -42\"",
		"spaces before the sign: \"  -42\""},
	{"espaços e + : \" \\t+13xyz\"",
		"spaces and + : \" \\t+13xyz\""},
	{"lixo depois do número: \"42abc\"",
		"garbage after the number: \"42abc\""},
	{"lixo depois do negativo: \"  -42abc\"",
		"garbage after the negative: \"  -42abc\""},
	{"espaço no meio: \"12 34\"",
		"space in the middle: \"12 34\""},
	{"só letras: \"abc\"",
		"only letters: \"abc\""},
	{"letra antes do número: \"a42\"",
		"letter before the number: \"a42\""},
	{"só espaços",
		"only spaces"},
	{"só o sinal '-'",
		"only the sign '-'"},
	{"só o sinal '+'",
		"only the sign '+'"},
	{"dois sinais: \"+-5\"",
		"two signs: \"+-5\""},
	{"dois sinais: \"-+5\"",
		"two signs: \"-+5\""},
	{"dois sinais: \"--5\"",
		"two signs: \"--5\""},
	{"dois sinais: \"++5\"",
		"two signs: \"++5\""},
	{"espaço depois do sinal: \"- 5\"",
		"space after the sign: \"- 5\""},
	{"espaço depois do +: \"+ 5\"",
		"space after the +: \"+ 5\""},
	{"INT_MAX: \"2147483647\"",
		"INT_MAX: \"2147483647\""},
	{"INT_MIN: \"-2147483648\"",
		"INT_MIN: \"-2147483648\""},
	{"INT_MAX com espaços",
		"INT_MAX with spaces"},
	{"         devolveu NULL\n",
		"         returned NULL\n"},
	{"         ft_calloc devolveu %s, a calloc da libc devolveu %s\n",
		"         ft_calloc returned %s, libc calloc returned %s\n"},
	{"overflow de count * size devolve NULL (recomendado)",
		"overflow of count * size returns NULL (recommended)"},
	{"         count * size dá a volta e a memória devolvida é demasiado pequena\n",
		"         count * size wraps around and the returned memory is too small\n"},
	{"5 ints a zero",
		"5 ints set to zero"},
	{"1 byte a zero",
		"1 byte set to zero"},
	{"100 chars a zero",
		"100 chars set to zero"},
	{"1000 ints a zero",
		"1000 ints set to zero"},
	{"bloco grande (4096 bytes) a zero",
		"large block (4096 bytes) set to zero"},
	{"10 elementos de 16 bytes a zero",
		"10 elements of 16 bytes set to zero"},
	{"count = 0 e size = 0",
		"count = 0 and size = 0"},
	{"count = 0 e size = 4",
		"count = 0 and size = 4"},
	{"count = 4 e size = 0",
		"count = 4 and size = 0"},
	{"%s - conteúdo e '\\0' final",
		"%s - content and final '\\0'"},
	{"%s - memória diferente da original",
		"%s - memory different from the original"},
	{"%s - alterar a cópia não altera o original",
		"%s - changing the copy does not change the original"},
	{"1 caracter",
		"1 character"},
	{"espaços e caracteres especiais",
		"spaces and special characters"},
	{"         valor %d: obtido %d | esperado %d\n",
		"         value %d: got %d | expected %d\n"},
	{"todos os valores 128-255 (ficam iguais)",
		"all values 128-255 (stay the same)"},
	{"limites das letras (um antes, a primeira, a última, um depois)",
		"letter limits (one before, the first, the last, one after)"},
	{"valores fora do intervalo (-1, 256, 1000) ficam iguais",
		"values outside the range (-1, 256, 1000) stay the same"},
	{"         obtido: \"%s\" | esperado: \"%s\"\n",
		"         got: \"%s\" | expected: \"%s\"\n"},
	{"         devolveu um ponteiro para dentro de s (deve ser memória nova)\n",
		"         returned a pointer inside s (must be new memory)\n"},
	{"         a string original foi alterada\n",
		"         the original string was modified\n"},
	{"do início, 5 caracteres",
		"from the start, 5 characters"},
	{"do meio até ao fim",
		"from the middle to the end"},
	{"no meio da string",
		"in the middle of the string"},
	{"1 caracter no fim",
		"1 character at the end"},
	{"a string inteira",
		"the whole string"},
	{"len maior que o resto (copia só até ao fim)",
		"len larger than the rest (copies only up to the end)"},
	{"len muito maior que a string (1000000)",
		"len much larger than the string (1000000)"},
	{"len = 0 devolve string vazia",
		"len = 0 returns an empty string"},
	{"start depois do fim devolve string vazia",
		"start after the end returns an empty string"},
	{"start = strlen devolve string vazia",
		"start = strlen returns an empty string"},
	{"start = UINT_MAX devolve string vazia",
		"start = UINT_MAX returns an empty string"},
	{"string de entrada vazia",
		"empty input string"},
	{"         devolveu um ponteiro para dentro de s1 ou s2\n",
		"         returned a pointer inside s1 or s2\n"},
	{"         s1 ou s2 foram alteradas\n",
		"         s1 or s2 were modified\n"},
	{"duas strings normais",
		"two normal strings"},
	{"sem espaços",
		"without spaces"},
	{"ambas vazias",
		"both empty"},
	{"1 caracter + 1 caracter",
		"1 character + 1 character"},
	{"com espaços e \\t \\n",
		"with spaces and \\t \\n"},
	{"strings longas (500 + 500)",
		"long strings (500 + 500)"},
	{"         devolveu um ponteiro para dentro de s1 (deve ser memória nova)\n",
		"         returned a pointer inside s1 (must be new memory)\n"},
	{"         s1 foi alterada\n",
		"         s1 was modified\n"},
	{"espaços dos dois lados",
		"spaces on both sides"},
	{"só no início",
		"only at the start"},
	{"só no fim",
		"only at the end"},
	{"1 caracter repetido",
		"1 repeated character"},
	{"vários caracteres no set",
		"several characters in the set"},
	{"a ordem do set não importa",
		"the order of the set does not matter"},
	{"não corta o interior da string",
		"does not trim the inside of the string"},
	{"whitespace variado",
		"mixed whitespace"},
	{"nada para cortar",
		"nothing to trim"},
	{"set vazio",
		"empty set"},
	{"s1 só com caracteres do set",
		"s1 only with characters from the set"},
	{"s1 igual ao set",
		"s1 equal to the set"},
	{"s1 e set vazios",
		"s1 and set empty"},
	{"corta 't' e 'e' no fim",
		"trims 't' and 'e' at the end"},
	{"1 caracter que fica no meio",
		"1 character that stays in the middle"},
	{"         palavras incorretas, memória não independente ou falta o NULL final (esperado: %zu palavras)\n",
		"         wrong words, non-independent memory or missing final NULL (expected: %zu words)\n"},
	{"duas palavras",
		"two words"},
	{"separadores repetidos, no início e no fim",
		"repeated separators, at the start and at the end"},
	{"separadores só no início",
		"separators only at the start"},
	{"separadores só no fim",
		"separators only at the end"},
	{"uma palavra, sem separadores",
		"one word, no separators"},
	{"string vazia (array só com NULL)",
		"empty string (array with only NULL)"},
	{"só separadores (array só com NULL)",
		"only separators (array with only NULL)"},
	{"separador ','",
		"separator ','"},
	{"',' repetidas e nas pontas",
		"repeated ',' and at the ends"},
	{"separador inexistente",
		"non-existent separator"},
	{"separador nas duas pontas",
		"separator on both ends"},
	{"várias palavras",
		"several words"},
	{"frase maior",
		"longer sentence"},
	{"palavras de 1 caracter",
		"1-character words"},
	{"separador 'l' no meio das palavras",
		"separator 'l' in the middle of the words"},
	{"separador '\\0' devolve a string inteira",
		"separator '\\0' returns the whole string"},
	{"todos os valores de -1000 a 1000",
		"all values from -1000 to 1000"},
	{"         f chamada %u vez(es) | esperado: %zu\n",
		"         f called %u time(s) | expected: %zu\n"},
	{"         devolveu um ponteiro para dentro de s\n",
		"         returned a pointer inside s\n"},
	{"maiúsculas (ignora o índice)",
		"uppercase (ignores the index)"},
	{"usa o índice (0, 1, 2, ...)",
		"uses the index (0, 1, 2, ...)"},
	{"índice > 9 (volta a 0)",
		"index > 9 (wraps back to 0)"},
	{"alterna maiúsculas e minúsculas",
		"alternates upper and lower case"},
	{"string vazia (f nunca é chamada)",
		"empty string (f is never called)"},
	{"         f deve receber o endereço do próprio caracter (&s[i])\n",
		"         f must receive the address of the character itself (&s[i])\n"},
	{"maiúsculas (altera a string original)",
		"uppercase (changes the original string)"},
	{"         obtido: ",
		"         got: "},
	{" (%zu bytes)\n         esperado: ",
		" (%zu bytes)\n         expected: "},
	{" (%zu bytes)\n",
		" (%zu bytes)\n"},
	{"         não foi possível criar o pipe do teste\n",
		"         could not create the test pipe\n"},
	{"letra 'A'",
		"letter 'A'"},
	{"dígito '7'",
		"digit '7'"},
	{"espaço",
		"space"},
	{"newline",
		"newline"},
	{"byte 0 ('\\0') é escrito",
		"byte 0 ('\\0') is written"},
	{"byte >= 128 (0xE9)",
		"byte >= 128 (0xE9)"},
	{"byte 0xFF",
		"byte 0xFF"},
	{"fd inválido (-1) não crasha",
		"invalid fd (-1) does not crash"},
	{"string vazia (não escreve nada)",
		"empty string (writes nothing)"},
	{"com \\n e \\t",
		"with \\n and \\t"},
	{"só escreve até ao '\\0'",
		"only writes up to the '\\0'"},
	{"string normal + '\\n'",
		"normal string + '\\n'"},
	{"1 caracter + '\\n'",
		"1 character + '\\n'"},
	{"string vazia escreve só '\\n'",
		"empty string writes only '\\n'"},
	{"string que já tem '\\n' no fim",
		"string that already has '\\n' at the end"},
	{"erro: malloc falhou dentro dos testes\n",
		"error: malloc failed inside the tests\n"},
	{"devolve um nó (não NULL)",
		"returns a node (not NULL)"},
	{"content guarda o ponteiro recebido",
		"content stores the received pointer"},
	{"next é NULL",
		"next is NULL"},
	{"content com uma string",
		"content with a string"},
	{"cada chamada devolve um nó diferente",
		"each call returns a different node"},
	{"content NULL é aceite (next também NULL)",
		"NULL content is accepted (next is also NULL)"},
	{"lista vazia: o novo nó passa a ser a cabeça",
		"empty list: the new node becomes the head"},
	{"a cabeça passa a ser o novo nó",
		"the head becomes the new node"},
	{"o novo nó aponta para a antiga cabeça",
		"the new node points to the old head"},
	{"3 inserções: ordem c -> b -> a",
		"3 insertions: order c -> b -> a"},
	{"tamanho final = 3",
		"final size = 3"},
	{"lista vazia (NULL)",
		"empty list (NULL)"},
	{"1 nó",
		"1 node"},
	{"2 nós",
		"2 nodes"},
	{"5 nós",
		"5 nodes"},
	{"100 nós",
		"100 nodes"},
	{"lista vazia (NULL) devolve NULL",
		"empty list (NULL) returns NULL"},
	{"1 nó: devolve esse nó",
		"1 node: returns that node"},
	{"2 nós: devolve o segundo",
		"2 nodes: returns the second"},
	{"5 nós: devolve o último",
		"5 nodes: returns the last"},
	{"o último nó tem next == NULL",
		"the last node has next == NULL"},
	{"a lista não é alterada",
		"the list is not modified"},
	{"a cabeça mantém-se",
		"the head stays the same"},
	{"o novo nó fica depois do último",
		"the new node goes after the last"},
	{"3 inserções: ordem a -> b -> c",
		"3 insertions: order a -> b -> c"},
	{"lista com 5 nós: novo nó fica no fim",
		"list with 5 nodes: new node goes at the end"},
	{"chama del exatamente 1 vez",
		"calls del exactly 1 time"},
	{"del recebe o content do nó",
		"del receives the content of the node"},
	{"não apaga o nó seguinte",
		"does not delete the next node"},
	{"nó isolado: del recebe o content certo",
		"single node: del receives the right content"},
	{"4 nós: del chamada 1 vez por cada content",
		"4 nodes: del called once per content"},
	{"o ponteiro da lista fica NULL",
		"the list pointer becomes NULL"},
	{"1 nó: del chamada 1 vez e lista NULL",
		"1 node: del called once and list NULL"},
	{"lista vazia: del não é chamada, lista continua NULL",
		"empty list: del is not called, list stays NULL"},
	{"conteúdos em heap: del (free) chamada 3 vezes",
		"heap contents: del (free) called 3 times"},
	{"f aplicada ao content de cada nó",
		"f applied to the content of each node"},
	{"f chamada 1 vez por nó",
		"f called once per node"},
	{"percorre do 1º ao último nó (por ordem)",
		"walks from the 1st to the last node (in order)"},
	{"a lista não é alterada (continua com 3 nós)",
		"the list is not modified (still has 3 nodes)"},
	{"lista vazia: f não é chamada",
		"empty list: f is not called"},
	{"1 nó: f chamada 1 vez",
		"1 node: f called once"},
	{"devolve uma lista (não NULL)",
		"returns a list (not NULL)"},
	{"nova lista com 4 nós e valores 2, 4, 6, 8 (por ordem)",
		"new list with 4 nodes and values 2, 4, 6, 8 (in order)"},
	{"del não é chamada quando tudo corre bem",
		"del is not called when everything goes well"},
	{"a lista original não é alterada",
		"the original list is not modified"},
	{"os nós e contents da nova lista são independentes",
		"the nodes and contents of the new list are independent"},
	{"lista vazia devolve NULL (f não é chamada)",
		"empty list returns NULL (f is not called)"},
	{"1 nó: nova lista com 1 nó (valor 2)",
		"1 node: new list with 1 node (value 2)"},
	{"\nFicheiros .c em falta na Parte %d (só informativo):\n",
		"\nMissing .c files in Part %d (informational only):\n"},
	{"\nVerificação de ficheiros: nenhum .c em falta na Parte %d.\n",
		"\nFile check: no .c file missing in Part %d.\n"},
	{"TESTES DA LIBFT (42 Lisboa)",
		"LIBFT TESTS (42 Lisbon)"},
	{"RESUMO",
		"SUMMARY"},
	{"Total de testes:",
		"Total tests:"},
	{"Passaram:",
		"Passed:"},
	{"Falharam:",
		"Failed:"},
	{"Funções com erros:",
		"Functions with errors:"},
	{"Todos os testes passaram!",
		"All tests passed!"},
	{"Há testes a falhar. Vê as linhas [ERRO] acima.",
		"Some tests failed. Check the [FAIL] lines above."},
	{NULL, NULL}
};



static const char	*tr(const char *pt)
{
	size_t	i;

	if (!g_en)
		return (pt);
	for (i = 0; g_tr[i].pt != NULL; i++)
	{
		if (strcmp(g_tr[i].pt, pt) == 0)
			return (g_tr[i].en);
	}
	return (pt);
}






static int	file_exists(const char *path)
{
	FILE	*f;

	f = fopen(path, "r");
	if (!f)
		return (0);
	fclose(f);
	return (1);
}


static int	has_source(const char *name)
{
	char	path[256];

	snprintf(path, sizeof(path), "%s.c", name);
	if (file_exists(path))
		return (1);
	snprintf(path, sizeof(path), "srcs/%s.c", name);
	if (file_exists(path))
		return (1);
	snprintf(path, sizeof(path), "src/%s.c", name);
	if (file_exists(path))
		return (1);
	return (0);
}


static void	check_missing(int part)
{
	const char	*part1[] = {"ft_isalpha", "ft_isdigit", "ft_isalnum",
		"ft_isascii", "ft_isprint", "ft_strlen", "ft_memset", "ft_bzero",
		"ft_memcpy", "ft_memmove", "ft_strlcpy", "ft_strlcat", "ft_toupper",
		"ft_tolower", "ft_strchr", "ft_strrchr", "ft_strncmp", "ft_memchr",
		"ft_memcmp", "ft_strnstr", "ft_atoi", "ft_calloc", "ft_strdup", NULL};
	const char	*part2[] = {"ft_substr", "ft_strjoin", "ft_strtrim", "ft_split",
		"ft_itoa", "ft_strmapi", "ft_striteri", "ft_putchar_fd",
		"ft_putstr_fd", "ft_putendl_fd", "ft_putnbr_fd", NULL};
	const char	*part3[] = {"ft_lstnew", "ft_lstadd_front", "ft_lstsize",
		"ft_lstlast", "ft_lstadd_back", "ft_lstdelone", "ft_lstclear",
		"ft_lstiter", "ft_lstmap", NULL};
	const char	**list;
	int			i;
	int			missing;

	list = part1;
	if (part == 2)
		list = part2;
	if (part == 3)
		list = part3;
	missing = 0;
	for (i = 0; list[i]; i++)
	{
		if (!has_source(list[i]))
		{
			if (!missing)
				out_printf(tr("\nFicheiros .c em falta na Parte %d "
						"(só informativo):\n"), part);
			out_printf("  - %s.c\n", list[i]);
			missing++;
		}
	}
	if (!missing)
		out_printf(tr("\nVerificação de ficheiros: nenhum .c em falta na "
				"Parte %d.\n"), part);
}

static void	run_part1(void)
{
	test_char_func("ft_isalpha", ft_isalpha, ref_isalpha);
	test_char_func("ft_isdigit", ft_isdigit, ref_isdigit);
	test_char_func("ft_isalnum", ft_isalnum, ref_isalnum);
	test_char_func("ft_isascii", ft_isascii, ref_isascii);
	test_char_func("ft_isprint", ft_isprint, ref_isprint);
	test_memset();
	test_bzero();
	test_memcpy();
	test_memmove();
	test_memchr();
	test_memcmp();
	test_strlen();
	test_strlcpy();
	test_strlcat();
	test_case_func("ft_toupper", ft_toupper, ref_toupper, 'a', 'z');
	test_case_func("ft_tolower", ft_tolower, ref_tolower, 'A', 'Z');
	test_strchr();
	test_strrchr();
	test_strncmp();
	test_strnstr();
	test_atoi();
	test_calloc();
	test_strdup();
}

static void	run_part2(void)
{
	test_substr();
	test_strjoin();
	test_strtrim();
	test_split();
	test_itoa();
	test_strmapi();
	test_striteri();
	test_putchar_fd();
	test_putstr_fd();
	test_putendl_fd();
	test_putnbr_fd();
}

static void	run_part3(void)
{
	test_lstnew();
	test_lstadd_front();
	test_lstsize();
	test_lstlast();
	test_lstadd_back();
	test_lstdelone();
	test_lstclear();
	test_lstiter();
	test_lstmap();
}


static int	arg_is(const char *arg, const char *name)
{
	while (*arg == '-')
		arg++;
	return (strcmp(arg, name) == 0);
}

static void	print_usage(const char *prog)
{
	printf("Uso / Usage: %s [1|2|3|all] [pt|en] [utf8] [nocolor]\n\n", prog);
	printf("  1 | 2 | 3 | all   partes da libft / libft parts (default: all)\n");
	printf("  pt | en           idioma / language\n");
	printf("  utf8              mostra acentos / show accents "
		"(default: ASCII only)\n");
	printf("  nocolor           sem cores / no colors\n");
}


static void	ask_language(void)
{
	char	line[32];

	out_printf("Language / Idioma:  [1] English   [2] Português (pt)\n> ");
	fflush(stdout);
	if (fgets(line, sizeof(line), stdin) == NULL)
		return ;
	if (line[0] == '1' || line[0] == 'e' || line[0] == 'E')
		g_en = 1;
}

int	main(int argc, char **argv)
{
	int	part;
	int	lang_set;
	int	i;

	part = 0;
	lang_set = 0;
	g_color = (isatty(STDOUT_FILENO) && getenv("NO_COLOR") == NULL);
	for (i = 1; i < argc; i++)
	{
		if (arg_is(argv[i], "1") || arg_is(argv[i], "2")
			|| arg_is(argv[i], "3"))
			part = atoi(argv[i]);
		else if (arg_is(argv[i], "all") || arg_is(argv[i], "todos"))
			part = 0;
		else if (arg_is(argv[i], "pt") || arg_is(argv[i], "en"))
		{
			g_en = arg_is(argv[i], "en");
			lang_set = 1;
		}
		else if (arg_is(argv[i], "utf8"))
			g_utf8 = 1;
		else if (arg_is(argv[i], "nocolor") || arg_is(argv[i], "no-color"))
			g_color = 0;
		else
		{
			print_usage(argv[0]);
			return (arg_is(argv[i], "h") || arg_is(argv[i], "help") ? 0 : 2);
		}
	}
	if (!lang_set && isatty(STDIN_FILENO) && isatty(STDOUT_FILENO))
		ask_language();
	out_printf(BOLD "########## %s ##########" RESET "\n",
		tr("TESTES DA LIBFT (42 Lisboa)"));
	if (part == 0)
	{
		check_missing(1);
		check_missing(2);
		check_missing(3);
	}
	else
		check_missing(part);
	if (part == 0 || part == 1)
		run_part1();
	if (part == 0 || part == 2)
		run_part2();
	if (part == 0 || part == 3)
		run_part3();
	out_printf("\n" BOLD "==================== %s ====================" RESET "\n",
		tr("RESUMO"));
	out_printf("%-18s%d\n", tr("Total de testes:"), g_total);
	out_printf("%-18s" GREEN "%d" RESET "\n", tr("Passaram:"), g_ok);
	if (g_fail == 0)
		out_printf("%-18s%d\n", tr("Falharam:"), g_fail);
	else
		out_printf("%-18s" RED "%d" RESET "\n", tr("Falharam:"), g_fail);
	if (g_failed_func_count > 0)
	{
		int j;

		out_printf("\n" RED "%s" RESET "\n", tr("Funções com erros:"));
		j = 0;
		while (j < g_failed_func_count)
		{
			out_printf("  - %s\n", g_failed_funcs[j]);
			j++;
		}
	}
	if (g_fail == 0)
		out_printf("\n" GREEN "%s" RESET "\n", tr("Todos os testes passaram!"));
	else
		out_printf("\n" RED "%s" RESET "\n",
			tr("Há testes a falhar. Vê as linhas [ERRO] acima."));
	return (g_fail == 0 ? 0 : 1);
}