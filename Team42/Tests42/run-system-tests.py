import subprocess
import sys

files = [
    "affects",
    "and_clauses",
    "BOOLEAN",
    "calls",
    "complex",
    "follows_parent",
    "iter3_system_test_1",
    "iter3_system_test_2",
    "iter3_system_test_3",
    "iter3_system_test_4",
    "next_affects_bip",
    "next",
    "no_common_synonyms",
    "one_common_synonym",
    "pattern",
    "Sample",
    "stress_testing_calls",
    "stress_testing_while",
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