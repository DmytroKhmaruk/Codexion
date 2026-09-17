#include "codexion.h"
#include <limits.h>

int is_number(char *str)
{
    int i;
    
    if (!str || !str[0])
        return (0);
    i = 0;
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (0);
        i++;
    }
    return (1);
}
long    to_long(char *str)
{
    long    result;
    int     digit;
    int     i;

    result = 0;
    i = 0;
    while (str[i])
    {
        digit = str[i] - '0';
        if (result > (LONG_MAX - digit) / 10)
            return (-1);
        result = result * 10 + digit;
        i++;
    }
    return (result);
}

int parse_scheduler(char *str, t_config *config)
{
    if (strcmp(str, "fifo") == 0)
        config->scheduler = FIFO;
    else if (strcmp(str, "edf") == 0)
        config->scheduler = EDF;
    else
        return (0);
    return (1);
}

int get_number(char *str, long *value)
{
    if (!is_number(str))
        return (0);
    *value = to_long(str);
    if (*value == -1)
        return (0);
    return (1);
}

int    parse_args(int argc, char **argv, t_config *config)
{   
    long    value;

    if (argc != 9)  
        return (0);
    if (!get_number(argv[1], &value) || value > INT_MAX)
        return (0);
    config->number_of_coders = (int)value;
    if (!get_number(argv[2], &config->time_to_burnout))
        return (0);
    if (!get_number(argv[3], &config->time_to_compile))
        return (0);
    if (!get_number(argv[4], &config->time_to_debug))
        return (0);
    if (!get_number(argv[5], &config->time_to_refactor))
        return (0);
    if (!get_number(argv[6], &value) || value > INT_MAX)
        return (0);
    config->number_of_compiles_required = (int)value;
    if (!get_number(argv[7], &config->dongle_cooldown))
        return (0);
    if (!parse_scheduler(argv[8], config))
        return (0);
    return (1);
}


int main(int argc, char **argv)
{
    t_config    config;

    if (!parse_args(argc, argv, &config))
    {
        printf("Error: invalid arguments\n");
        return(1);
    }
    printf("Arguments are valid\n");
    return (0);
}