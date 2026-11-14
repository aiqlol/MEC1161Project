# MEC1161 Mass–Spring–Damper Simulation

This version integrates the existing Parts 1 and 2 solver with the Part 3 data-management module. It keeps the existing `Params`, `State`, `accel()`, `step_euler()`, `step_rk4()`, `num_steps()` and `simulate()` interfaces.

## Project files

| File | Responsibility |
|---|---|
| `main.c` | Reads inputs, selects the numerical method, runs the solver and coordinates the data module. |
| `model.c` | Existing mathematical model and Euler/RK4 simulation functions. |
| `msd.h` | Existing shared `Params` and `State` definitions and model function declarations. |
| `data.h` | Part 3 data structure and function declarations. |
| `data.c` | Part 3 dynamic allocation, result printing, CSV export and read-only data access. |
| `.vscode/tasks.json` | VS Code build task for GCC on Windows. |
| `Project_MassSpringDamper_Parts1and2.cbp` | Updated Code::Blocks project file, retained for teammates who use Code::Blocks. |

## Part 3 data structure

`State` in `msd.h` continues to represent one time step:

- `t`: time in seconds
- `x`: displacement in metres
- `v`: velocity in metres per second
- `a`: acceleration in metres per second squared

`SimulationData` in `data.h` owns a dynamically allocated array of `State` records and the number of records stored. This avoids duplicating the solver's existing `State` structure and provides one common data interface for later output/visualisation work.

The Part 3 interface is:

- `simulation_data_init()` — initialise an empty collection.
- `simulation_data_allocate()` — allocate the required number of result records.
- `simulation_data_print()` — print the time-series table.
- `simulation_data_export_csv()` — write the result data to CSV.
- `simulation_data_get()` — obtain a read-only pointer to an individual record.
- `simulation_data_free()` — release the allocated memory.

## Build and run in VS Code (Windows)

1. Install a GCC toolchain such as MinGW-w64 and ensure `gcc` is available in the PATH.
2. Open this project folder in VS Code.
3. Open **Terminal → New Terminal**.
4. Check GCC is available:

   ```powershell
   gcc --version
   ```

5. Build the project:

   ```powershell
   gcc -std=c99 -Wall -Wextra -Wpedantic -Werror main.c model.c data.c -lm -o msd.exe
   ```

6. Run it:

   ```powershell
   .\msd.exe
   ```

Alternatively, press **Ctrl+Shift+B** and select **Build Mass-Spring-Damper (GCC)**. The task in `.vscode/tasks.json` runs the same build command.

For Linux/macOS, build with:

```bash
gcc -std=c99 -Wall -Wextra -Wpedantic -Werror main.c model.c data.c -lm -o msd
./msd
```

## Output file for Part 4 visualisation

After a successful run, the program writes `simulation_results.csv` in the project root. The column names are a stable interface for the visualisation module:

```csv
time_s,displacement_m,velocity_m_per_s,acceleration_m_per_s2
```

Each subsequent row contains one simulation time step. The file contains numeric data only, apart from the header row, so it can be read by Gnuplot or another plotting tool.

Example Gnuplot commands (for the Part 4 teammate):

```gnuplot
set datafile separator ","
set xlabel "Time (s)"
set ylabel "Displacement (m)"
set title "Displacement vs Time"
plot "simulation_results.csv" every ::1 using 1:2 with lines title "Displacement"
```

To plot velocity or acceleration, change the y-column to 3 or 4 respectively. `every ::1` skips the CSV header.

## Compatibility notes

- The original `State` definition and solver function signatures in `msd.h` are unchanged.
- The existing solver in `model.c` is unchanged.
- The new `data.h` includes `msd.h`, so visualisation code can use the same `State` records without defining a second, incompatible data structure.
- The CSV column names and order should not be changed without coordinating with the Part 4 visualisation teammate.
- The current `main.c` retains the original group's current behaviour of setting forcing amplitude and frequency to zero. User inputs for forced vibration are not added in this Part 3 integration.

## Current scope

This is the fundamentals-first Part 3 integration: dynamically store the solver's results, print the records, and export a visualisation-ready CSV file. Sorting, searching, peak analysis, energy analysis and settling-time calculations are intentionally left out for now.
