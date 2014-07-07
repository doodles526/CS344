# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

import math
import sys

COMPOSITE = 3

def primes_to_n(scan_to):
    """
    Returns number of primes and list of primes between 0 and scan_to
    
    Return value is a tuple, as (num_primes, primes[])
    """
    # initialize the lists
    num_list = [None] * scan_to
    primes = list()

    # initialize counters
    num_primes = 0
    k = 2
    
    # Mark composites until k exceeds the sqrt of our scan range
    while k < math.sqrt(scan_to):
        m = k

        # iterate m, until we find a non-composite 
        while num_list[m] == COMPOSITE and m < scan_to:
            m = m + 1
        
        # Until we reach our scan_to, we set all
        # 2n * m values to composite
        comp_numbers = 2
        while (comp_numbers * m) < scan_to:
            num_list[comp_numbers * m] = COMPOSITE
            comp_numbers = comp_numbers + 1

        # Add this m to the list of primes
        primes.append(m)
        # Increment num_primes
        num_primes = num_primes + 1
        # And setup for the next loop
        k = m + 1

    # iterate from k to scan_to
    for i in range(k, scan_to):
        if num_list[i] != COMPOSITE:
            primes.append(i)
            num_primes = num_primes + 1

    return (num_primes, primes)

def get_nth_prime(n):
    """
    Returns the nth prime number
    """
    # Plug n into the prime number theorem
    # multiplied by 1.5 and added with 3 as offset
    upper_to_test = int(n * 1.5 * math.log(n) + 3)
    return primes_to_n(upper_to_test)[1][n - 1]

def main(argv):
    # print the result of get_nth_prime
    print get_nth_prime(int(argv[0]))

if __name__ == "__main__":
    # call main with cli args
    main(sys.argv[1:])
