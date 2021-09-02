#include "StmtNum.h"

StmtNum::StmtNum(int stmt_num) {
    this->stmt_num = stmt_num;
}

int StmtNum::getStmtNum() {
    return this->stmt_num;
}