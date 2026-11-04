/*
 * solver_test.c - Part 1 (Aqil): check that the solvers are correct.
 *
 * Test 1  Free vibration (m = 1 kg, k = 20 N/m, c = 0, x0 = 1 m, dt = 0.01 s).
 *         Compare Euler and RK4 with the exact solution at every step.
 *         Also check energy: with no damping it must stay constant.
 *         Output: solver_comparison.csv
 *
 * Test 2  Step-size study. Repeat Test 1 for several dt and find the largest
 *         error over 10 s. The observed order shows how fast each solver's
 *         error falls when dt gets smaller (Euler ~1, RK4 ~4).
 *         Output: step_size_study.csv
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "msd.h"

/* Largest |x_numeric - x_exact| over the whole run. */
static double max_error(const State *data, int n, const Params *p)
{
    double worst = 0.0;
    for (int i = 0; i < n; i++) {
        double err = fabs(data[i].x - exact_undamped(data[i].t, p));
        if (err > worst) worst = err;
    }
    return worst;
}

/* Test 1: Euler vs RK4 vs exact for one dt. Returns 0 on success. */
static int test_free_vibration(const Params *p, const char *csv_name)
{
    int n = num_steps(p) + 1;
    State *eu = malloc((size_t)n * sizeof(State));
    State *rk = malloc((size_t)n * sizeof(State));
    if (eu == NULL || rk == NULL) {
        fprintf(stderr, "Not enough memory for %d states.\n", n);
        free(eu);
        free(rk);
        return 1;
    }

    simulate(eu, n, p, step_euler);   /* same loop, solver passed */
    simulate(rk, n, p, step_rk4);     /* as a function pointer    */

    FILE *f = fopen(csv_name, "w");
    if (f == NULL) {
        fprintf(stderr, "Cannot open %s for writing.\n", csv_name);
        free(eu);
        free(rk);
        return 1;
    }
    fprintf(f, "t,x_exact,x_euler,x_rk4,err_euler,err_rk4,energy_euler,energy_rk4\n");

    printf("Test 1: free vibration, Euler vs RK4 vs exact (dt = %g s)\n\n", p->dt);
    printf("   t (s)   x_exact    x_euler     x_rk4    err_euler    err_rk4\n");

    int print_every = (int)(1.0 / p->dt + 0.5);   /* one row per second */
    for (int i = 0; i < n; i++) {
        double xe   = exact_undamped(eu[i].t, p);
        double errE = fabs(eu[i].x - xe);
        double errR = fabs(rk[i].x - xe);

        fprintf(f, "%.4f,%.8f,%.8f,%.8f,%.3e,%.3e,%.8f,%.8f\n",
                eu[i].t, xe, eu[i].x, rk[i].x, errE, errR,
                energy(&eu[i], p), energy(&rk[i], p));

        if (i % print_every == 0) {
            printf("  %5.1f  %9.5f  %9.5f  %9.5f  %10.2e  %10.2e\n",
                   eu[i].t, xe, eu[i].x, rk[i].x, errE, errR);
        }
    }
    fclose(f);

    double E0 = energy(&eu[0], p);
    printf("\n  Energy at t = 0:      %.6f J\n", E0);
    printf("  Energy at t = %.0f s:  Euler %.6f J (%+.2f%%), RK4 %.6f J (%+.6f%%)\n",
           p->t_end,
           energy(&eu[n - 1], p), 100.0 * (energy(&eu[n - 1], p) - E0) / E0,
           energy(&rk[n - 1], p), 100.0 * (energy(&rk[n - 1], p) - E0) / E0);
    printf("  Wrote %s (%d rows)\n\n", csv_name, n);

    free(eu);
    free(rk);
    return 0;
}

/* Test 2: largest error over the run for several time steps. */
static int test_step_size(const Params *base, const char *csv_name)
{
    const double dts[] = {0.1, 0.05, 0.02, 0.01, 0.005, 0.002, 0.001};
    const int count = (int)(sizeof dts / sizeof dts[0]);

    FILE *f = fopen(csv_name, "w");
    if (f == NULL) {
        fprintf(stderr, "Cannot open %s for writing.\n", csv_name);
        return 1;
    }
    fprintf(f, "dt,max_err_euler,max_err_rk4\n");

    printf("Test 2: step-size study (largest error over %.0f s)\n\n", base->t_end);
    printf("   dt (s)   max_err_euler   max_err_rk4   order_euler   order_rk4\n");

    double prev_dt = 0.0, prev_e = 0.0, prev_r = 0.0;
    for (int j = 0; j < count; j++) {
        Params p = *base;
        p.dt = dts[j];
        int n = num_steps(&p) + 1;

        State *data = malloc((size_t)n * sizeof(State));
        if (data == NULL) {
            fprintf(stderr, "Not enough memory for %d states.\n", n);
            fclose(f);
            return 1;
        }
        simulate(data, n, &p, step_euler);
        double errE = max_error(data, n, &p);
        simulate(data, n, &p, step_rk4);
        double errR = max_error(data, n, &p);
        free(data);

        fprintf(f, "%g,%.6e,%.6e\n", p.dt, errE, errR);

        if (j == 0) {
            printf("  %7.3f   %13.3e   %11.3e          -           -\n",
                   p.dt, errE, errR);
        } else {
            /* order = log(error ratio) / log(dt ratio) */
            double oE = log(prev_e / errE) / log(prev_dt / p.dt);
            double oR = log(prev_r / errR) / log(prev_dt / p.dt);
            printf("  %7.3f   %13.3e   %11.3e   %11.2f   %9.2f\n",
                   p.dt, errE, errR, oE, oR);
        }
        prev_dt = p.dt;
        prev_e = errE;
        prev_r = errR;
    }
    fclose(f);
    printf("\n  Wrote %s\n\n", csv_name);
    return 0;
}

int main(void)
{
    /* Free vibration: m, c, k, F0, w, x0, v0, dt, t_end */
    Params p = {1.0, 0.0, 20.0, 0.0, 0.0, 1.0, 0.0, 0.01, 10.0};

    double wn = natural_frequency(&p);
    printf("Mass-spring-damper solver test\n");
    printf("  m = %g kg, c = %g N s/m, k = %g N/m, x0 = %g m\n", p.m, p.c, p.k, p.x0);
    printf("  wn = %.4f rad/s, period T = %.4f s, zeta = %.3f\n\n",
           wn, 2.0 * 3.14159265358979323846 / wn, damping_ratio(&p));

    if (test_free_vibration(&p, "solver_comparison.csv") != 0) return 1;
    if (test_step_size(&p, "step_size_study.csv") != 0) return 1;

    printf("Plot the results with:  gnuplot plot_part1.gp\n");
    return 0;
}
