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
    "testing",
    # "testing2",
    # "BOOLEAN"
    # "SIMPLE_6_0clause",
    # "SIMPLE_6_1clause"
    # "callsStar_1"
    # "SIMPLE_WITH"
    # "andPattern_invalid"
    # "SIMPLE_7",
    # "with_0"
    # "iter3_system_test_1",
    # "iter3_system_test_2",
    # "iter3_system_test_3",
    # "iter3_system_test_4",
    # "invalid_chars_0"
    # "patternwith_4",
    # "patternwith_5"
    # "select_invalid"
    # "simple_logicExprNoBracket",
    "andWith_invalid"
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