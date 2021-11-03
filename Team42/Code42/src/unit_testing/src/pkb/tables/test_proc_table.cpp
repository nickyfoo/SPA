#include <vector>
#include "proc_table.h"
#include "catch.hpp"

TEST_CASE("GetNumProcedures_DifferentSizeTables_ReturnsNumProcedures", "[procedure][table]") {
  SECTION("Table size: 1") {
    std::string proc_name = "main";
    ProcTable proc_table;
    proc_table.AddProcedure(proc_name, 1);

    REQUIRE(proc_table.get_num_procedures() == 1);
  }

  SECTION("Table size: 5") {
    std::set<std::string> proc_names{"apple", "banana", "carrot", "donut", "either"};
    ProcTable proc_table;
    for (const std::string &proc_name: proc_names) proc_table.AddProcedure(proc_name, 1);

    REQUIRE(proc_table.get_num_procedures() == proc_names.size());
  }
}

TEST_CASE("AddGetAllProcedures_DifferentSizeTables_ReturnsVectorProcedures", "[procedure][table]") {
  SECTION("Table size: 1") {
    std::string proc_name = "main";
    ProcTable proc_table;
    proc_table.AddProcedure(proc_name, 1);
    std::vector<Procedure *> procs_vec = proc_table.get_all_procedures();

    REQUIRE(procs_vec.size() == 1);
    REQUIRE(std::any_of(procs_vec.begin(),
                        procs_vec.end(),
                        [&](Procedure *elem) { return elem->get_name() == proc_name; }));
  }

  SECTION("Table size: 5") {
    std::set<std::string> proc_names{"apple", "banana", "carrot", "donut", "either"};
    ProcTable proc_table;
    for (const std::string &proc_name: proc_names) proc_table.AddProcedure(proc_name, 1);
    std::vector<Procedure *> procs_vec = proc_table.get_all_procedures();

    REQUIRE(procs_vec.size() == proc_names.size());
    for (std::string proc_name: proc_names) {
      REQUIRE(std::any_of(procs_vec.begin(),
                          procs_vec.end(),
                          [&](Procedure *elem) { return elem->get_name() == proc_name; }));
    }
  }
}

TEST_CASE("GetProcedures_DifferentSizeTables_ReturnsProcedure", "[procedure][table]") {
  SECTION("Table size: 1") {
    std::string proc_name = "main";
    ProcTable proc_table;
    proc_table.AddProcedure(proc_name, 1);

    REQUIRE(proc_table.get_procedure(proc_name)->get_name() == proc_name);
  }

  SECTION("Table size: 5") {
    std::set<std::string> proc_names{"apple", "banana", "carrot", "donut", "either"};
    ProcTable proc_table;
    for (const std::string &proc_name: proc_names) proc_table.AddProcedure(proc_name, 1);

    for (const std::string &proc_name: proc_names) {
      REQUIRE(proc_table.get_procedure(proc_name)->get_name() == proc_name);
    }
  }
}
