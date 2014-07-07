# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

import subprocess
import shlex
import os
import sys

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
    arg_str = ""
    for arg in argv:
        arg_str = arg_str + arg
    who_sp(arg_str)

if __name__ == "__main__":
    main(sys.argv[1:])
