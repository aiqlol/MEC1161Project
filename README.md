# Part 1: model and solvers (Aqil)

## Files

| File | What it holds |
|---|---|
| `msd.h` | Shared header: `Params`, `State`, all function names. Send this to the team. |
| `model.c` | `accel()`, `step_euler()`, `step_rk4()`, `simulate()`, helpers (`num_steps`, `natural_frequency`, `damping_ratio`, `energy`, `exact_undamped`) |
| `solver_test.c` | Test program: Euler vs RK4 vs exact, energy check, step-size study. Writes two CSV files. |
| `plot_part1.gp` | Gnuplot script: makes four PNG plots from the CSV files |

## Build and run in Code::Blocks

1. File → New → Project → Console application → C. Name it `part1_test`.
2. Delete the `main.c` that Code::Blocks made (right-click → Remove file from project). A project can have only one `main()`, and it is in `solver_test.c`.
3. Copy `msd.h`, `model.c` and `solver_test.c` into the project folder. Right-click the project → Add files → select all three.
4. Project → Build options → Compiler flags → tick `-std=c99` and `-Wall`.
5. Press F9 (Build and run). The CSV files appear in the project folder.

If you see `undefined reference to 'sin'` (or `sqrt`, `cos`): Project → Build options → Linker settings → Add → type `m`.

Command line instead:

```
gcc -std=c99 -Wall -Wextra -o solver_test solver_test.c model.c -lm
./solver_test
```

## Expected output (key lines)

```
  wn = 4.4721 rad/s, period T = 1.4050 s, zeta = 0.000
   10.0    0.73910    2.06065    0.73910    1.32e+00    9.62e-07
  Energy at t = 10 s:  Euler 73.743124 J (+637.43%), RK4 9.999999 J (-0.000011%)
    0.010       1.675e+00     1.415e-06          1.88        4.00
    0.001       1.034e-01     1.414e-10          1.07        4.00
```

On Windows, exponents can show 3 digits (`1.32e+000`). That is the same number.

## Make the plots

Install Gnuplot (tick "add to PATH"), then in the project folder:

```
gnuplot plot_part1.gp
```

If it says `unknown or ambiguous terminal type`, change `pngcairo` to `png` in line 16 of the script.

## Results for the report

1. **RK4 is correct.** With dt = 0.01 s, RK4 stays within 1.4 × 10⁻⁶ m of the exact solution for 10 s (7 oscillations).
2. **Euler is unstable for an undamped oscillator.** The amplitude grows from 1 m to about 2.7 m, and the energy grows from 10 J to 74 J (+637%), although no energy goes in. The cause: each Euler step multiplies the energy by (1 + ωn²·dt²) = 1.002, and 1.002¹⁰⁰⁰ ≈ 7.4.
3. **RK4 conserves energy:** −0.00001% after 10 s.
4. **Order of accuracy.** Make dt 10 times smaller: the RK4 error becomes 10,000 times smaller (observed order 4.00, the theory value). The Euler error tends to order 1 for small dt; for large dt its error grows faster, because the amplitude growth compounds.
5. **Damped and forced cases also checked** (RK4, dt = 0.01 s): critical damping (c = 8.944) gives no overshoot; the forced steady-state amplitude matches F0 / √((k − mω²)² + (cω)²) to 4 decimal places (0.2481 m at ω = 2, 0.8944 m at ω = ωn).

These are your "exploration beyond class" items for Task 3c.

## Tell the team

- `msd.h` now has helper functions: `num_steps`, `natural_frequency`, `damping_ratio`, `energy`, `exact_undamped`. Nothing that was already agreed has changed.
- Part 3 runs a case like this:

```c
Params p = {1.0, 1.0, 20.0, 0.0, 0.0, 1.0, 0.0, 0.01, 10.0};  /* underdamped */
int n = num_steps(&p) + 1;
State *data = malloc((size_t)n * sizeof(State));
simulate(data, n, &p, step_rk4);      /* or step_euler */
/* ... analyse, write_csv("underdamped.csv", data, n), plot ... */
free(data);
```

- Part 3 / Part 4 still write `write_csv()` and `plot_gnuplot()`.
