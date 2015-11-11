#! /usr/bin/python

import sys
from os import listdir
from os.path import isfile, join
import os
import re
import create_cdf
import create_gnuplot
from datetime import datetime

file = open(sys.argv[1], "r")
index = int(sys.argv[2])
xlabel = sys.argv[3]

# queue counter: The counter for measuring how many http requests are on the fly
datapoints = []
linecnt = 0

for lines in file:
	if lines[0] == '#':
		continue
	# print lines
	linecnt = linecnt + 1
	tuples = lines.split()
	if len(tuples) < index:
		index = len(tuples) - 1
	datapoints.append(float(tuples[index]))

file.close()

verbose = 1
sto = sys.argv[1] + '.cdf'

dps = create_cdf.create_cdf(datapoints)
dps.cdf(sto, verbose)


#
# CDFs are generated, let's plot them now
#
pltSt = create_gnuplot.create_gnuplot(sto)
pltSt.plot_many_plots(xlabel, "Fraction", [sto])
