
# Note you need gnuplot 4.4 for the pdfcairo terminal.

set terminal pdfcairo font "Gill Sans,5" linewidth 4 rounded

# Line style for axes
set style line 80 lt rgb "#808080"

# Line style for grid
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"  # grey

set grid back linestyle 81
set border 3 back linestyle 80 # Remove border on top and right.  These
             # borders are useless and make it harder
             # to see plotted lines near the border.
    # Also, put it in grey; no need for so much emphasis on a border.
set xtics nomirror
set ytics nomirror

#set log x
#set mxtics 10    # Makes logscale look good.

# Line styles: try to pick pleasing colors, rather
# than strictly primary colors or hard-to-see colors
# like gnuplot's default yellow.  Make the lines thick
# so they're easy to see in small plots in papers.
set style line 1 lt rgb "#A00000" lw 2 pt 1
set style line 2 lt rgb "#00A000" lw 2 pt 6
set style line 3 lt rgb "#5060D0" lw 2 pt 2
set style line 4 lt rgb "#F25900" lw 2 pt 9
set style line 5 lt rgb "#000000" lw 2 pt 7
set style line 6 lt rgb "#B222FF" lw 2 pt 5
set style line 7 lt rgb "#EEE8AA" lw 2 pt 5

set output "../redis-server/logs.txt.cdf.pdf"
set xlabel "time(ns) OR ratio"
set ylabel "Fraction"

#set key bottom right

set key font ",5"

set xrange [0:]
#set yrange [0:1]

#
# blank spot for options
#


plot "../redis-server/logs-1200000.txt-.cdf" using 1:2 title "1.2GHz" w lines ls 1, "../redis-server/logs-3200000.txt-.cdf" using 1:2 title "3.2GHz" w lines ls 2 #, "../redis-server/logs-1200000.txt-non-IO.cdf" using 1:2 title "1.2GHz-Comp" w lines ls 3, "../redis-server/logs-3200000.txt-non-IO.cdf" using 1:2 title "3.2GHz-Comp" w lines ls 4 #, "../redis-server/logs--1.txt-ratio.cdf" using 1:2 title "ondemand" w lines ls 7#, "../redis-server/logs--1.txt-ratio.cdf" using 1:2 title "ondemand" w lines ls 7
