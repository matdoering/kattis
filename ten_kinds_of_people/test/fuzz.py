import random
import os

# create input files with varying characteristics
# in order to validate implementation

# write an input file with specific properties
def generateSample(nrow, ncol, nQueries):
    # nrow: number of rows in map
    # ncol: number of cols in map
    # nQueries: number of queries
    #nOnes = random.randrange(0, nrow*ncol)
    fname = "data/" + str(nrow) + "_" + str(ncol) + "_" + str(nQueries) + "_" + ".in"
    with open(fname, "w+") as f:
        f.write(str(nrow) + " " + str(ncol) + "\n")
        # write map
        for _ in range(nrow):
            rowData = [0 for i in range(ncol)]
            oneIdx = random.sample([i for i in range(ncol)], random.randrange(0, ncol))
            for i in oneIdx:
                rowData[i] = 1
            f.write("".join(str(x) for x in rowData) + "\n")
        f.write(str(nQueries) + "\n")
        # write queries
        for _ in range(nQueries):
            fro = (random.randrange(1, nrow+1), random.randrange(1, ncol+1))
            to = (random.randrange(1, nrow+1), random.randrange(1, ncol+1))
            f.write(str(fro[0]) + " " + str(fro[1]) + " " + str(to[0]) + " " + str(to[1]) + "\n")

def generateSamples(n, maxDim, maxQueries):
    # n: number of samples to generate
    for i in range(n):
        nrow = random.randrange(1, maxDim)
        ncol = random.randrange(1, maxDim)
        nQueries = random.randrange(1, maxQueries)
        generateSample(nrow, ncol, nQueries)
    print("generated " + str(n) + " samples with maxDim " + str(maxDim))


def generateSomeSamples():
    os.system("rm data/*")
    generateSamples(10, 10, 10)
    generateSamples(5, 50, 10)
    generateSamples(5, 200, 10)
    generateSamples(5, 1000, 5)
    generateSamples(3, 1000, 1000)
    #generateSamples(1, 10000, 5)
    #generateSamples(1, 15000, 10)


def testSolver():
    generateSomeSamples()
    testFiles = os.listdir("data")
    for f in testFiles:
        # call with file as input
        #rc = os.system("../Solver " + "data/" + f)
        # call with cin as input
        rc = os.system("../Solver < data/" + f)
        if rc != 0:
            print("Failure at: " + f)

testSolver()
