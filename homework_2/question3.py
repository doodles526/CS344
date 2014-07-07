# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

import urllib2
import sys

def main(argv):
    # get a response from the url
    # also concat the http protocol
    response = urllib2.urlopen("http://%s"%argv[0])

    # open a file for output
    # Note we use wb, so we can save more than just
    # Text
    outfile = open(argv[1], "wb")

    # write the response to the file
    outfile.write(response.read())

    # close the file
    outfile.close()

if __name__ == "__main__":
    # Call main, passing all args except file name
    main(sys.argv[1:])
