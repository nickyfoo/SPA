import subprocess
import sys

files = [
    "parent_1",
    "parent_2",
    "parent_3",
    "parent_4",
    "parent_5",
]

for file in files:
    file_path = "log/" + file
    sys.stdout = open(file_path, 'w')
    autotester = "../Code42/cmake-build-debug/src/autotester/autotester"
    source = file + "_source.txt"
    query = file + "_queries.txt"
    out = "out/" + file + ".xml"
    print(subprocess.run([autotester, source, query, out]))
    sys.stdout.close()