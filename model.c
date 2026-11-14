#include <math.h>
#include "msd.h"

double accel(double t, double x, double v, const Params *p)
{
    double F = p->F0 * sin(p->w * t);          // external force at time t (0 if F0 = 0)
    return (F - p->c * v - p->k * x) / p->m;   // Newton's 2nd law: a = sum of forces / m
}

void step_euler(State *s, const Params *p)
{
    double a = accel(s->t, s->x, s->v, p);     // acceleration at start of step
    s->x += s->v * p->dt;                      // new x from old v
    s->v += a * p->dt;                         // new v from old a
    s->t += p->dt;                             // advance time
    s->a = accel(s->t, s->x, s->v, p);         // acceleration at new state
}

void step_rk4(State *s, const Params *p)
{
    double dt = p->dt, h = dt / 2;             // full step and half step
    double t = s->t, x = s->x, v = s->v;       // values at start of step

    double k1x = v;                                             // slope of x at start
    double k1v = accel(t, x, v, p);                             // slope of v at start
    double k2x = v + h * k1v;                                   // slope of x at midpoint (using k1)
    double k2v = accel(t + h, x + h * k1x, v + h * k1v, p);     // slope of v at midpoint (using k1)
    double k3x = v + h * k2v;                                   // slope of x at midpoint (using k2)
    double k3v = accel(t + h, x + h * k2x, v + h * k2v, p);     // slope of v at midpoint (using k2)
    double k4x = v + dt * k3v;                                  // slope of x at end (using k3)
    double k4v = accel(t + dt, x + dt * k3x, v + dt * k3v, p);  // slope of v at end (using k3)

    s->x = x + dt / 6 * (k1x + 2 * k2x + 2 * k3x + k4x);        // new x from weighted average slope
    s->v = v + dt / 6 * (k1v + 2 * k2v + 2 * k3v + k4v);        // new v from weighted average slope
    s->t = t + dt;                                              // advance time
    s->a = accel(s->t, s->x, s->v, p);                          // acceleration at new state
}

int num_steps(const Params *p)
{
    return (int)(p->t_total / p->dt + 0.5);    // t_total / dt, rounded to nearest whole step
}

void simulate(State *out, int n, const Params *p, void (*step)(State *, const Params *))
{
    out[0].t = 0;                              // start time
    out[0].x = p->x0;                          // initial displacement
    out[0].v = p->v0;                          // initial velocity
    out[0].a = accel(0, p->x0, p->v0, p);      // initial acceleration
    for (int i = 1; i < n; i++) {              // each remaining time step
        out[i] = out[i - 1];                   // start from previous state
        step(&out[i], p);                      // advance one step with chosen method
        out[i].t = i * p->dt;                  // exact time (stops round-off building up)
    }
}
