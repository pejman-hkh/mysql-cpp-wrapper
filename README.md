# mysql-cpp-wrapper
C++ wrapper for mysql

# Usage

```c++
//Connect to mysql
mysql db;
db.connect("tcp://127.0.0.1:3306", "root", "123456", "test");

//Fetch with accosiated array
var loop = db.sql("SELECT id, title FROM test ").find();
print_r( loop );

//Insert
var insert;
insert["title"] = "peji";
int id = db.table("test").insert( insert );

//Update
db.sql("UPDATE test SET title = ? WHERE id = ? ").exec( { "ppp", 1 } );

//Pagination
var loop1 = db.sql("SELECT id, title FROM test ").pagination(5,1).find();
print_r( db.loop_pagination );

//Fetch one record
var fetch = db.sql("SELECT id, title FROM test ").find_one();
print_r( fetch );

//get error
echo( db.get_error() );


//Fetch one record with assign value
var fetch1 = db.sql("SELECT id, title FROM test WHERE id = ? ").find_one( { 1 } );
print_r( fetch1 );


//Fetch with assign value
var loop2 = db.sql("SELECT id, title FROM test WHERE id = ? ").find( { 1 } );
print_r( loop2 );
```

# C++ vars
See https://github.com/pejman-hkh/cpp-vars for var usage

# Sqlite wrapper
see my sqlite wrapper too :

https://github.com/pejman-hkh/sqlite-cpp-wrapper


# Build
```
g++ -std=c++11 main.cpp -o test -lmysqlcppconn
```

