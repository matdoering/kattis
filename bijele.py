nbrs = input()
nbrs = nbrs.split(" ")
expected = [1, 1, 2, 2, 2, 8]
diff = [str(int(e) - int(a)) for (e,a) in zip(expected, nbrs)]
print(" ".join(diff))
