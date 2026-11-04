/*
 * msd.h - shared interface for the MEC1161 mass-spring-damper project.
 *
 * Every team member codes against these names. Do not change a
 * signature without telling the team. Adding new functions is fine.
 *
 * Model (Newton's second law):  m*a = F(t) - c*v - k*x
 *   with the outside force       F(t) = F0 * sin(w * t)
 *
 * Part 1 (Aqil): accel, step_euler, step_rk4, simulate and the helpers.
 * Part 3 / Part 4: write_csv, plot_gnuplot.
 */
#ifndef MSD_H
#define MSD_H

/* Inputs for one simulation run. */
typedef struct {
    double m;      /* mass [kg]                                  */
    double c;      /* damping coefficient [N s/m]                */
    double k;      /* spring stiffness [N/m]                     */
    double F0;     /* forcing amplitude [N]  (0 = no force)      */
    double w;      /* forcing frequency [rad/s]                  */
    double x0;     /* start displacement [m]                     */
    double v0;     /* start velocity [m/s]                       */
    double dt;     /* time step [s]                              */
    double t_end;  /* end time [s]                               */
} Params;

/* The system at one moment in time. */
typedef struct {
    double t;      /* time [s]                                   */
    double x;      /* displacement [m]                           */
    double v;      /* velocity [m/s]                             */
    double a;      /* acceleration [m/s^2]                       */
} State;

/* ---- Part 1: model and solvers (model.c) ---------------------------- */

/* Acceleration from Newton's second law at time t, position x, velocity v. */
double accel(double t, double x, double v, const Params *p);

/* Move state s forward by one time step p->dt.
 * Both solvers have the same inputs, so either can be passed to simulate(). */
void   step_euler(State *s, const Params *p);   /* 1st order, simple     */
void   step_rk4(State *s, const Params *p);     /* 4th order, accurate   */

/* Fill out[0 .. n-1] with the motion from t = 0.
 * out[0] is set from p->x0 and p->v0. The caller allocates out9wh
 * (n states, use n = num_steps(p) + 1). step = step_euler or step_rk4. */
void   simulate(State *out, int n, const Params *p,
                void (*step)(State *, const Params *));

/* Helpers */
int    num_steps(const Params *p);          /* steps from 0 to t_end        */
double natural_frequency(const Params *p);  /* wn = sqrt(k/m) [rad/s]       */
double damping_ratio(const Params *p);      /* zeta = c / (2 sqrt(m k))     */
double energy(const State *s, const Params *p); /* 1/2 k x^2 + 1/2 m v^2 [J] */

/* Exact solution for free vibration with no damping (c = 0, F0 = 0):
 * x(t) = x0 cos(wn t) + (v0 / wn) sin(wn t). Used to test the solvers. */
double exact_undamped(double t, const Params *p);

/* ---- Part 3 / Part 4 ------------------------------------------------- */
int    write_csv(const char *file, const State *data, int n);
void   plot_gnuplot(const char *csv_file, const char *title);

#endif /* MSD_H */
