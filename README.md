# mysql-cpp-wrapper
C++ wrapper for mysql

# Usage

```c++
//Connect to mysql
mysql db;
db.connect("tcp://127.0.0.1:3306", "username", "password", "database_name");

//Fetch with accosiated array
var loop = db.sql("SELECT id, title FROM admin_group ").find();
print_r( loop );

//Insert
var insert;
insert["title"] = "peji";
int id = db.table("test").insert( insert );

//Update
db.sql("UPDATE test SET title = ? WHERE id = ? ").exec( { "ppp", 1 } );

//Pagination
var loop = db.sql("SELECT id, title FROM test ").pagination(1,5).find();
print_r( db.loop_pagination );

//Fetch one record
var fetch = db.sql("SELECT id, title FROM test ").find_one();
print_r( fetch );


//get error
echo( db.get_error() );

```

# Build
```
g++ -std=c++11 test.cpp -o test -lmysqlcppconn
```
