#include "../includes/philo.h"

void    exit_philo(void)
{
    printf("./philo [philos] [die_ms] [eat_ms] [sleep_ms] *opt:[no_of_eat]\n");
    exit(1);
}

int check_args(int argc, char **argv)
{
    int  i;

    if (argc < 5 || argc > 6)
        exit_philo();
    printf("here?\n");
}

int main(int argc, char **argv)
{
    //t_arg   args;

    if (check_args(argc, argv))
        return (1);
}