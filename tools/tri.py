LENGTH = 2**8
HEIGHT = 255 / 2

# not entirely sure how this works but it gives the right output so whatever
n = list(range(LENGTH))
s = [-round(2 * abs(i - HEIGHT) - HEIGHT) for i in n]
print(s)
