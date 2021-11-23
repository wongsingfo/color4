call '../third_party/gnuplot/gnuplot/header.gp' load

set key above
set ylabel 'Load factor'
set xlabel 'Filter capacity (log base 2)'
set yrange [0.98:1]

plot 'load-12-4.txt' u 1:2 w lp title 'YeahCuckoo', \
     ''              u 1:4 w lp title 'Cuckoo', \
     ''              u 1:8 w lp title 'Cuckoo-2', \
     ''              u 1:6 w lp title 'Cuckoo-4', \

call '../third_party/gnuplot/gnuplot/footer.gp' load
