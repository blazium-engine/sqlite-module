/**************************************************************************/
/*  resource_sqlite.h                                                     */
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

#ifndef SQLITE_RESOURCE_H
#define SQLITE_RESOURCE_H

#include "core/io/resource.h"
#include "core/variant/variant.h"
#include "godot_sqlite.h"
#include "core/variant/typed_array.h"

class SQLiteColumnSchema: public RefCounted {
    GDCLASS(SQLiteColumnSchema, RefCounted);
    String name;
    Variant::Type type;
    Variant default_value;
    bool primary_key = false;
    bool auto_increment = false;
    bool not_null = false;
    bool unique = false;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_name"), &SQLiteColumnSchema::get_name);
        ClassDB::bind_method(D_METHOD("get_type"), &SQLiteColumnSchema::get_type);
        ClassDB::bind_method(D_METHOD("get_default_value"), &SQLiteColumnSchema::get_default_value);
        ClassDB::bind_method(D_METHOD("is_primary_key"), &SQLiteColumnSchema::is_primary_key);
        ClassDB::bind_method(D_METHOD("is_auto_increment"), &SQLiteColumnSchema::is_auto_increment);
        ClassDB::bind_method(D_METHOD("is_not_null"), &SQLiteColumnSchema::is_not_null);
        ClassDB::bind_method(D_METHOD("is_unique"), &SQLiteColumnSchema::is_unique);

        ClassDB::bind_method(D_METHOD("set_name", "name"), &SQLiteColumnSchema::set_name);
        ClassDB::bind_method(D_METHOD("set_type", "type"), &SQLiteColumnSchema::set_type);
        ClassDB::bind_method(D_METHOD("set_default_value", "default_value"), &SQLiteColumnSchema::set_default_value);
        ClassDB::bind_method(D_METHOD("set_primary_key", "primary_key"), &SQLiteColumnSchema::set_primary_key);
        ClassDB::bind_method(D_METHOD("set_auto_increment", "auto_increment"), &SQLiteColumnSchema::set_auto_increment);
        ClassDB::bind_method(D_METHOD("set_not_null", "not_null"), &SQLiteColumnSchema::set_not_null);
        ClassDB::bind_method(D_METHOD("set_unique", "unique"), &SQLiteColumnSchema::set_unique);

        ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "type"), "set_type", "get_type");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "default_value"), "set_default_value", "get_default_value");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "primary_key"), "set_primary_key", "is_primary_key");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_increment"), "set_auto_increment", "is_auto_increment");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "not_null"), "set_not_null", "is_not_null");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "unique"), "set_unique", "is_unique");

        ClassDB::bind_static_method("SQLiteColumnSchema", D_METHOD("create", "name", "type", "default_value", "primary_key", "auto_increment", "not_null", "unique"), &SQLiteColumnSchema::create, DEFVAL(Variant::Type::STRING), DEFVAL(Variant()), DEFVAL(false), DEFVAL(false), DEFVAL(false), DEFVAL(false));
    }
public:
    static Ref<SQLiteColumnSchema> create(const String &p_name, Variant::Type p_type, const Variant &p_default_value, bool p_primary_key, bool p_auto_increment, bool p_not_null, bool p_unique) {
        Ref<SQLiteColumnSchema> schema;
        schema.instantiate();
        schema->set_name(p_name);
        schema->set_type(p_type);
        schema->set_default_value(p_default_value);
        schema->set_primary_key(p_primary_key);
        schema->set_auto_increment(p_auto_increment);
        schema->set_not_null(p_not_null);
        schema->set_unique(p_unique);
        return schema;
    }
    String get_name() const { return name; }
    Variant::Type get_type() const { return type; }
    Variant get_default_value() const { return default_value; }
    bool is_primary_key() const { return primary_key; }
    bool is_auto_increment() const { return auto_increment; }
    bool is_not_null() const { return not_null; }
    bool is_unique() const { return unique; }

    void set_name(const String &p_name) { name = p_name; }
    void set_type(Variant::Type p_type) { type = p_type; }
    void set_default_value(const String &p_default_value) { default_value = p_default_value; }
    void set_primary_key(bool p_primary_key) { primary_key = p_primary_key; }
    void set_auto_increment(bool p_auto_increment) { auto_increment = p_auto_increment; }
    void set_not_null(bool p_not_null) { not_null = p_not_null; }
    void set_unique(bool p_unique) { unique = p_unique; }
};

class SQLiteDatabase : public Resource {
    GDCLASS(SQLiteDatabase, Resource);
    Ref<SQLite> db;

protected:
    static void _bind_methods();

public:
    static Ref<SQLiteDatabase> create();
    void set_resource(const String &p_path);
    void create_table(const String &p_table_name, const TypedArray<SQLiteColumnSchema> &p_columns);
    TypedArray<String> get_table_names() const;
    Ref<SQLite> get_sqlite();

    SQLiteDatabase();
    ~SQLiteDatabase();
};
#endif // SQLITE_RESOURCE_H
