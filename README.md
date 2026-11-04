# Part 1: model and solvers


## Files

| File | What it holds |
|---|---|
| `msd.h` | Shared header: `Params`, `State`, all function names. Send this to the team. |
| `model.c` | `accel()`, `step_euler()`, `step_rk4()`, `simulate()`, helpers (`num_steps`, `natural_frequency`, `damping_ratio`, `energy`, `exact_undamped`) |
| `solver_test.c` | Test program: Euler vs RK4 vs exact, energy check, step-size study. Writes two CSV files. |
| `plot_part1.gp` | Gnuplot script: makes four PNG plots from the CSV files |

