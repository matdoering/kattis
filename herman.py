# taxicab geometry
# input: radius of circle
# 1st output: area of circle using Euclidean distance
# 2nd output: area of circle using Manhattan distance

import math

r = int(input())
A_eucl = math.pi * r**2
# circle in manhattan distance:
#       x
#     x x x
#       x
# use pythagoras to find length of triangle side
# a**2 + b**2 = c**2 <=> r**2 + r**2 = c**2
# <=> 2r**2 = c**2
# A = c * c = 2r**2
A_manh = 2 * r**2

print(A_eucl)
print(A_manh)

