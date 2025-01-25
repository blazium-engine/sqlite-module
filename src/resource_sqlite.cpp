/**************************************************************************/
/*  resource_sqlite.cpp                                                   */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "resource_sqlite.h"
#include "core/io/file_access.h"

void SQLiteDatabase::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_sqlite"), &SQLiteDatabase::get_sqlite);
    ClassDB::bind_method(D_METHOD("create_table", "table_name", "columns"), &SQLiteDatabase::create_table);
    ClassDB::bind_method(D_METHOD("get_table_names"), &SQLiteDatabase::get_table_names);

    ClassDB::bind_method(D_METHOD("set_resource", "path"), &SQLiteDatabase::set_resource);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "table_names", PROPERTY_HINT_ARRAY_TYPE, "String"), "", "get_table_names");
    ClassDB::bind_static_method("SQLiteDatabase", D_METHOD("create"), &SQLiteDatabase::create);
}

Ref<SQLiteDatabase> SQLiteDatabase::create() {
    Ref<SQLite> db;
    db.instantiate();
    db->open_in_memory();
    Ref<SQLiteDatabase> sqlite_database;
    sqlite_database.instantiate();
    sqlite_database->db = db;
    return sqlite_database;
}

void SQLiteDatabase::set_resource(const String &p_path) {
    db->open(p_path);
    emit_changed();
}

Ref<SQLite> SQLiteDatabase::get_sqlite() {
    return db;
}

void SQLiteDatabase::create_table(const String &p_table_name, const TypedArray<SQLiteColumnSchema> &p_columns) {
	String query_string, type_string, key_string, primary_string;
	/* Create SQL statement */
	query_string = "CREATE TABLE IF NOT EXISTS " + p_table_name + " (";
	key_string = "";
	primary_string = "";

	Dictionary column_dict;
	int primary_key_columns = 0;
	for (int64_t i = 0; i < p_columns.size(); i++) {
        Ref<SQLiteColumnSchema> schema = p_columns[i];
		if (schema->is_primary_key()) {
			primary_key_columns++;
		}
	}
	for (int64_t i = 0; i < p_columns.size(); i++) {
        Ref<SQLiteColumnSchema> schema = p_columns[i];
		query_string += (const String &)schema->get_name() + String(" ");
        switch (schema->get_type()) {
            case Variant::NIL:
                type_string = "NULL";
                break;
            case Variant::INT:
                type_string = "INTEGER";
                break;
            case Variant::FLOAT:
                type_string = "REAL";
                break;
            case Variant::STRING:
                type_string = "TEXT";
                break;
            case Variant::PACKED_BYTE_ARRAY:
                type_string = "BLOB";
                break;
            default:
                ERR_PRINT("Invalid column type.");
                type_string = "NULL";
                break;
        }
        query_string += type_string;

		/* Primary key check */
		if (schema->is_primary_key()) {
			if (primary_key_columns == 1) {
				query_string += String(" PRIMARY KEY");
				/* Autoincrement check */
				if (schema->is_auto_increment()) {
					query_string += String(" AUTOINCREMENT");
				}
			} else {
				if (primary_string.is_empty()) {
					primary_string = schema->get_name();
				} else {
					primary_string += String(", ") + schema->get_name();
				}
			}
		}
		/* Not null check */
		if (schema->is_not_null()) {
			query_string += String(" NOT NULL");
		}
		/* Unique check */
		if (schema->is_unique()) {
			query_string += String(" UNIQUE");
		}
		/* Default check */
		if (!schema->get_default_value().is_null()) {
			query_string += String(" DEFAULT ") + schema->get_default_value().to_json_string();
		}

		if (i != p_columns.size() - 1) {
			query_string += ",";
		}
	}

	if (primary_key_columns > 1) {
		query_string += String(", PRIMARY KEY (") + primary_string + String(")");
	}

	query_string += key_string + ");";
    Ref<SQLiteQuery> query = db->create_query(query_string);
    query->execute(Array());
}

TypedArray<String> SQLiteDatabase::get_table_names() const {
    Array result = db->create_query("SELECT name FROM sqlite_master WHERE type = \"table\"")->execute(Array());
    TypedArray<String> table_names;
    for (int i = 0; i < result.size(); i++) {
        Array row = result[i];
        for (int j = 0; j < row.size(); j++) {
            String name = row[j];
            table_names.append(name);
        }
    }
    return table_names;
}


SQLiteDatabase::SQLiteDatabase() {
    db.instantiate();
}

SQLiteDatabase::~SQLiteDatabase() {
    db.unref();
}
