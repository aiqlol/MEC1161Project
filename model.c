/*
 * model.c - Part 1 (Aqil): the mass-spring-damper model and its solvers.
 *
 * The equation of motion  m*a = F(t) - c*v - k*x  is second order.
 * The solvers split it into two first-order equations:
 *     dx/dt = v
 *     dv/dt = a = (F(t) - c*v - k*x) / m
 * and move (x, v) forward in small time steps dt.
 */
#include <math.h>
#include "msd.h"

/* Newton's second law, rearranged for acceleration.
 * The same function serves all three cases:
 *   free:   c = 0, F0 = 0      damped: c > 0, F0 = 0      forced: F0 > 0 */
double accel(double t, double x, double v, const Params *p)
{
    double F = p->F0 * sin(p->w * t);          /* outside force [N]        */
    return (F - p->c * v - p->k * x) / p->m;   /* a = (sum of forces) / m  */
}

/* Euler's method: use the slope at the START of the step for the whole step.
 *   x_new = x + v * dt
 *   v_new = v + a * dt
 * Simple, but the error grows over time. With c = 0 the amplitude grows,
 * although the real system keeps a constant amplitude. */
void step_euler(State *s, const Params *p)
{
    double a_old = accel(s->t, s->x, s->v, p);   /* acceleration now       */
    double x_old = s->x;                         /* keep the old values:   */
    double v_old = s->v;                         /* x must use the OLD v   */

    s->x = x_old + v_old * p->dt;                /* new position           */
    s->v = v_old + a_old * p->dt;                /* new velocity           */
    s->t = s->t + p->dt;                         /* move time forward      */
    s->a = accel(s->t, s->x, s->v, p);           /* acceleration at new t  */
}

/* 4th-order Runge-Kutta: measure the slope four times in the step and
 * take a weighted average.
 *   k1: slope at the start
 *   k2: slope at the midpoint, reached with k1
 *   k3: slope at the midpoint, reached with k2
 *   k4: slope at the end, reached with k3
 *   new value = old value + dt/6 * (k1 + 2 k2 + 2 k3 + k4)
 * kNx is the slope of x (a velocity); kNv is the slope of v (an acceleration). */
void step_rk4(State *s, const Params *p)
{
    double dt = p->dt;
    double h  = 0.5 * dt;                        /* half step              */
    double t  = s->t, x = s->x, v = s->v;

    double k1x = v;
    double k1v = accel(t, x, v, p);

    double k2x = v + h * k1v;
    double k2v = accel(t + h, x + h * k1x, v + h * k1v, p);

    double k3x = v + h * k2v;
    double k3v = accel(t + h, x + h * k2x, v + h * k2v, p);

    double k4x = v + dt * k3v;
    double k4v = accel(t + dt, x + dt * k3x, v + dt * k3v, p);

    s->x = x + dt / 6.0 * (k1x + 2.0 * k2x + 2.0 * k3x + k4x);
    s->v = v + dt / 6.0 * (k1v + 2.0 * k2v + 2.0 * k3v + k4v);
    s->t = t + dt;
    s->a = accel(s->t, s->x, s->v, p);
}

/* Run a whole simulation with the solver given as a function pointer.
 * The same loop works for Euler and RK4: only the 'step' argument changes. */
void simulate(State *out, int n, const Params *p,
              void (*step)(State *, const Params *))
{
    if (n <= 0) return;

    out[0].t = 0.0;
    out[0].x = p->x0;
    out[0].v = p->v0;
    out[0].a = accel(0.0, p->x0, p->v0, p);

    for (int i = 1; i < n; i++) {
        out[i] = out[i - 1];     /* copy the previous state            */
        step(&out[i], p);        /* move it forward one step           */
        out[i].t = i * p->dt;    /* exact time, no round-off build-up  */
    }
}

/* Number of time steps from t = 0 to t_end.
 * + 0.5 rounds correctly: 10.0 / 0.01 can be 999.9999... in floating point. */
int num_steps(const Params *p)
{
    return (int)(p->t_end / p->dt + 0.5);
}

double natural_frequency(const Params *p)
{
    return sqrt(p->k / p->m);
}

double damping_ratio(const Params *p)
{
    return p->c / (2.0 * sqrt(p->m * p->k));
}

/* Energy stored in the spring plus kinetic energy of the mass. */
double energy(const State *s, const Params *p)
{
    return 0.5 * p->k * s->x * s->x + 0.5 * p->m * s->v * s->v;
}

double exact_undamped(double t, const Params *p)
{
    double wn = natural_frequency(p);
    return p->x0 * cos(wn * t) + (p->v0 / wn) * sin(wn * t);
}
