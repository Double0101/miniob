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

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "util/date.h"

RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || update.attr_values.empty()) {
    LOG_WARN("invalid argument. db=%s, table_name=%s, value_num=%d",
             db, table_name, static_cast<int>(update.attr_values.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check fields
  std::vector<UpdateValue> update_values;
  for (AttrValue attr_value : update.attr_values) {
    const char *field_name = attr_value.attribute_name.c_str();
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta) {
      LOG_WARN("no such field. field=%s.%s.%s", db, table_name, field_name);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    const AttrType field_type = field_meta->type();
    const AttrType value_type = attr_value.value.attr_type();
    if (field_type != value_type) {
      switch (field_type) {
        case DATES: {
          int date = -1;
          RC rc = string_to_date(attr_value.value.data(), date);
          if (rc != RC::SUCCESS) {
            LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                     table_name, field_meta->name(), field_type, value_type);
          }
          attr_value.value.set_date(date);
        } break;
        default: {
          LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                   table_name, field_meta->name(), field_type, value_type);
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
    }
    update_values.emplace_back(Field(table, field_meta), attr_value.value);
  }

  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
                             table,
                             nullptr,
                             update.conditions.data(),
                             static_cast<int>(update.conditions.size()),
                             filter_stmt);

  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  UpdateStmt *update_stmt = new UpdateStmt();
  update_stmt->table_ = table;
  update_stmt->update_values_.swap(update_values);
  update_stmt->filter_stmt_ = filter_stmt;
  stmt = update_stmt;
  return RC::SUCCESS;
}
