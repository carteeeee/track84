import math

LENGTH = 2**8
HEIGHT = 255 / 2

n = list(range(LENGTH))
# the // 6 is for the 6 oscs so it doesn't clip
s = [(round(math.sin((i*math.pi*2)/LENGTH)*HEIGHT+HEIGHT)-math.ceil(HEIGHT)) // 6 + 1 for i in n]
# very hacky solution to have 0 at the start
print([0] + s[:-1])
