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
// Created by double0101 on 23-10-15.
//

#include "sql/operator/update_physical_operator.h"
#include "sql/stmt/update_stmt.h"
#include "storage/trx/trx.h"


UpdatePhysicalOperator::UpdatePhysicalOperator(Table *table, std::vector<UpdateValue> &&update_values)
    : table_(table), update_values_(std::move(update_values)) {}

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (!children_.empty()) {
    PhysicalOperator *child = children_[0].get();
    RC rc = child->open(trx);
  }
  RC rc;
  RowTuple *tuple;
  Record record;
  while (RC::SUCCESS == (rc = next())) {
    tuple = static_cast<RowTuple *>(current_tuple());
    record = tuple->record();
    rc = trx->delete_record(table_, record);
    if (rc == RC::SUCCESS) {
      table_->modify_record(update_values_.size(), update_values_.data(), record);
      rc = trx->insert_record(table_, record);
    }
  }
  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }
  return children_[0]->next();
}

RC UpdatePhysicalOperator::close()
{
  return RC::SUCCESS;
}

Tuple *UpdatePhysicalOperator::current_tuple()
{
  return children_[0]->current_tuple();
}