*This project has been created as part of the 42 curriculum by tvithara.*

# Philosophers – 42 Project

## Description

This project is an implementation of the classic **Dining Philosophers Problem**, a well-known synchronization problem in computer science.

The goal of the project is to explore:

- Thread creation and management using `pthread`
- Mutual exclusion using `pthread_mutex`
- Synchronization of shared resources
- Prevention of data races
- Precise time management in concurrent systems

The simulation represents philosophers sitting around a circular table. Each philosopher alternates between:

- Thinking
- Eating
- Sleeping

To eat, a philosopher must acquire **two forks**: one on the left and one on the right. Since forks are shared resources, they are protected by mutexes to prevent race conditions.

The simulation stops when:

- A philosopher dies (if they do not eat within `time_to_die` milliseconds), or
- (Optional argument) All philosophers have eaten at least a specified number of times.

This implementation follows the **fork indexing strategy**:
- Forks are assigned using their index number.
- Philosophers attempt to pick up forks in a defined order (left-to-right or right-to-left depending on position) to prevent deadlocks.

Only the **mandatory part** (threads + mutexes) has been implemented.

---

## Technical Choices

### Thread Model
- Each philosopher is represented by a separate thread.
- The main thread initializes the simulation and monitors termination conditions.

### Fork Management
- One mutex per fork.
- Forks are stored in an indexed array.
- Each philosopher knows:
  - Its left fork index
  - Its right fork index

### Deadlock Prevention Strategy
To prevent circular wait and deadlock:
- Philosophers use an ordered fork acquisition strategy.
- Forks are picked consistently either left-to-right or right-to-left based on philosopher ID.
- This prevents a situation where all philosophers hold one fork and wait forever.

### Death Monitoring
- A monitoring mechanism continuously checks:
  - Time since last meal
  - Whether a philosopher has exceeded `time_to_die`
- A dedicated mutex protects shared state related to death and printing.
- The death message is printed within the required time constraints.

### Output Protection
- All log messages are protected by a mutex.
- Messages are never interleaved.
- Format strictly follows subject requirements.

### Constraints Followed

- No global variables
- No data races
- No memory leaks
- No forbidden functions
- Norm compliant (Norm v4.1) :contentReference[oaicite:2]{index=2}
- Makefile includes required rules: `NAME`, `all`, `clean`, `fclean`, `re`

---

## Compilation

```bash
make
