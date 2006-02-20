#set terminal svg size 720 540 fixed fsize 24
#set terminal png large size 800,600 xffffff
set terminal png medium xffffff

set xlabel "Profiling Cycle"

set ylabel "Percent of Total Time"
set output "results.png"
plot "results.dat" using 1:2 title "block1" with lines linewidth 4, \
	"results.dat" using 1:3 title "block2" with lines linewidth 4, \
	"results.dat" using 1:4 title "block3" with lines linewidth 4
