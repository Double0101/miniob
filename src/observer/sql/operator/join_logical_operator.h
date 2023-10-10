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
// Created by Wangyunlai on 2022/12/07
//

#pragma once

#include "sql/operator/logical_operator.h"

/**
 * @brief 连接算子
 * @ingroup LogicalOperator
 * @details 连接算子，用于连接两个表。对应的物理算子或者实现，可能有NestedLoopJoin，HashJoin等等。
 */
class JoinLogicalOperator : public LogicalOperator 
{
public:
  JoinLogicalOperator() = default;
  virtual ~JoinLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::JOIN;
  }
  JoinOp join_type() const { return join_type_; }
  void set_join_type(JoinOp join_type) { join_type_ = join_type; }
  void set_condition(std::unique_ptr<Expression> join_condition)
  {
    join_conditions_.push_back(std::move(join_condition));
  }

  std::vector<std::unique_ptr<Expression>> &join_conditions()
  {
    return join_conditions_;
  }
private:
  JoinOp join_type_ = NO_JOIN;
  std::vector<std::unique_ptr<Expression>> join_conditions_;
};
