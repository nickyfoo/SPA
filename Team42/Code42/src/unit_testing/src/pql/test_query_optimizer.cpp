#include "query_optimizer.h"
#include "query_preprocessor.h"
#include "pql_query.h"
#include "catch.hpp"

TEST_CASE("GroupingSuchThat_ClausesWithNoSynonyms_AddsOnlyToNoSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(4, 5)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(5);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();

    REQUIRE(clause_groups.at(0)->get_clauses().size() == 1);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(4, 5) and Uses(4, 'v') and Parent(3, 6)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef follows_left_stmt = StmtRef();
    follows_left_stmt.set_stmt_num(4);
    SuchThatRef *follows_left_such_that = new SuchThatRef(follows_left_stmt);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(5);
    SuchThatRef *follows_right_such_that = new SuchThatRef(right_stmt_ref);
    follows->set_ref(follows_left_such_that, follows_right_such_that);

    SuchThatClause *parent = new SuchThatClause("Parent");
    StmtRef parent_left_stmt = StmtRef();
    parent_left_stmt.set_stmt_num(3);
    SuchThatRef *parent_left_such_that = new SuchThatRef(parent_left_stmt);
    StmtRef parent_right_stmt = StmtRef();
    parent_right_stmt.set_stmt_num(6);
    SuchThatRef *parent_right_such_that = new SuchThatRef(parent_right_stmt);
    parent->set_ref(parent_left_such_that, parent_right_such_that);

    SuchThatClause *uses = new SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_stmt_num(4);
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_argument("v");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses->set_ref(uses_left_such_that, uses_right_such_that);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    such_that_clauses->push_back(uses);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 3);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingSuchThat_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(s2, 5)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s2");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(5);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 1);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1, s2; variable v; assign a;\n"
                     "Select s1 such that Follows(s2, a) and Uses(4, v) and Parent(s2, 6)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef follows_left_stmt = StmtRef();
    follows_left_stmt.set_synonym("s2");
    SuchThatRef *follows_left_such_that = new SuchThatRef(follows_left_stmt);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(5);
    SuchThatRef *follows_right_such_that = new SuchThatRef(right_stmt_ref);
    follows->set_ref(follows_left_such_that, follows_right_such_that);

    SuchThatClause *parent = new SuchThatClause("Parent");
    StmtRef parent_left_stmt = StmtRef();
    parent_left_stmt.set_synonym("s2");
    SuchThatRef *parent_left_such_that = new SuchThatRef(parent_left_stmt);
    StmtRef parent_right_stmt = StmtRef();
    parent_right_stmt.set_stmt_num(6);
    SuchThatRef *parent_right_such_that = new SuchThatRef(parent_right_stmt);
    parent->set_ref(parent_left_such_that, parent_right_such_that);

    SuchThatClause *uses = new SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_stmt_num(4);
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_synonym("v");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses->set_ref(uses_left_such_that, uses_right_such_that);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    such_that_clauses->push_back(uses);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 3);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingSuchThat_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(s1, 5)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(5);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1, s2; variable v; assign a;\n"
                     "Select <s1, s2> such that Follows(s1, s2) and Uses(s1, v) and Parent(s2, 6)";
    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef follows_left_stmt = StmtRef();
    follows_left_stmt.set_synonym("s1");
    SuchThatRef *follows_left_such_that = new SuchThatRef(follows_left_stmt);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *follows_right_such_that = new SuchThatRef(right_stmt_ref);
    follows->set_ref(follows_left_such_that, follows_right_such_that);

    SuchThatClause *parent = new SuchThatClause("Parent");
    StmtRef parent_left_stmt = StmtRef();
    parent_left_stmt.set_synonym("s2");
    SuchThatRef *parent_left_such_that = new SuchThatRef(parent_left_stmt);
    StmtRef parent_right_stmt = StmtRef();
    parent_right_stmt.set_stmt_num(6);
    SuchThatRef *parent_right_such_that = new SuchThatRef(parent_right_stmt);
    parent->set_ref(parent_left_such_that, parent_right_such_that);

    SuchThatClause *uses = new SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_synonym("s1");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_synonym("v");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses->set_ref(uses_left_such_that, uses_right_such_that);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("s2",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    such_that_clauses->push_back(uses);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 3);
  }
}

TEST_CASE("GroupingPattern_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1; assign a;\n"
                     "Select s1 pattern a('x', _)";
    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_argument("x");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 1);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1; variable v; assign a; if ifs; while w;\n"
                     "Select s1 pattern a(v, _) and ifs(v, _, _) and w(v, _)";
    auto *pattern1 = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern1_ent_ref = new EntRef();
    pattern1_ent_ref->set_argument("x");
    pattern1->set_ref(pattern1_ent_ref, "_");

    auto *pattern2 = new PatternClause(new EntityDeclaration(EntityType::If, "ifs"));
    auto *pattern2_ent_ref = new EntRef();
    pattern2_ent_ref->set_synonym("v");
    pattern2->set_ref(pattern2_ent_ref, "_");

    auto *pattern3 = new PatternClause(new EntityDeclaration(EntityType::While, "w"));
    auto *pattern3_ent_ref = new EntRef();
    pattern3_ent_ref->set_synonym("v");
    pattern3->set_ref(pattern3_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    pattern_clauses->push_back(pattern1);
    pattern_clauses->push_back(pattern2);
    pattern_clauses->push_back(pattern3);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 3);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingPattern_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a;\n"
                     "Select a pattern a('x', _)";
    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_argument("x");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a; if ifs; while w;\n"
                     "Select <v, a> pattern a(v, _) and ifs(v, _, _) and w(v, _)";
    auto *pattern1 = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern1_ent_ref = new EntRef();
    pattern1_ent_ref->set_synonym("v");
    pattern1->set_ref(pattern1_ent_ref, "_");

    auto *pattern2 = new PatternClause(new EntityDeclaration(EntityType::If, "ifs"));
    auto *pattern2_ent_ref = new EntRef();
    pattern2_ent_ref->set_synonym("v");
    pattern2->set_ref(pattern2_ent_ref, "_");

    auto *pattern3 = new PatternClause(new EntityDeclaration(EntityType::While, "w"));
    auto *pattern3_ent_ref = new EntRef();
    pattern3_ent_ref->set_synonym("v");
    pattern3->set_ref(pattern3_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("v",
                                            EntityType::Variable,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    pattern_clauses->push_back(pattern1);
    pattern_clauses->push_back(pattern2);
    pattern_clauses->push_back(pattern3);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 3);
  }
}

TEST_CASE("GroupingWith_ClausesWithNoSynonyms_AddsOnlyToNoSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a;\n"
                     "Select a with 2 = 3";
    WithClause *with = new WithClause("2", EntityType::None, AttrValueType::Integer,
                                      "3", EntityType::None, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 1);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a;\n"
                     "Select a with 3 = 3 and 2 = 4 and 5 = 5";
    WithClause *with1 = new WithClause("3", EntityType::None, AttrValueType::Integer,
                                       "3", EntityType::None, AttrValueType::Integer);
    WithClause *with2 = new WithClause("2", EntityType::None, AttrValueType::Integer,
                                       "4", EntityType::None, AttrValueType::Integer);
    WithClause *with3 = new WithClause("5", EntityType::None, AttrValueType::Integer,
                                       "5", EntityType::None, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with1);
    with_clauses->push_back(with2);
    with_clauses->push_back(with3);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 3);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingWith_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select a with 2 = s1.stmt#";
    WithClause *with = new WithClause("2", EntityType::None, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 1);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a;\n"
                     "Select BOOLEAN with 3 = a.stmt# and v.varName = 'x' and a.stmt# = 5";
    WithClause *with1 = new WithClause("3", EntityType::None, AttrValueType::Integer,
                                       "a", EntityType::Stmt, AttrValueType::Integer);
    WithClause *with2 = new WithClause("v", EntityType::Variable, AttrValueType::Name,
                                       "x", EntityType::None, AttrValueType::Name);
    WithClause *with3 = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                       "5", EntityType::None, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("",
                                            EntityType::None,
                                            ReturnType::Boolean));
    with_clauses->push_back(with1);
    with_clauses->push_back(with2);
    with_clauses->push_back(with3);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 3);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingWith_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select s1 with a.stmt# = s1.stmt#";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "assign a, a2;\n"
                     "Select <a, a2> with 3 = a.stmt# and a.stmt# = a2.stmt# and a.stmt# = 5";
    WithClause *with1 = new WithClause("3", EntityType::None, AttrValueType::Integer,
                                       "a", EntityType::Stmt, AttrValueType::Integer);
    WithClause *with2 = new WithClause("a", EntityType::Stmt, AttrValueType::Integer,
                                       "a2", EntityType::Stmt, AttrValueType::Integer);
    WithClause *with3 = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                       "5", EntityType::None, AttrValueType::Integer);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("a2",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with1);
    with_clauses->push_back(with2);
    with_clauses->push_back(with3);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 3);
  }
}

TEST_CASE("GroupingNoReturnButConnected_ClausesWithSynonyms_AddsToReturnSynGroup") {
  SECTION("1 Clause with no return syn and 1 Clause with return syn but connected") {
    std::string ss = "assign a; stmt s1, s2;\n"
                     "Select s1 with a.stmt# = s1.stmt# such that Follows(a, s2)";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("a");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 2);
  }

  SECTION("2 Clause with no return syn and 1 Clause with return syn but connected") {
    std::string ss = "assign a; variable v; stmt s1, s2;\n"
                     "Select s1 such that Follows(a, s1) pattern a(v, _) with 3 = s2.stmt#";
    WithClause *with = new WithClause("3", EntityType::None, AttrValueType::Integer,
                                      "s2", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("a");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_synonym("v");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 1);
    REQUIRE(clause_groups[1]->get_clauses()[0].get_clause()->get_type() == ClauseType::WithClause);
    REQUIRE(clause_groups[2]->get_clauses().size() == 2);
  }
}

TEST_CASE("SortingWithinGroup_SortingHasReturnSynonymGroup_AddsOnlyToReturnSynGroup") {
  SECTION("With and 1 synonym Follows clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select s1 such that Follows(s1, 4) with a.stmt# = s1.stmt#";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 2);
    REQUIRE(clause_groups[2]->get_clauses()[0].get_clause()->get_type() == ClauseType::WithClause);
    REQUIRE(clause_groups[2]->get_clauses()[1].get_clause()->get_type() == ClauseType::SuchThatClause);
  }

  SECTION("With, 1 synonym Follows and 1 synonym Parent*") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select <s1, a> such that Parent*(4, a) and Follows(s1, 4) with a.stmt# = s1.stmt#";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    SuchThatClause *parent = new SuchThatClause("Parent*");
    StmtRef left_parent_stmt_ref = StmtRef();
    left_parent_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_parent_such_that_ref = new SuchThatRef(left_parent_stmt_ref);
    StmtRef right_parent_stmt_ref = StmtRef();
    right_parent_stmt_ref.set_synonym("a");
    SuchThatRef *right_parent_such_that_ref = new SuchThatRef(right_parent_stmt_ref);
    parent->set_ref(left_parent_such_that_ref, right_parent_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 3);
    REQUIRE(clause_groups[2]->get_clauses()[0].get_clause()->get_type() == ClauseType::WithClause);
    REQUIRE(clause_groups[2]->get_clauses()[1].get_synonyms_used()[0] == "s1");
    REQUIRE(clause_groups[2]->get_clauses()[2].get_synonyms_used()[0] == "a");
  }

  SECTION("With, 1 synonym Follows, 2 synonym Parent*, 2 synonym pattern") {
    std::string ss = "assign a; stmt s1; variable v;\n"
                     "Select <s1, a> such that Parent*(s1, a) and Follows(s1, 4) with a.stmt# = s1.stmt# pattern a(v, _)";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    SuchThatClause *parent = new SuchThatClause("Parent*");
    StmtRef left_parent_stmt_ref = StmtRef();
    left_parent_stmt_ref.set_synonym("s1");
    SuchThatRef *left_parent_such_that_ref = new SuchThatRef(left_parent_stmt_ref);
    StmtRef right_parent_stmt_ref = StmtRef();
    right_parent_stmt_ref.set_synonym("a");
    SuchThatRef *right_parent_such_that_ref = new SuchThatRef(right_parent_stmt_ref);
    parent->set_ref(left_parent_such_that_ref, right_parent_such_that_ref);

    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_synonym("v");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 4);
    REQUIRE(clause_groups[2]->get_clauses()[0].get_clause()->get_type() == ClauseType::WithClause);
    REQUIRE(clause_groups[2]->get_clauses()[1].get_synonyms_used()[0] == "s1");
    REQUIRE(clause_groups[2]->get_clauses()[2].get_clause()->get_type() == ClauseType::PatternClause);
    REQUIRE(clause_groups[2]->get_clauses()[3].get_synonyms_used().size() == 2);
    std::shared_ptr<SuchThatClause> such_that_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause_groups[2]->get_clauses()[3].get_clause());
    REQUIRE(such_that_clause->get_type() == RelRef::ParentT);
  }

  SECTION("With, 2 synonym Follows, 1 synonym Parent*, 1 synonym pattern, 2 synonym Affects") {
    std::string ss = "assign a, a1; stmt s1;\n"
                     "Select <s1, a> such that Follows(s1, a) and Affects(a, a1) and Parent*(s1, 3) with a.stmt# = 3 pattern a1(_, 'x')";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "3", EntityType::None, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("a");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    SuchThatClause *parent = new SuchThatClause("Parent*");
    StmtRef left_parent_stmt_ref = StmtRef();
    left_parent_stmt_ref.set_synonym("s1");
    SuchThatRef *left_parent_such_that_ref = new SuchThatRef(left_parent_stmt_ref);
    StmtRef right_parent_stmt_ref = StmtRef();
    right_parent_stmt_ref.set_stmt_num(3);
    SuchThatRef *right_parent_such_that_ref = new SuchThatRef(right_parent_stmt_ref);
    parent->set_ref(left_parent_such_that_ref, right_parent_such_that_ref);

    SuchThatClause *affects = new SuchThatClause("Affects");
    StmtRef left_affects_stmt_ref = StmtRef();
    left_affects_stmt_ref.set_synonym("a");
    SuchThatRef *left_affects_such_that_ref = new SuchThatRef(left_affects_stmt_ref);
    StmtRef right_affects_stmt_ref = StmtRef();
    right_affects_stmt_ref.set_synonym("a1");
    SuchThatRef *right_affects_such_that_ref = new SuchThatRef(right_affects_stmt_ref);
    affects->set_ref(left_affects_such_that_ref, right_affects_such_that_ref);

    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a1"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_wild_card();
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    return_syns->push_back(new ResultClause("a",
                                            EntityType::Assign,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    such_that_clauses->push_back(affects);
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();

    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 5);
    REQUIRE(clause_groups[2]->get_clauses()[0].get_clause()->get_type() == ClauseType::WithClause);
    std::shared_ptr<SuchThatClause> follows_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause_groups[2]->get_clauses()[1].get_clause());
    REQUIRE(follows_clause->get_type() == RelRef::Follows);
    std::shared_ptr<SuchThatClause> parent_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause_groups[2]->get_clauses()[2].get_clause());
    REQUIRE(parent_clause->get_type() == RelRef::ParentT);
    std::shared_ptr<SuchThatClause> affects_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause_groups[2]->get_clauses()[3].get_clause());
    REQUIRE(affects_clause->get_type() == RelRef::Affects);
    REQUIRE(clause_groups[2]->get_clauses()[4].get_clause()->get_type() == ClauseType::PatternClause);
  }
}

TEST_CASE("SortingWithinGroup_SortingNoReturnSynonymGroup_AddsOnlyToNoReturnSynGroup") {
  SECTION("With and 1 synonym Follows clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select s1 such that Follows(a, 4) with a.stmt# = 4";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "4", EntityType::None, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("a");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s1",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 2);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("With, 1 synonym Follows and 1 synonym Parent*") {
    std::string ss = "assign a; stmt s1, s2;\n"
                     "Select s2 such that Parent*(4, a) and Follows(s1, 4) with a.stmt# = s1.stmt#";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    SuchThatClause *parent = new SuchThatClause("Parent*");
    StmtRef left_parent_stmt_ref = StmtRef();
    left_parent_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_parent_such_that_ref = new SuchThatRef(left_parent_stmt_ref);
    StmtRef right_parent_stmt_ref = StmtRef();
    right_parent_stmt_ref.set_synonym("a");
    SuchThatRef *right_parent_such_that_ref = new SuchThatRef(right_parent_stmt_ref);
    parent->set_ref(left_parent_such_that_ref, right_parent_such_that_ref);

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s2",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 3);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }

  SECTION("With, 1 synonym Follows, 2 synonym Parent*, 2 synonym pattern") {
    std::string ss = "assign a; stmt s1, s2; variable v;\n"
                     "Select s2 such that Parent*(s1, a) and Follows(s1, 4) "
                     "with a.stmt# = s1.stmt# pattern a(v, _)";
    WithClause *with = new WithClause("a", EntityType::Assign, AttrValueType::Integer,
                                      "s1", EntityType::Stmt, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    SuchThatClause *parent = new SuchThatClause("Parent*");
    StmtRef left_parent_stmt_ref = StmtRef();
    left_parent_stmt_ref.set_synonym("s1");
    SuchThatRef *left_parent_such_that_ref = new SuchThatRef(left_parent_stmt_ref);
    StmtRef right_parent_stmt_ref = StmtRef();
    right_parent_stmt_ref.set_synonym("a");
    SuchThatRef *right_parent_such_that_ref = new SuchThatRef(right_parent_stmt_ref);
    parent->set_ref(left_parent_such_that_ref, right_parent_such_that_ref);

    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_synonym("v");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s2",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    such_that_clauses->push_back(parent);
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 4);
    REQUIRE(clause_groups[2]->get_clauses().size() == 0);
  }
}

TEST_CASE("DefaultGrouping_NoOptimization_AddsOnlyToLastGroup") {
  SECTION("No synonym With, 1 synonym Follows, 2 synonym pattern") {
    std::string ss = "assign a; stmt s1, s2; variable v;\n"
                     "Select s1 such that Follows(s1, 4) "
                     "with 2 = 3 pattern a(v, _)";
    WithClause *with = new WithClause("2", EntityType::None, AttrValueType::Integer,
                                       "3", EntityType::None, AttrValueType::Integer);

    SuchThatClause *follows = new SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows->set_ref(left_such_that_ref, right_such_that_ref);

    auto *pattern = new PatternClause(new EntityDeclaration(EntityType::Assign, "a"));
    auto *pattern_ent_ref = new EntRef();
    pattern_ent_ref->set_synonym("v");
    pattern->set_ref(pattern_ent_ref, "_");

    auto *such_that_clauses = new std::vector<SuchThatClause *>();
    auto *pattern_clauses = new std::vector<PatternClause *>();
    auto *with_clauses = new std::vector<WithClause *>();
    auto *return_syns = new std::vector<ResultClause *>();
    return_syns->push_back(new ResultClause("s2",
                                            EntityType::Stmt,
                                            ReturnType::Default));
    with_clauses->push_back(with);
    such_that_clauses->push_back(follows);
    pattern_clauses->push_back(pattern);

    QueryOptimizer query_optimizer = QueryOptimizer(such_that_clauses,
                                                    pattern_clauses,
                                                    with_clauses,
                                                    return_syns);
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateDefaultGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0]->get_clauses().size() == 0);
    REQUIRE(clause_groups[1]->get_clauses().size() == 0);
    REQUIRE(clause_groups[2]->get_clauses().size() == 3);
  }
}
