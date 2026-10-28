#include "main.h"

int	ft_strcmp(const char *s1, const char *s2){
	int	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char *get_next_line(int fd){
    static char stash[BUFFER_SIZE * 4];
    static int len_stach = 0;

    char buffer[BUFFER_SIZE + 1];
    int nb_carac;
    char *newline_pos;
    char *line;
    int len_ligne;

    while((newline_pos = ft_memchr(stash, '\n', len_stach)) == NULL){
        nb_carac = read(fd, buffer, BUFFER_SIZE);
        if (nb_carac <= 0){
            if (len_stach == 0)
                return NULL;
            break;
        }
        ft_memcpy(stash + len_stach, buffer, nb_carac);
        len_stach += nb_carac;
    }

    if(len_stach == 0)
        return NULL;

    if(newline_pos != NULL)
        len_ligne = newline_pos - stash + 1;
    else
        len_ligne = len_stach;

    line = malloc(len_ligne + 1);
    if(!line)
        return NULL;
    
    ft_memcpy(line, stash, len_ligne);
    line[len_ligne] = '\0';

    ft_memmove(stash, stash + len_ligne, len_stach - len_ligne);
    len_stach -= len_ligne;

    return line;
}

char *ft_strdup(const char *s){
	int		i;
	int		len_src;
	char	*result;

	len_src = 0;
	i = 0;
	while (s[len_src])
		len_src++;
	result = malloc((len_src + 1) * sizeof(char));
	if (!result)
		return (NULL);
	while (s[i])
	{
		result[i] = s[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}

static void	free_tab(char **tab){
	int	i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

static int	t_array(char const *str, char c){
	unsigned int	nb;
	int				i;

	nb = 0;
	i = 0;
	if (str[0] != c && str[0] != '\0')
		nb++;
	while (str[i])
	{
		if (str[i] == c && str[i + 1] && str[i + 1] != c)
			nb++;
		i++;
	}
	return (nb);
}

static int	t_mot(unsigned int index_d, const char *str, char set){
	int	t_chaine;

	t_chaine = 0;
	while (str[index_d] != set && str[index_d])
	{
		t_chaine++;
		index_d++;
	}
	return (t_chaine);
}

static char	**insert(char **tab, const char *str, char c){
	int	i;
	int	p_chaine;
	int	n_array;

	n_array = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] != c)
		{
			p_chaine = 0;
			while (str[i] != c && str[i])
			{
				tab[n_array][p_chaine] = str[i];
				i++;
				p_chaine++;
			}
			i--;
			tab[n_array][p_chaine] = '\0';
			n_array++;
		}
		i++;
	}
	return (tab);
}

char **ft_split(char const *s, char c){
	char	**tab;
	int		i;
	int		n_array;

	if (!s)
		return (0);
	n_array = 0;
	i = 0;
	tab = malloc(sizeof(char *) * (t_array(s, c) + 1));
	if (!tab)
		return (NULL);
	while (s[i])
	{
		if (s[i] != c && s[i])
		{
			tab[n_array] = malloc(sizeof(char) * t_mot(i, s, c) + 1);
			if (!tab[n_array])
				return (free_tab(tab), NULL);
			i = i + t_mot(i, s, c) - 1;
			n_array++;
		}
		i++;
	}
	tab[t_array(s, c)] = 0;
	return (insert(tab, s, c));
}


int	ft_atoi(const char *nptr){
	unsigned int	i;
	unsigned int	compteur;
	int				signe;
	int				valeur;

	compteur = 0;
	valeur = 0;
	i = 0;
	signe = 1;
	while(nptr[i] && (nptr[i] == ' ' || nptr[i] == '\f' || nptr[i] == '\n' || nptr[i] == '\r' || nptr[i] == '\t' || nptr[i] == '\v'))
		i++;
	if(nptr[i] == '-' || nptr[i] == '+'){
		if (nptr[i] == '-')
			signe = -1;
		i++;
	}
	while(nptr[i] && nptr[i] >= '0' && nptr[i] <= '9'){
		valeur = valeur * 10 + (nptr[i] - '0');
		i++;
	}
	return (signe * valeur);
}

void *ft_calloc(size_t nmemb, size_t size){
	size_t	i;
	char	*c;

	if (nmemb == 0 || size == 0){
		nmemb = 1;
		size = 1;
	}
	if (nmemb > 4294967296 / size)
		return (NULL);
	c = malloc(nmemb * size);
	if (!c)
		return (NULL);
	i = 0;
	while (i < nmemb * size){
		c[i] = 0;
		i++;
	}
	return ((void *)c);
}

void *ft_memchr(const void *s, int c, size_t n){
	unsigned char	*t;
	unsigned char	p;
	size_t			i;

	p = c;
	i = 0;
	if (n == 0)
		return (0);
	while (i < n){
		t = (unsigned char *)s;
		if (t[i] == p)
			return ((void *) &t[i]);
		i++;
	}
	return (0);
}

void *ft_memcpy(void *dest, const void *src, size_t n){
	char	*d;
	char	*s;
	size_t	i;

	if (!dest && !src)
		return (0);
	i = 0;
	d = (char *)dest;
	s = (char *)src;
	while (i < n){
		d[i] = s[i];
		i++;
	}
	return (dest);
}

void *ft_memmove(void *dest, const void *src, size_t n){
	unsigned char	*s;
	unsigned char	*d;
	size_t			i;	

	s = (unsigned char *)src;
	d = (unsigned char *)dest;
	if (!dest && !src)
		return (0);
	i = 0;
	if (d < s){
		while (i < n){
			d[i] = s[i];
			i++;
		}
	}
	else{	
		while (n-- > 0)
			d[n] = s[n];
	}
	return (dest);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n){
	size_t			i;
	unsigned int	result;

	if (n == 0)
		return (0);
	i = 0;
	while (i < n - 1 && (unsigned char)s1[i] && (unsigned char)s2[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
		i++;
	result = (unsigned char)s1[i] - (unsigned char)s2[i];
	return (result);
}

char *ft_strchr(const char *s, int c){
	char	p;
	int		i;

	p = c;
	i = 0;
	while (s[i])
	{
		if (s[i] == p)
		{
			return ((char *)&s[i]);
		}
		i++;
	}
	if (s[i] == c)
		return ((char *)&s[i]);
	return (0);
}

size_t ft_strlcpy(char	*dest, const char *src, size_t size){
	size_t	i;
	size_t	t_src;

	t_src = 0;
	while (src[t_src])
		t_src++;
	if (!size)
		return (t_src);
	i = 0;
	while (src[i] && i < size - 1)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (t_src);
}

void free_split(char **split)
{
    int i = 0;

    if (!split)
        return;
    while (split[i])
    {
        free(split[i]);
        i++;
    }
    free(split);
}