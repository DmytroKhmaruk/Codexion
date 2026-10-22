/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   main.c                                            :+:      :+:    :+:    */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/10/21 16:07:42 by username         #+#    #+#              */
/*   Updated: 2026/10/22 11:12:46 by username        ###   ########.fr        */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config	config;
	t_sim		sim;

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
