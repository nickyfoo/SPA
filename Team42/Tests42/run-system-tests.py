import subprocess
import sys

files1 = [
    "affects",
    "affectss",
    "and_clauses",
    "Basic_BOOLEAN",
    "Basic_Next",
    "Basic_pattern_if_while",
    "Basic_tuple",
    "Basic_with",
    "bonus_3_features_queries",
    "BOOLEAN",
    "chaining",
    "complex_condexpr_nested",
    "complex",
    "Container",
    "follows_parent",
    "Follows_Parent",
    "Follows",
    "Invalid",
    "iter2Calls",
    "iter2Invalid",
    "iter2Modifies",
    "iter2MultipleClauses",
    "iter2Next",
    "iter2Tuple",
    "iter2Uses",
    "Medium_Calls",
    "Medium_Modifies",
    "Medium_MultipleClauses",
    "Medium_Next",
    "Medium_Select_attrRef",
    "Medium_Uses",
    "Medium_with",
    "Mix_and_edge_1",
    "Mix_and_edge_2",
    "Mix_and_edge_3",
    "Mix_and_edge_4",
    "Mix_and_edge_5",
    "Modifies",
    "MultipleClauses",
    "next_source",
    "nexts",
    "no_common_synonyms",
    "NoClauses",
    "one_common_synonym",
    "Parent",
    "Pattern_Assign",
    "Pattern_if",
    "pattern_ModifiesS_UsesS",
    "pattern",
    "pattern_suchthat_clauses",
    "Pattern_while",
    "Sample",
    "simple_while",
    "test1",
    "test2",
    "test3",
    "tuple_others",
    "tuple",
    "uncommon_invalid",
    "uses_modifies",
    "Uses",
    "with",
    "next"
]

files2 = [
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "24",
]

for file in files1:
    file_path = "log/" + file
    sys.stdout = open(file_path, 'w')
    autotester = "../Code42/cmake-build-debug/src/autotester/autotester"
    source = file + "_source.txt"
    query = file + "_queries.txt"
    out = "out/" + file + ".xml"
    print(subprocess.run([autotester, source, query, out]))
    sys.stdout.close()