import subprocess
import sys

files = [
    "and_clauses",
    "BOOLEAN",
    "calls",
    "complex",
    "follows_parent",
    "next",
    "no_common_synonyms",
    "one_common_synonym",
    "pattern",
    "Sample",
    "test1",
    "test2",
    "test3",
    "tuple",
    "uses_modifies",
    "with",
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