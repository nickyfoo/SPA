#include <vector>

#include <statement.h>
#include <parse.h>
#include <pkb.h>
#include <design_extractor.h>

#include <query_evaluator.h>
#include <query_preprocessor.h>
#include <query_optimizer.h>

enum ProgramSource {
  kSampleSource,
  kNestedSource,
  kSimpleContainer,
  kUndefinedProc,
  kCyclicProcs,
  kAdvancedSample,
  kNestedIf,
  kNestedWhileIf,
  kCfgBipSample,
  kCfgBipSecondSample
};

static ProgramNode *BuildProgAst(ProgramSource source_name) {
  std::string sample_source =
      "procedure main {"
      "  flag = 0;"
      "  call computeCentroid;"
      "  call printResults;"
      "}"
      "procedure readPoint {"
      "  read x;"
      "  read y;"
      "}"
      "procedure printResults {"
      "  print flag;"
      "  print cenX;"
      "  print cenY;"
      "  print normSq;"
      "}"
      "procedure computeCentroid {"
      "  count = 0;"
      "  cenX = 0;"
      "  cenY = 0;"
      "  call readPoint;"
      "  while((x != 0) && (y != 0)) {"
      "    count = count+1;"
      "    cenX = cenX + x;"
      "    cenY = cenY + y;"
      "    call readPoint;"
      "  }"
      "  if (count == 0) then {"
      "    flag = 1;"
      "  } else {"
      "    cenX = cenX / count;"
      "    cenY = cenY / count;"
      "  }"
      "  normSq = cenX * cenX + cenY * cenY;"
      "}";

  std::string nested_source =
      "procedure nestedproc{"
      "  i = 0;"
      "  j = 0;"
      "  k = 0;"
      "  while(j < n){"
      "    j = j + 1;"
      "    while(k < n){"
      "      if(i <= k) then {"
      "        d = d + 1;"
      "      } else {"
      "        e = e + 1;"
      "      }"
      "    }"
      "  }"
      "  while(j < n){"
      "    j = i + k;"
      "    if (j < n) then {"
      "      if(i <= k) then {"
      "        d = d + 1;"
      "      } else {"
      "        e = e + 1;"
      "      }"
      "    } else {"
      "      f = f + 1;"
      "    }"
      "  }"
      "}";

  std::string simple_container =
      "procedure main {"
      "  if((x==0) && (y==0) ) then {"
      "    a = b + c;"
      "  } else {"
      "    d = e + f;}"
      "}";

  std::string undefined_proc =
      "procedure main {"
      "  call undefinedproc;"
      "}";

  std::string cyclic_procs =
      "procedure proc1 {"
      "  call proc2;"
      "}"
      "procedure proc2 {"
      "  call proc3;"
      "}"
      "procedure proc3 {"
      "  call proc1;"
      "}";

  std::string cfg_sample =
      "procedure First {"
      "  read x;"
      "  read y;"
      "  call Second; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1; "
      "  }"
      "  if (x == 1) then {"
      "    x = x + 1; }"
      "  else {"
      "    z = 1;"
      "  }"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v; }";

  std::string nested_if =
      "procedure main {"
      "  zero = 1;"
      "  if(a==1) then {"
      "    if(b==1) then {"
      "      first = 1;"
      "    }else{"
      "      second = 1;"
      "    }"
      "  } else {"
      "    if(c==1) then {"
      "      third = 1;"
      "    } else {"
      "      fourth = 1;"
      "    }"
      "  }"
      "  fifth = 1;"
      "}";

  std::string nested_if_while =
      "procedure main {"
      "  x = 0;"
      "  read y;"
      "  call Second;"
      "  x = x + 1; }"
      "procedure Second {"
      "  x = 0;"
      "  i = 5;"
      "  while (i != 0) {"
      "    x = x + 2 * y;"
      "    call Third;"
      "    i = i - 1;"
      "    if (z == y) then {"
      "      j = 8;"
      "    } else {"
      "      y = j;"
      "    }}"
      "  if (x == 1) then {"
      "    x = x + 1;"
      "    z = y + c + v;"
      "  } else {"
      "    z = z + i;}"
      "  z = z + x + i;"
      "  y = z + 2;"
      "  x = x * y + z; }"
      "procedure Third {"
      "  z = 5;"
      "  v = z;"
      "  print v;}";

  std::string cfg_bip_sample =
      "procedure Bill {"
      "  x = 5;"
      "  call Mary;"
      "  y = x + 6;"
      "  call John;"
      "  z = x * y + 2;"
      "}"
      "procedure Mary {"
      "  y = x * 3;"
      "  call John;"
      "  z = x + y;"
      "}"
      "procedure John {"
      "  if (i > 0) then {"
      "    x = x + z;"
      "  } else {"
      "    y = x * y;"
      "  }"
      "}";

  std::string cfg_bip_second_sample =
      "procedure B {"
      "  call C;"
      "  call C;"
      "  call C;"
      "}"
      "procedure C {"
      "d = a;"
      "a = b;"
      "b = c;"
      "c = d;"
      "}";

  std::string program;
  switch (source_name) {
    case kSampleSource:
      program = sample_source;
      break;
    case kNestedSource:
      program = nested_source;
      break;
    case kSimpleContainer:
      program = simple_container;
      break;
    case kUndefinedProc:
      program = undefined_proc;
      break;
    case kCyclicProcs:
      program = cyclic_procs;
      break;
    case kAdvancedSample:
      program = cfg_sample;
      break;
    case kNestedIf:
      program = nested_if;
      break;
    case kNestedWhileIf:
      program = nested_if_while;
      break;
    case kCfgBipSample:
      program = cfg_bip_sample;
      break;
    case kCfgBipSecondSample:
      program = cfg_bip_second_sample;
      break;
    default:
      break;
  }

  BufferedLexer lexer(program);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  return p;
}

static PKB InitialisePKB(ProgramSource source_name) {
  ProgramNode *p = BuildProgAst(source_name);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();

  return pkb;
}

static std::vector<std::string> *EvaluateQuery(std::string ss) {
  std::string source = "procedure main {"
                       "flag = 0;"
                       "call computeCentroid;"
                       "call printResults;"
                       "}"
                       "procedure readPoint {"
                       "read x;"
                       "read y;"
                       "}"
                       "procedure printResults {"
                       "print flag;"
                       "print cenX;"
                       "print cenY;"
                       "print normSq;"
                       "}"
                       "procedure computeCentroid {"
                       "count = 0;"
                       "cenX = 0;"
                       "cenY = 0;"
                       "call readPoint;"
                       "while((x != 0) && (y != 0)) {"
                       "count = count+1;"
                       "cenX = cenX + x;"
                       "cenY = cenY + y;"
                       "call readPoint;"
                       "}"
                       "if (count == 0) then {"
                       "flag = 1;"
                       "} else {"
                       "cenX = cenX / count;"
                       "cenY = cenY / count;"
                       "}"
                       "normSq = cenX * cenX + cenY * cenY;"
                       "}";

  auto query = QueryPreprocessor(std::move(ss));
  std::tuple<std::vector<ResultClause *> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> *clause = query.get_clauses();
  QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clause),
                                                  std::get<2>(*clause),
                                                  std::get<3>(*clause),
                                                  std::get<0>(*clause));
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  auto *pql_query = new PQLQuery(std::get<0>(*clause),
                                 clause_groups,
                                 std::get<4>(*clause),
                                 std::get<5>(*clause),
                                 std::get<6>(*clause));

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);

  // Initialise PKB and extract relationships
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();

  // Set up query evaluator and evaluate
  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  return ret;
}


static std::vector<std::string> *EvaluateBipQuery(std::string ss) {
  std::string source = "procedure foo {\n"
                                   "  read x;\n"
                                   "  read y;\n"
                                   "  length = x * x + y * y;\n"
                                   "  t = 30;\n"
                                   "  call bar;\n"
                                   "  print length;\n"
                                   "}\n"
                                   "\n"
                                   "procedure bar {\n"
                                   "  if ((x >= 0) && (y >= 0)) then {\n"
                                   "    while (x != 0) {\n"
                                   "      tan = y / x;\n"
                                   "      sin = y / length;\n"
                                   "      cos = x / length;\n"
                                   "      call ellipse;\n"
                                   "      if (((sin * sin + cos * cos) == 1) || (!(tan != (sin / cos)))) then {\n"
                                   "        x = y / 2;\n"
                                   "        y = x / 2;\n"
                                   "      } else {\n"
                                   "        x = 0;\n"
                                   "      }\n"
                                   "      length = x * x + y * y;\n"
                                   "    }\n"
                                   "  } else {\n"
                                   "    x = 10;\n"
                                   "    y = 10;\n"
                                   "    length = x * x + y * y;\n"
                                   "  }\n"
                                   "}\n"
                                   "\n"
                                   "procedure ellipse {\n"
                                   "  pi = 3;\n"
                                   "  area = pi * x * y;\n"
                                   "  if (area < 10) then {\n"
                                   "    x = x + 1;\n"
                                   "    y = y + y / x + 1;\n"
                                   "  } else {\n"
                                   "    print area;\n"
                                   "  }\n"
                                   "}";

  auto query = QueryPreprocessor(std::move(ss));
  std::tuple<std::vector<ResultClause *> *,
             std::vector<SuchThatClause *> *,
             std::vector<PatternClause *> *,
             std::vector<WithClause *> *,
             std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> *clause = query.get_clauses();
  QueryOptimizer query_optimizer = QueryOptimizer(std::get<1>(*clause),
                                                  std::get<2>(*clause),
                                                  std::get<3>(*clause),
                                                  std::get<0>(*clause));
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups = query_optimizer.CreateOptimizedGroupings();
  auto *pql_query = new PQLQuery(std::get<0>(*clause),
                                 clause_groups,
                                 std::get<4>(*clause),
                                 std::get<5>(*clause),
                                 std::get<6>(*clause));

  // Parse source
  BufferedLexer lexer(source);
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);

  // Initialise PKB and extract relationships
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();

  // Set up query evaluator and evaluate
  auto evaluator = new QueryEvaluator(pql_query, &pkb);
  std::vector<std::string> *ret = evaluator->Evaluate();

  return ret;
}