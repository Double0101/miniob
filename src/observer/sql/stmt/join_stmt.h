/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include <vector>
#include <unordered_map>
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"
#include "sql/stmt/filter_stmt.h"

/**
 * @brief Filter/谓词/过滤语句
 * @ingroup Statement
 */
class JoinStmt
{
public:
  JoinStmt() = default;
  virtual ~JoinStmt();

public:
  const std::vector<FilterUnit *> &filter_units() const
  {
    return filter_units_;
  }
  const JoinOp &join_type() const
  {
    return join_type_;
  }

public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
                   const ConditionSqlNode *conditions, int condition_num, JoinOp join_op, JoinStmt *&stmt);

private:
  std::vector<FilterUnit *> filter_units_;  // 默认当前都是AND关系
  JoinOp join_type_ = NO_JOIN;
};