#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "QueryObject.h"

class QueryReceiver {

    int main(int argc, char *argv[])
    {
        std::stringstream ss;

        for (std::string line; std::getline(std::cin, line);) {
            ss << line << " ";
        }
        QueryObject* query = new QueryObject(ss.str());
        tuple<string, string, string> clause = query->getClauses();
    }

};

