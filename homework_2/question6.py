# Josh Deare
# dearej@onid.orst.edu
# CS311-400
# Homework 2

from collections import deque

def main():
    # Initialize the big number
    big_number = 5369781797784617406495514929086256932197846862248282166370484403199890008895243450658541227588666881164271714799244429282308634656748139191231628245861786645835912456652947654568284891288314260769004224219022671055626321111109370544217506941658960408125406987471585238630507156932909632952274430435576689664895044524452316173185640309871112172238311362229893423380308135336276614282806444486645238749657273330010533678812202354218097512545405947522435258490771167055601360483958644670632441572215539707198403850962455444362981230987879927244284909188845801561660979191338754992005240636899125607176060588611646710940507754100225698315520005593572972530358907296290491560440772390713810515859307960866701724271218839987979087922749219016997208880937767163626956188267042825248360082325753042075296345073167176531330624919225119674426574742355349194934839722413756570560574902614079729686524145351004749698352031277450632623957831801698480186947885184385861560789112949495459501737958331952853208805511
    # turn that number into an iterable string
    big_number = str(big_number)

    # Create a queue with a maxlen of 5
    # when we push a value onto the queue
    # when the len is 5, then it removes
    # the oldest element
    number_queue = deque(maxlen=5)
    largest_product = 0

    for char in big_number:
        # add the current character to the number queue
        number_queue.append(int(char))
        # Check if we have an appropriate number of items in the queue
        if len(number_queue) == number_queue.maxlen:
            #initialize the product
            product = 1
            
            # Produce the product
            for i in number_queue:
                product = product * i

            # Set the largest product if we have exceeded it
            if product > largest_product:
                largest_product = product
    print largest_product

if __name__ == "__main__":
    main()