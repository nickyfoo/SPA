import subprocess
import sys

files = [
    # "affectsbip_1",
    # "affectsbip_2",
    # "affectsbip_3",
    # "affectsbip_4",
    # "affectsbip_5",
    # "nextbip_1",
    # "nextbip_2",
    # "nextbip_3",
    # "nextbip_4",
    # "nextbip_5",
    # "testing",
    # "testing2",
    # "BOOLEAN"
    # "SIMPLE_6_0clause",
    # "SIMPLE_6_1clause"
    "callsStar_1"
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