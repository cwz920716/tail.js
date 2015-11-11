from subprocess import call

#
# Generate gnuplot files for data. 
#
class create_gnuplot(object):


    #
    # outputfile: give the prefix only-- file extensions will be added by my script!
    #
    def __init__(self, outputfile):
        self.pltfile = outputfile
        self.options = ""

    #
    # can set custom options in gnuplot file. just gives you the last line before "plot"
    #  in order to override any previous things in template, or to set new options
    #
    def setOptions(self, options):
        self.options = "%s\n%s"%(self.options, options)

    #
    # If you call this file with only a single input file, then it is important
    # to make sure it is still an array. If you just have a string 'filename', then
    # call this function like:
    #    plot_many_plots("time", "mbps", [filename])
    #
    def plot_many_plots(self, xlabel, ylabel, inputfiles = []):
        gf = "%s.pdf"%(self.pltfile)
        pf = "%s.plt"%(self.pltfile)
        file = open(pf, 'w')
        #print "length is %d"%(len(inputfiles))
        index=1
        plotString = "plot "
        for i in inputfiles:
#            print "got i %s" % (i)
            plotString = "%s \"%s\" using 1:2 title \"%s\" w lines ls %d"%(plotString, i, i, index)
            if (index < len(inputfiles)):
                plotString = "%s, "%(plotString)

            index = index+1

#        print plotString

        gnuplottemplate="""
# Note you need gnuplot 4.4 for the pdfcairo terminal.

set terminal pdfcairo font "Gill Sans,9" linewidth 4 rounded

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

set output "%s"
set xlabel "%s"
set ylabel "%s"

#set key bottom right

set key font ",5"

#set xrange [0:1]
#set yrange [0:1]

#
# blank spot for options
#
%s

%s
""" % (gf, xlabel, ylabel, self.options, plotString)
        
        
        #
        # print gnuplottemplate
        #
        file.write(gnuplottemplate)
        file.close()

        # run gnuplot on our file
        call(["gnuplot", pf])
