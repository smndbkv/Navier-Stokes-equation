set terminal wxt size 900,700 enhanced
set title "Velocity field (arrows) coloured by pressure p"
set xlabel 'x'
set ylabel 'y'
set xrange [0:1]
set yrange [0:1]
set size ratio -1
set palette defined (0 'blue', 0.5 'cyan', 1 'red')
set cblabel 'Pressure p'
plot 'p_matrix.dat' matrix using ($1/199):($2/199):3 with image notitle, \
     'vec_field.dat' using 1:2:3:4 with vectors head filled lc 'black' scale 0.15 notitle
pause mouse close
