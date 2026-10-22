/* ************************************************************************** */
/*                                                                            */
/*                                                       :::      ::::::::    */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                   +:+ +:+         +:+      */
/*   By: username <username@student.42tokyo.jp>    #+#  +:+       +#+         */
/*                                               +#+#+#+#+#+   +#+            */
/*   Created: 2026/09/18 11:24:09 by username         #+#    #+#              */
/*   Updated: 2026/10/22 12:46:00 by dkhmaruk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdio.h>
# include <string.h>

typedef struct s_sim		t_sim;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_request	t_request;
typedef struct s_heap		t_heap;
typedef struct s_config		t_config;

int			init_one_dongle(t_sim *sim, int i);
int			init_one_coder(t_sim *sim, int i);
void		cleanup_dongles(t_sim *sim, int count);
void		cleanup_coders(t_sim *sim, int count);
void		cleanup_sim_mutexes(t_sim *sim);
int			request_before(t_request *a, t_request *b, t_sim *sim);
void		swap_requests(t_request **a, t_request **b);
int			heap_find(t_heap *heap, t_request *request);
int			heap_should_sift_up(t_heap *heap, int i, t_sim *sim);
void		heap_sift_up(t_heap *heap, int i, t_sim *sim);
void		heap_sift_down(t_heap *heap, int i, t_sim *sim);
int			heap_best_child(t_heap *heap, int child, int best, t_sim *sim);
int			coder_cycle(t_coder *coder);
void		one_coder_routine(t_coder *coder);
int			parse_args(int argc, char **argv, t_config *config);
int			init_all(t_sim *sim, t_config *config);
void		destroy_dongles(t_sim *sim);
void		destroy_sim(t_sim *sim);
long		get_time_ms(void);
void		set_start_time(t_sim *sim);
int			is_stopped(t_sim *sim);
void		stop_sim(t_sim *sim);
void		print_status(t_coder *coder, char *message);
void		smart_sleep(long duration, t_sim *sim);
void		*coder_routine(void *arg);
int			start_coders(t_sim *sim);
void		join_coders(t_sim *sim);
void		*monitor_routine(void *arg);
void		report_burnout(t_coder *coder);
int			start_simulation(t_sim *sim);
void		wait_simulation(t_sim *sim);
int			heap_push(t_heap *heap, t_request *request, t_sim *sim);
t_request	*heap_pop(t_heap *heap, t_sim *sim);
t_request	*heap_peek(t_heap *heap);
void		release_dongle(t_dongle *dongle, long cooldown);
void		ms_to_timespec(long ms, struct timespec *ts);
int			request_dongle(t_coder *coder, t_dongle *dongle,
				t_request *request);
int			heap_remove(t_heap *heap, t_request *request, t_sim *sim);
void		wake_all_dongles(t_sim *sim);
int			take_two_dongles(t_coder *coder);
void		release_two_dongles(t_coder *coder);
t_request	*heap_best_runnable(t_heap *heap, t_sim *sim, long now);
void		prepare_pair_requests(t_coder *coder);
int			push_pair_requests(t_coder *coder);
void		remove_pair_requests(t_coder *coder);

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_config
{
	int			number_of_coders;
	long		time_to_burnout;
	long		time_to_compile;
	long		time_to_debug;
	long		time_to_refactor;
	int			number_of_compiles_required;
	long		dongle_cooldown;
	t_scheduler	scheduler;
}	t_config;

typedef struct s_request
{
	t_coder	*coder;
	long	arrival_time;
	long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	**items;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				available;
	long			available_at;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			waiting;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		tread;
	t_dongle		*left;
	t_dongle		*right;
	long			last_compile_start;
	int				compile_count;
	pthread_mutex_t	state_mutex;
	struct s_sim	*sim;
	t_request		left_request;
	t_request		right_request;
}	t_coder;

typedef struct s_sim
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	long			start_time;
	int				stopped;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	pthread_t		monitor;
}	t_sim;

#endif
