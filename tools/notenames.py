notes = ["c", "c#", "d", "d#", "e", "f", "f#", "g", "g#", "a", "a#", "b"]
n = list(range(21, 94)) # A0-A6 (incl)
o = [notes[i%12]+str((i-12)//12) for i in n]
print(str(o).replace("'", '"'))
