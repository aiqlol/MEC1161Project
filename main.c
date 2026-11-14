#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "msd.h"

#define MAX_SIMULATION_STEPS 10000000.0
#define DEFAULT_CSV_FILENAME "simulation_results.csv"
#define INPUT_BUFFER_SIZE 128

static int read_double(const char *prompt, double *value)
{
    char line[INPUT_BUFFER_SIZE];

    for (;;)
    {
        char *start;
        char *end;
        double parsed;

        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            return 0;
        }

        start = line;
        while (isspace((unsigned char)*start))
        {
            start++;
        }

        if (*start == '\0')
        {
            printf(" [Error] Please enter a numeric value.\n");
            continue;
        }

        errno = 0;
        parsed = strtod(start, &end);
        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (start == end || *end != '\0' || errno == ERANGE || !isfinite(parsed))
        {
            printf(" [Error] Invalid input. Please enter a finite numeric value.\n");
            continue;
        }

        *value = parsed;
        return 1;
    }
}

static int read_int(const char *prompt, int *value)
{
    char line[INPUT_BUFFER_SIZE];

    for (;;)
    {
        char *start;
        char *end;
        long parsed;

        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            return 0;
        }

        start = line;
        while (isspace((unsigned char)*start))
        {
            start++;
        }

        if (*start == '\0')
        {
            printf(" [Error] Please enter an integer.\n");
            continue;
        }

        errno = 0;
        parsed = strtol(start, &end, 10);
        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (start == end || *end != '\0' || errno == ERANGE ||
            parsed < INT_MIN || parsed > INT_MAX)
        {
            printf(" [Error] Invalid input. Please enter an integer.\n");
            continue;
        }

        *value = (int)parsed;
        return 1;
    }
}

int main(void)
{
    double m;
    double c;
    double k;
    double x0;
    double v0;
    double t_total;
    double dt;
    double estimated_steps;
    int choice;
    int steps;
    Params params;
    SimulationData results;
    void (*step_function)(State *, const Params *);
    const char *method_name;

    simulation_data_init(&results);

    printf("======= Mass-Spring-Damper Simulation =======\n\n");
    printf("1. System parameters\n");

    if (!read_double("Enter mass (m in kg): ", &m) || m <= 0.0)
    {
        printf("[Error] Mass must be a finite value greater than 0.\n");
        return 1;
    }

    if (!read_double("Enter damping coefficient (c in N*s/m): ", &c) || c < 0.0)
    {
        printf("[Error] Damping must be a finite value greater than or equal to 0.\n");
        return 1;
    }

    if (!read_double("Enter spring stiffness (k in N/m): ", &k) || k <= 0.0)
    {
        printf("[Error] Stiffness must be a finite value greater than 0.\n");
        return 1;
    }

    printf("\n2. Initial conditions\n");
    if (!read_double("Enter initial displacement (x0 in m): ", &x0))
    {
        printf("[Error] Unable to read initial displacement.\n");
        return 1;
    }

    if (!read_double("Enter initial velocity (v0 in m/s): ", &v0))
    {
        printf("[Error] Unable to read initial velocity.\n");
        return 1;
    }

    printf("\n3. Simulation time parameters\n");
    if (!read_double("Enter total simulation time (t_total in s): ", &t_total) || t_total <= 0.0)
    {
        printf("[Error] Total simulation time must be greater than 0.\n");
        return 1;
    }

    if (!read_double("Enter time step size (dt in s): ", &dt) || dt <= 0.0 || dt > t_total)
    {
        printf("[Error] dt must be greater than 0 and no larger than t_total.\n");
        return 1;
    }

    estimated_steps = t_total / dt;
    if (!isfinite(estimated_steps) || estimated_steps > MAX_SIMULATION_STEPS)
    {
        printf("[Error] Too many simulation steps. Use a larger dt or shorter duration.\n");
        return 1;
    }

    printf("\n4. Numerical method\n");
    printf("1. Forward Euler's Method\n");
    printf("2. Runge-Kutta 4th Order Method (RK4)\n");
    printf("3. Recommended Method (RK4)\n");

    if (!read_int("Enter choice (1-3): ", &choice) || choice < 1 || choice > 3)
    {
        printf("[Error] Please select 1, 2 or 3.\n");
        return 1;
    }

    params.m = m;
    params.c = c;
    params.k = k;
    params.F0 = 0.0;
    params.w = 0.0;
    params.x0 = x0;
    params.v0 = v0;
    params.dt = dt;
    params.t_total = t_total;

    steps = num_steps(&params);
    if (steps < 1 || steps >= INT_MAX)
    {
        printf("[Error] The calculated number of simulation steps is invalid.\n");
        return 1;
    }

    if (!simulation_data_allocate(&results, (size_t)steps + 1U))
    {
        printf("[Error] Unable to allocate memory for simulation results.\n");
        simulation_data_free(&results);
        return 1;
    }

    if (choice == 1)
    {
        step_function = step_euler;
        method_name = "Forward Euler";
    }
    else
    {
        step_function = step_rk4;
        method_name = "Runge-Kutta 4th Order (RK4)";
    }

    printf("\nRunning simulation using %s...\n", method_name);
    simulate(results.states, (int)results.count, &params, step_function);

    if (!simulation_data_print(&results, stdout))
    {
        printf("[Error] Unable to print simulation results.\n");
        simulation_data_free(&results);
        return 1;
    }

    if (!simulation_data_export_csv(&results, DEFAULT_CSV_FILENAME))
    {
        printf("[Error] Simulation completed, but CSV export failed.\n");
        simulation_data_free(&results);
        return 1;
    }

    printf("\nSimulation completed successfully.\n");
    printf("%lu data points stored.\n", (unsigned long)results.count);
    printf("Results exported to %s.\n", DEFAULT_CSV_FILENAME);

    simulation_data_free(&results);
    return 0;
}
