import math

LENGTH = 2**8
HEIGHT = 255 / 2

n = list(range(LENGTH))
s = [round(math.sin((i*math.pi*2)/LENGTH)*HEIGHT+HEIGHT)-math.ceil(HEIGHT) for i in n]
print(s)
