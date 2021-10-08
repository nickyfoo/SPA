#include "TestWrapper.h"
#include "parse.h"
#include "pkb.h"
#include "fstream"
#include "string"
#include "sstream"
#include "query_preprocessor.h"
#include "query_evaluator.h"
#include "query_optimizer.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper *WrapperFactory::wrapper = 0;
AbstractWrapper *WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program

}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  // call your parser to do the parsing
  // ...rest of your code...
  const std::ifstream input_stream(filename, std::ios_base::binary);

  if (input_stream.fail()) {
    throw std::runtime_error("Failed to open file");
  }

  std::stringstream buffer;
  buffer << input_stream.rdbuf();
  std::string source = buffer.str();

  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);

  this->pkb_ = new PKB(p);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string> &results) {
// call your evaluator to evaluate the query here
  // ...code to evaluate query...

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
  auto *query_preprocessor = new QueryPreprocessor(query);
  std::tuple<std::vector<std::string> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> *clauses = query_preprocessor->get_clauses();
  PQLQuery *pql_query;
  if (clauses == nullptr) {
    pql_query = nullptr;
  } else {
    QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clauses),
                                                    std::get<2>(*clauses),
                                                    std::get<3>(*clauses),
                                                    std::get<0>(*clauses));
    std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateGroupings();
    pql_query = new PQLQuery(std::get<0>(*clauses),
                             clause_groups,
                             std::get<4>(*clauses),
                             std::get<5>(*clauses),
                             std::get<6>(*clauses));
  }
  QueryEvaluator *query_evaluator = new QueryEvaluator(pql_query, pkb_);
  std::vector<std::string> *res = query_evaluator->Evaluate();
  std::copy(res->begin(), res->end(), std::back_inserter(results));
}
