*This project has been created as part of the 42 curriculum by dkhmaruk.*

# Codexion

## Description

Codexion is a multithreaded simulation written in C.

The project models several coders working concurrently and competing for a limited number of USB dongles. Each coder is represented by a POSIX thread.

To start compiling, a coder must acquire two dongles. After compiling, the coder releases both dongles, then starts debugging and refactoring before attempting to compile again.

The simulation supports two scheduling policies for deciding which coder gets access to a dongle:

- `fifo` — First In, First Out. The request that arrived first has priority.
- `edf` — Earliest Deadline First. The coder with the earliest burnout deadline has priority.

The EDF deadline is calculated as:

```text
last_compile_start + time_to_burnout
```

Each dongle also has a cooldown period. After a dongle is released, it cannot be taken again until the cooldown has elapsed.

The simulation stops when:

- a coder burns out, or
- every coder has completed the required number of compilations.

The project focuses on concurrency, thread synchronization, resource arbitration, deadlock prevention, starvation prevention, timing precision, and safe shared-state management.

## Instructions

### Compilation

Compile the project with:

```bash
make
```

or:

```bash
make all
```

The executable is:

```text
codexion
```

Other available Makefile rules:

```bash
make clean
make fclean
make re
```

### Usage

Run the program with:

```bash
./codexion number_of_coders time_to_burnout time_to_compile \
time_to_debug time_to_refactor number_of_compiles_required \
dongle_cooldown scheduler
```

All time values are expressed in milliseconds.

Arguments:

- `number_of_coders` — number of coders and dongles.
- `time_to_burnout` — maximum time a coder can go without starting a new compile.
- `time_to_compile` — duration of the compiling phase.
- `time_to_debug` — duration of the debugging phase.
- `time_to_refactor` — duration of the refactoring phase.
- `number_of_compiles_required` — number of successful compilations required for every coder.
- `dongle_cooldown` — time during which a released dongle cannot be reused.
- `scheduler` — scheduling policy. Must be exactly `fifo` or `edf`.

Example:

```bash
./codexion 5 800 200 200 200 5 50 fifo
```

EDF example:

```bash
./codexion 5 800 200 200 200 5 50 edf
```

### Output

Coder state changes are displayed using the following format:

```text
timestamp coder_id has taken a dongle
timestamp coder_id is compiling
timestamp coder_id is debugging
timestamp coder_id is refactoring
timestamp coder_id burned out
```

Example:

```text
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
```

## Scheduling

### FIFO

FIFO stands for First In, First Out.

Each request stores its arrival time. The request with the earliest arrival time has the highest priority.

If two requests have equal priority, the coder ID is used as a deterministic tie-breaker.

### EDF

EDF stands for Earliest Deadline First.

Each request receives a deadline:

```text
deadline = last_compile_start + time_to_burnout
```

The request with the earliest deadline has the highest priority.

If two deadlines are equal, the coder with the lower ID has priority.

### Priority queue

Each dongle maintains its own waiting queue implemented as a binary heap.

The heap supports operations such as:

```text
heap_push
heap_pop
heap_peek
heap_remove
```

The comparison logic changes depending on whether the selected scheduler is FIFO or EDF.

## Blocking cases handled

### Deadlock prevention

A coder needs two dongles simultaneously to compile.

To avoid circular wait, dongles are acquired in a deterministic order based on their IDs. The dongle with the lower ID is requested first and the one with the higher ID second.

This prevents a circular dependency where every coder holds one dongle while waiting permanently for another one.

### Starvation prevention

Every dongle maintains a priority queue of waiting requests.

Access is not granted simply to whichever thread reaches the mutex first. Instead, the next coder is selected according to the configured scheduler:

```text
FIFO -> earliest request
EDF  -> earliest burnout deadline
```

This provides deterministic arbitration of competing requests.

### Dongle cooldown

After a dongle is released, its next usable time is stored in:

```text
available_at
```

The value is calculated as:

```text
current_time + dongle_cooldown
```

A coder may acquire the dongle only when:

```text
current_time >= available_at
```

`pthread_cond_timedwait()` is used when the coder has priority but must still wait for the cooldown to expire.

### Burnout detection

A dedicated monitor thread periodically checks every coder.

The burnout condition is based on:

```text
current_time - last_compile_start >= time_to_burnout
```

`last_compile_start` is protected by the coder's state mutex.

When a burnout is detected, the simulation is stopped and the burnout message is printed through the serialized logging mechanism.

### One coder case

When there is only one coder, there is only one dongle.

The coder can acquire that dongle, but cannot start compiling because compiling requires two dongles.

The coder therefore eventually burns out.

### Simulation shutdown

When the simulation stops, all threads waiting on dongle condition variables are woken with `pthread_cond_broadcast()`.

This prevents threads from remaining blocked inside `pthread_cond_wait()` after the simulation has already ended.

Before resources are destroyed, the main thread waits for worker threads using `pthread_join()`.

## Thread synchronization mechanisms

### POSIX threads

Each coder runs in its own thread created with:

```c
pthread_create()
```

A separate monitor thread is responsible for checking burnout and the completion condition.

The main thread waits for created threads using:

```c
pthread_join()
```

### Dongle mutex

Each dongle contains its own:

```c
pthread_mutex_t mutex;
```

This mutex protects the shared dongle state, including:

```text
available
available_at
waiting heap
```

Only one thread may modify or inspect this critical state at a time.

### Coder state mutex

Each coder contains:

```c
pthread_mutex_t state_mutex;
```

It protects shared coder state such as:

```text
last_compile_start
compile_count
```

The coder thread updates these values while the monitor thread reads them.

### Stop mutex

The global simulation stop flag:

```text
sim->stopped
```

is protected by:

```c
pthread_mutex_t stop_mutex;
```

Helper functions are used to read and update the stop state safely.

### Print mutex

All logging is protected by:

```c
pthread_mutex_t print_mutex;
```

This prevents output produced by different threads from being interleaved.

It also ensures that normal state messages are not printed after the simulation has already been stopped because of burnout.

### Condition variables

Each dongle contains:

```c
pthread_cond_t cond;
```

A coder that cannot currently acquire a dongle waits using:

```c
pthread_cond_wait()
```

or:

```c
pthread_cond_timedwait()
```

This avoids continuously polling the resource and wasting CPU time.

When a dongle is released or the simulation is stopped, `pthread_cond_broadcast()` wakes waiting threads so they can re-check their conditions.

## Project architecture

The project is divided into several modules:

```text
main.c
parsing.c
init.c
cleanup.c
coder.c
dongle.c
heap.c
monitor.c
logging.c
simulation.c
time.c
utils.c
codexion.h
Makefile
README.md
```

Main responsibilities:

```text
parsing.c     argument validation and configuration
init.c        initialization of simulation structures
cleanup.c     destruction and memory cleanup
coder.c       coder thread behaviour
dongle.c      dongle acquisition and release
heap.c        FIFO/EDF priority queue
monitor.c     burnout and completion monitoring
logging.c     synchronized output
simulation.c  simulation startup and thread coordination
time.c        time utilities and controlled sleeping
utils.c       shared helper functions
```

## Resources

Resources used while working on the project:

- POSIX Threads documentation (`pthread`)
- Linux manual pages for:
  - `pthread_create`
  - `pthread_join`
  - `pthread_mutex_*`
  - `pthread_cond_*`
  - `gettimeofday`
  - `usleep`
- Documentation and educational material about:
  - POSIX threads
  - mutexes and condition variables
  - race conditions
  - deadlocks
  - binary heaps and priority queues
  - FIFO scheduling
  - Earliest Deadline First scheduling

### AI usage

AI was used as a learning during the project.

It was used for:

- explaining POSIX thread concepts such as threads, mutexes, condition variables, `pthread_join()`, and timed waits;
- discussing the architecture of the simulation;
- explaining FIFO and EDF scheduling;
- reviewing code fragments for concurrency and memory-management issues;
- identifying implementation mistakes during testing;
- suggesting edge cases and test scenarios;
