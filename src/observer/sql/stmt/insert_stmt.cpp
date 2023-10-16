/* Copyright (c) 2021OceanBase and/or its affiliates. All rights reserved.
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

#include "sql/stmt/insert_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "util/date.h"

InsertStmt::InsertStmt(Table *table, std::vector<Value *> records, int value_amount)
    : table_(table), records_(records), value_amount_(value_amount)
{}

RC InsertStmt::create(Db *db, InsertSqlNode &inserts, Stmt *&stmt)
{
  const char *table_name = inserts.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || inserts.insert_records.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, static_cast<int>(inserts.insert_records.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the fields number
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.sys_field_num();
  for (std::vector<Value> &record : inserts.insert_records) {
    if (field_num != record.size()) {
      LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", record.size(), field_num);
      return RC::SCHEMA_FIELD_MISSING;
    }
  }
  const int record_num = static_cast<int>(inserts.insert_records.size());
  const int value_num = static_cast<int>(inserts.insert_records[0].size());
  std::vector<Value*> records;
  // check fields type
  const int sys_field_num = table_meta.sys_field_num();
  for (int i = 0; i < record_num; ++i) {
    Value *values = inserts.insert_records[i].data();
    for (int j = 0; j < value_num; j++) {
      const FieldMeta *field_meta = table_meta.field(j + sys_field_num);
      const AttrType field_type = field_meta->type();
      const AttrType value_type = values[j].attr_type();
      if (field_type == value_type) {
        continue;
      }
      switch (field_type) {
        case DATES: {
          int date = -1;
          RC rc = string_to_date(values[j].data(), date);
          if (rc != RC::SUCCESS) {
            LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                     table_name, field_meta->name(), field_type, value_type);
          }
          values[j].set_date(date);
        }
          break;
        default: {
          LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                   table_name, field_meta->name(), field_type, value_type);
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        } break;
      }
    }
    records.push_back(values);
  }

  // everything alright
  stmt = new InsertStmt(table, records, value_num);
  return RC::SUCCESS;
}
