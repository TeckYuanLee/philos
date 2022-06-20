#include "../includes/philo.h"

int check_args(int argc, char **argv)
{
    int  i;

    if (argc < 5 || argc > 6)
    {
        printf("./philo [philos] [die_ms] [eat_ms] [sleep_ms] *opt:[no_of_eat]\n");
        exit(1);
    }
    printf("here?\n");
    i = 0;
    while (argv[++i])
    {
        if (!ft_atoi(argv[i]))
        {
            printf("Invalid argument\n");
            exit(1);
        }
    }
    return (0);
}

int main(int argc, char **argv)
{
    t_arg   args;

    if (check_args(argc, argv))
        return (1);
    if (ft_init_args(&args, argc,argv)) //this weird, no return 1
        return (1);
    ft_init_mutex(&args);
	ft_init_philosophers(&args);
	ft_init_threads(&args);
}