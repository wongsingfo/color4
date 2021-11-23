call '../third_party/gnuplot/gnuplot/header.gp' fp

set key above
set ylabel 'False positive rate (%)'
set xlabel 'Filter capacity (log base 2)'
set yrange [0:1.2]

plot 'load-12-4.txt' u 1:($3*100) w lp title 'YeahCuckoo', \
     ''              u 1:($5*100) w lp title 'Cuckoo', \
     ''              u 1:($9*100) w lp title 'Cuckoo-2', \
     ''              u 1:($7*100) w lp title 'Cuckoo-4', \

call '../third_party/gnuplot/gnuplot/footer.gp'
