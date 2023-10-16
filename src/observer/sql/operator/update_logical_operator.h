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
// Created by double0101 on 23-10-14.
//

#include "storage/table/table.h"
#include "sql/stmt/update_stmt.h"
#include "sql/operator/logical_operator.h"

class UpdateLogicalOperator : public LogicalOperator {
public:
  UpdateLogicalOperator(Table *table, std::vector<UpdateValue> &update_values);
  virtual ~UpdateLogicalOperator() = default;
  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::UPATE;
  }
  Table *table() const { return table_; }
  const std::vector<UpdateValue> &update_values() const { return update_values_; }
  std::vector<UpdateValue> &update_values() { return update_values_; }
private:
  Table *table_ = nullptr;
  std::vector<UpdateValue> &update_values_;
};
