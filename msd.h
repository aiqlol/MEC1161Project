#ifndef MSD_H
#define MSD_H

typedef struct {
    double m, c, k;      // mass (kg), damping (N*s/m), stiffness (N/m)
    double F0, w;        // forcing amplitude (N), forcing frequency (rad/s)
    double x0, v0;       // initial displacement (m), initial velocity (m/s)
    double dt, t_total;  // time step (s), total simulation time (s)
} Params;

typedef struct {
    double t, x, v, a;   // time (s), displacement (m), velocity (m/s), acceleration (m/s^2)
} State;

double accel(double t, double x, double v, const Params *p);   // a = (F - c*v - k*x) / m
void step_euler(State *s, const Params *p);                     // move s forward one step (Euler)
void step_rk4(State *s, const Params *p);                       // move s forward one step (RK4)
int num_steps(const Params *p);                                 // number of time steps in t_total
void simulate(State *out, int n, const Params *p,
              void (*step)(State *, const Params *));           // fill out[0..n-1] using the chosen method

#endif
