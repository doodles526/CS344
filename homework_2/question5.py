# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

import subprocess
import shlex
import os
import sys
import getopt

def who_sp(options = None):
    # initialize base command
    command = "who"
    if options:
        # if we are passed additional options, add them too
        command = "%s %s"%(command, options)

    # split the command into CSV
    command_sequence = shlex.split(command)
    # Start the subprocess
    who = subprocess.Popen(command_sequence, stdout=subprocess.PIPE)
    # who_out is now acting as the pipe receiver
    who_out = who.stdout.readlines()

    # And print out the lines
    for line in who_out:
        print line,

def main(argv):
    # Accepting CLI args for extra credit
    try:
        opts, args = getopt.getopt(argv, "bdlprTtu")
    except getopt.GetoptError:
        print "Usage: question5.py (all optional args) -b -d -l -p -r -T -t -y"
        sys.exit(2)
    arg_str = ""
    
    # Construct the arg string
    for opt, arg in opts:
        arg_str = arg_str + opt + " "
    who_sp(arg_str)

if __name__ == "__main__":
    main(sys.argv[1:])
