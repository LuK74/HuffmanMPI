import string
import random
import sys

S = int(sys.argv[1])  # number of characters in the string.    
weights = random.choices([1,2,3,4,5,6,7,8,9],[0.1,0.2,0.8,0.7,0.3,0.1,0.9,0.6,0.5], k = 100)
ran = ''.join(random.choices(string.printable, weights, k = S))    
print(ran)
