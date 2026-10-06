#include "codexion.h"

int main(int argc, char **argv)
{
    t_config config;
    t_sim sim;

    if (!parse_args(argc, argv, &config))
        return (1);
    if (!init_all(&sim, &config))
        return (1);
    if (!start_simulation(&sim))
    {
        destroy_sim(&sim);
        return (1);
    }
    wait_simulation(&sim);
    destroy_sim(&sim);
    return (0);
}