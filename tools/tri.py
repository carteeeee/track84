import math
LENGTH = 2**8
HEIGHT = 255 / 2

# not entirely sure how this works but it gives the right output so whatever
# the / 6 is for the 6 oscs so it doesn't clip
n = list(range(LENGTH))
s = [math.floor(-round(2 * abs(i - HEIGHT) - HEIGHT) / 6 + 0.5) for i in n]
# this rotates the list to start at 0
b = LENGTH//4
print(s[b:]+s[:b])
