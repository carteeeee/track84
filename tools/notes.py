SAMPLE_FREQ = 8000
INT_SIZE = 65536

n = list(range(21, 94)) # A0-A6 (incl)
o = [int((440 * 2**((i-69)/12)) * (INT_SIZE / SAMPLE_FREQ)) for i in n]
print(o)
