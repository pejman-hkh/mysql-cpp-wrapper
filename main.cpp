#include "var.h"
#include "mysql.h"


int main( int argc, char** argv  ) {
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
	var loop1 = db.sql("SELECT id, title FROM test ").pagination(1,5).find();
	print_r( db.loop_pagination );

	//Fetch one record
	var fetch = db.sql("SELECT id, title FROM test ").find_one();
	print_r( fetch );

	//get error
	echo( db.get_error() );

}