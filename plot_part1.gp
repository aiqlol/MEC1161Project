# plot_part1.gp - Part 1 (Aqil): plots for the report.
#
# Run solver_test first, then in the same folder:
#     gnuplot plot_part1.gp
#
# Makes four PNG files:
#     part1_displacement.png  Euler vs RK4 vs exact, x against t
#     part1_error.png         error against the exact solution (log scale)
#     part1_energy.png        energy against t (must stay at 10 J)
#     part1_step_size.png     largest error against dt (log-log)

set datafile separator ','
set key autotitle columnhead          # first CSV line holds column names

# If this line gives an error, change 'pngcairo' to 'png'.
set terminal pngcairo size 1000,560 font 'Arial,12'

set grid
set key below horizontal

# ---- 1. Displacement ------------------------------------------------------
set output 'part1_displacement.png'
set title 'Free vibration (m = 1 kg, k = 20 N/m, c = 0, dt = 0.01 s)'
set xlabel 'Time t (s)'
set ylabel 'Displacement x (m)'
plot 'solver_comparison.csv' using 1:2 with lines lw 5 lc rgb '#BBBBBB' title 'Exact', \
     ''                      using 1:3 with lines lw 2 lc rgb '#D85A30' title 'Euler', \
     ''                      using 1:4 with lines lw 2 dashtype 2 lc rgb '#534AB7' title 'RK4'

# ---- 2. Error against the exact solution ----------------------------------
set output 'part1_error.png'
set title 'Error against the exact solution (dt = 0.01 s)'
set ylabel 'Error |x - x exact| (m)'
set logscale y
set format y '%.0e'
plot 'solver_comparison.csv' using 1:5 with lines lw 2 lc rgb '#D85A30' title 'Euler', \
     ''                      using 1:6 with lines lw 2 lc rgb '#534AB7' title 'RK4'
unset logscale y
set format y '%g'

# ---- 3. Energy ------------------------------------------------------------
set output 'part1_energy.png'
set title 'Energy with no damping (the true value stays at 10 J)'
set ylabel 'Energy 1/2 k x^2 + 1/2 m v^2 (J)'
plot 'solver_comparison.csv' using 1:7 with lines lw 2 lc rgb '#D85A30' title 'Euler', \
     ''                      using 1:8 with lines lw 2 lc rgb '#534AB7' title 'RK4'

# ---- 4. Step-size study ---------------------------------------------------
set output 'part1_step_size.png'
set title 'Largest error over 10 s against time step'
set xlabel 'Time step dt (s)'
set ylabel 'Largest error (m)'
set logscale xy
set format x '%g'
set format y '%.0e'
plot 'step_size_study.csv' using 1:2 with linespoints lw 2 pt 7 lc rgb '#D85A30' title 'Euler', \
     ''                    using 1:3 with linespoints lw 2 pt 7 lc rgb '#534AB7' title 'RK4'

unset output
