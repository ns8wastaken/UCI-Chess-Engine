# Based on https://github.com/AndyGrant/Ethereal/blob/master/src/perft/perft.py

import subprocess, pathlib

totalTime = 0.0
totalNodes = 0


enginePath = pathlib.Path(__file__).absolute().parent.parent.parent / "Epsilon.exe"
testSuitePath = pathlib.Path(__file__).absolute().parent / "standard.epd"
resultPath = pathlib.Path(__file__).absolute().parent / "result.txt"
maxDepth = 4


process = subprocess.Popen(
    str(enginePath),
    stderr = subprocess.STDOUT,
    stdout = subprocess.PIPE,
    stdin  = subprocess.PIPE,
    universal_newlines = True
)

data: list[str] = []

with open(testSuitePath, "r") as f:
    data = f.readlines()


outputFile = open(resultPath, "w")

for line in data:
    expectedPerfts = line.strip().split(" ;")
    fen = expectedPerfts[0]
    expectedPerfts.pop(0)

    resultsList: list[tuple[str, str, str, str, str]] = []
    maxLen0 = 0
    maxLen1 = 0
    maxLen2 = 0
    maxLen3 = 0

    outputFile.write(f"Fen: {fen}\n")

    # Get the perft data
    for token in expectedPerfts:
        depth = int(token.split(" ")[0][1:])
        nodes = int(token.split(" ")[1])

        if depth > maxDepth: break

        process.stdin.write(f"position fen {fen}\n")
        process.stdin.write(f"perft-auto {depth}\n")
        process.stdin.flush()

        results = process.stdout.readline().strip().split(" ;")

        totalTime += float(results[1])
        totalNodes += int(results[0])

        passed = ("PASS" if int(results[0]) == nodes else "FAIL")

        resultsList.append((str(depth), results[0], results[1], results[2], passed))
        maxLen0 = max(maxLen0, len(resultsList[-1][0]))
        maxLen1 = max(maxLen1, len(resultsList[-1][1]))
        maxLen2 = max(maxLen2, len(resultsList[-1][2]))
        maxLen3 = max(maxLen3, len(resultsList[-1][3]))

    # Print the data with formatting and padding
    for results in resultsList:
        outputFile.write(f"Depth: {results[0].rjust(maxLen0)} | Nodes: {results[1].rjust(maxLen1)} | Time: {results[2].rjust(maxLen2)}s | Nodes/s: {results[3].rjust(maxLen3)} | {results[4]}\n")

    outputFile.write(f"\n{"-" * 80}\n\n")
    outputFile.flush()

process.stdin.write("quit\n")
process.stdin.flush()

outputFile.write(f"Total Time: {totalTime}s\n")
outputFile.write(f"Total Nodes: {totalNodes}\n")
outputFile.write(f"Average Nodes/s: {totalNodes / totalTime}")

outputFile.close()
process.terminate()
