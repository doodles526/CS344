# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

import os
import sys
import getopt

def ensure_dir(path):
    try:
        os.mkdir(path)
    except:
        print "WARNING: %s already exists."%path

def main(argv):
    """
    Main entry function

    gets passed the cli arguments
    creates directory hierarchy:
    ~/eecs
        ./<term>
            ./<class>
            ./assignments/
            ./examples/
            ./exams/
            ./lecture_notes/
            ./submissions/
            ./src_class #symlink
    """
    # initialize the term and course in the outermost scope of
    # the main function
    term = None
    course = None
    try:
        # parse the CLI arguments into opts
        opts, args = getopt.getopt(argv, "t:c:", ["term", "class"])
    except getopt.GetoptError:
        # If we encountered an error, print the usage string
        print "question2.py -t[--term] <term> -c[--class] <class>"
        # And exit with status 2
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-t", "--term"):
            # set the term
            term = arg
        elif opt in ("-c", "--class"):
            course = arg
    
    # Initialize a base path
    base_path = os.path.expanduser("~/eecs/")
    
    # Create eecs dir
    ensure_dir(base_path)

    # Create Term Dir
    base_path = os.path.join(base_path, term)
    ensure_dir(base_path)

    # Create Class Dir
    base_path = os.path.join(base_path, course)
    ensure_dir(base_path)

    # Create all sub directories
    ensure_dir(os.path.join(base_path, "assignments"))
    ensure_dir(os.path.join(base_path, "examples"))
    ensure_dir(os.path.join(base_path, "exams"))
    ensure_dir(os.path.join(base_path, "lecture_notes"))
    ensure_dir(os.path.join(base_path, "submissions"))
    
    # create the symlink base path
    symlink_path = os.path.join("/usr", "local", "classes", "eecs", term, course, "src")
    
    # make the symlinks
    os.symlink(symlink_path, os.path.join(base_path, "src_class"))
    os.symlink(os.path.join(symlink_path, "README"), os.path.join(base_path, "README"))

if __name__ == "__main__":
    # Call main while passing all args
    # except the file name
    main(sys.argv[1:])
