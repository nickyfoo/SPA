#include "TestWrapper.h"
#include "../../spa/src/PQL/pql_query.h"
#include "../../spa/src/PQL/query_preprocessor.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
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
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
// call your evaluator to evaluate the query here
  // ...code to evaluate query...

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.

  auto* query_preprocessor = new QueryPreprocessor(query);
  PQLQuery *clause = query_preprocessor->get_pql_query();
  QueryEvaluator query_evaluator = new QueryEvaluator(*clause);
  std::vector<std::string> res = query_evaluator.evaluate();
  std::copy(res.begin(), res.end(), std::back_inserter(results));

}
