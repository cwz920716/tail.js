#!/usr/bin/python
import sys
from os import listdir
from os.path import isfile, join
import os
import re
import create_cdf
import create_gnuplot

#
# Will take in multiple directories, from the meta-*.sh script
#   -Goes over all directories and files within and generates
#    CDF of flow completion times over all flows and CDF of whole workload
#    completion times
#   IMPORTANT: make sure that all result directories are from consistent runs!
#      ie- all parameters are the same           
#
#
#    NOTE: this script doesn't work when background traffic is present
#      since it just sums the output of the workload-sender
#
# store dir -- where all results are put into
# results dir -- results from meta-*.sh run
#
if len(sys.argv) < 3:
    print "Usage: ", sys.argv[0], " <store dir> [<results dir>]+"
    exit(-1)


store = sys.argv[1]

#
# State we want
#
verbose = 0
#all flow times, in seconds
flowTimes = []
#all flow throughputs, in Mbps
flowMbps = []
#workload completion times, in seconds
#  if there is only 1 flow, then this is same as flowTimes
#  if it was shuffle workload, this is is time it took to do the shuffle
workloadTimes = []


#
# create directory to store results
#
if not os.path.exists(store):
    os.makedirs(store)

fap = "%s/aggregate-throughput" % (store)
afile = open(fap, 'w')
    
#
# go through all files in all directories
#
for dir in sys.argv[2:]:
    #
    # keep aggregate throughput (bisection bandwidth in fat tree)
    #
    flowSize=0 #all flow sizes should be consistent, in bytes
    bottleNeck=0 #workload that took longest to complete
    
    print "Got directory", dir 
    onlyfiles = [ f for f in listdir(dir) if isfile(join(dir,f)) ]

    for f in onlyfiles:
        match = re.match("arldcn(.*).out", f)
        if (not match):
            continue
        match = re.match("arldcn(.*).out.bak", f)
        if (match):
            continue
        match = re.match("arldcn(.*).background.out", f)
        if (match):
            continue
        if (verbose):
            print "   processing file:", f
#        print "   processing file:", f
        fp = "%s/%s" % (dir, f)
        file = open(fp, 'r')
        for line in file:
            #if (verbose):
            #    print "         got line", line
            match = re.match( r'Total time: (.*) seconds', line)
            if match:
                workloadTimes.append(float(match.group(1)))
                #print match.group(1)
                              
            match = re.match(r'Flow (.*) to (.*) done from (.*) sent (.*) bytes in (.*) secs (.*) to (.*) for (.*) Mbps', line)
            if match:
                if (verbose):
                    print "secs", match.group(5)
                    print "mbps", match.group(8)
                flowTimes.append(float(match.group(5)))
                flowMbps.append(float(match.group(8)))
                flowSize += int(match.group(4))


            #
            # background hack!!!
            #
            match = re.match(r'Sent (.*) MB to host (.*) in (.*) sec for (.*) Mbps',line)
            if match:
                if (verbose):
                    print "secs", match.group(3)
                    print "mbps", match.group(4)
                flowTimes.append(float(match.group(3)))
                flowMbps.append(float(match.group(4)))
                flowSize += float(match.group(1))
                bottleNeck = 1 #will never be found


            match = re.match(r'Total time: (.*) seconds', line)
            if match:
                if float(match.group(1)) > bottleNeck:
                    bottleNeck = float(match.group(1))

        file.close()
    Gbps = flowSize*8.0/bottleNeck/1000000000.0
    print "  Aggregate throughput is %f Gbps\n"%(Gbps) 
    afile.write("%s %f Gbps\n"%(dir, Gbps))
    
afile.close()

#
# output file names
#
sto = "%s/workloadTimes"%(store)
fto = "%s/flowTimes"%(store)
fmo = "%s/flowMbps"%(store)

#
# now we have our data, let's get CDFs
#
if (verbose):
    print "Workload CDF"
cdfWorkloadTimes = create_cdf.create_cdf(workloadTimes)
cdfWorkloadTimes.cdf(sto, verbose)

if (verbose):
    print "Flow times CDF"
cdfFlowTimes = create_cdf.create_cdf(flowTimes)
cdfFlowTimes.cdf(fto, verbose)

if (verbose):
    print "Flow Mbps CDF"
cdfFlowMbps = create_cdf.create_cdf(flowMbps)
cdfFlowMbps.cdf(fmo, verbose)


#
# CDFs are generated, let's plot them now
#
pltSt = create_gnuplot.create_gnuplot(sto)
pltSt.plot_many_plots("Time (sec)", "Fraction", [sto])

pltFt = create_gnuplot.create_gnuplot(fto)
pltFt.plot_many_plots("Time (sec)", "Fraction", [fto])

pltMb = create_gnuplot.create_gnuplot(fmo)
pltMb.plot_many_plots("Mbps", "Fraction", [fmo])


#os.system("mv flowMbps* %s"%(store))
#os.system("mv flowTimes* %s"%(store))
#os.system("mv workloadTimes* %s"%(store))
