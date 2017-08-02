#pragma once
#include "var.h"

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class mysql  {

private:

   sql::Connection *_con;
   sql::Driver * _driver;
   sql::PreparedStatement * _pstmt;
   sql::ResultSet * _res;
   sql::ResultSetMetaData * _res_meta;
   sql::Statement * _stmt;

   std::string _error;
   std::string _sql;
   std::string _table_name;
   int _wilcard = 0;
   var _paginate;
   


   bool paginate( var data ) {
      int number = _paginate["n"].to_int();
      int page = _paginate["p"].to_int();
      _paginate.unset();

      if( ! number ) return false;

      var exp = var(_sql).explode("FROM");

      std::string temp_sql = _sql;


       var fetch = sql( "SELECT COUNT(*) as count FROM " + exp[exp.count()-1].string() ).find_one( data );

      int count = fetch["count"].to_int();

      int limit = 8;

      int n_p = std::ceil( (float)count / (float)number );

      int start = ( page - limit ) <= 0  ? 1 : page - limit ; 
      int end = ( page + limit >= n_p ) ? n_p : page + limit;


      int next = page >= std::ceil( (float)count / (float)number ) ? page : page + 1;
      int prev = page <= 1 ? 1 : page - 1;


      var pagin;
      for( int i = start; i <= end ; i++ )
      {
         pagin[ i ] = i;
      }
      
      loop_pagination["loop"] = pagin; 
      loop_pagination["count"] = count;
      loop_pagination["next"] = next;
      loop_pagination["prev"] = prev;

      _wilcard = 0;
      _sql = temp_sql+" LIMIT "+std::to_string( page * number - number )+", "+std::to_string(number);

      return true;
   }

   bool exec_a() {

      try {
         _res = _pstmt->executeQuery();
      } catch (sql::SQLException e) {

         _error = e.what();
         _error += "\r\nexecuteQuery problem !\r\n";         
      }
  
      _wilcard = 0;
      return true;     
   }

   var get_column_name() {

      _res_meta = _pstmt->getMetaData();
      int cols = _res_meta->getColumnCount();
      var out;

      for ( int i = 0; i < cols; i++ ) {
 
         out[ (var)_res_meta->getColumnLabel( i + 1 ) ] = (var)_res_meta->getColumnTypeName( i + 1 );

      }

      return out;     
   }

   bool bind( var &data ) {
   
      if( prepare() ) {

         for( auto x : data  ) {
            var val = data[x];

            if( val.type() == "int" ) {

               if( ! bind( val.to_int() ) ) {
                  return false;
               }

            } else if( val.type() == "double" ) {

               if( ! bind( to_number<double>( val.string() ) ) ) {
                  return false;
               }

            } else {
               if( ! bind( val.string() ) ) {
                  return false;
               }               
            }

         }

         return true;
      }

      return false;
   }

   bool prepare() {

      try {
         _pstmt = _con->prepareStatement( _sql );

      } catch (sql::SQLException e) {
         _error = e.what();
         _error += "\r\nprepareStatement problem !\r\n";

         return false; 
      }
               
      return true;
   }

   bool bind( const double &val ) {
      ++_wilcard;
      _pstmt->setDouble( _wilcard, val );

      return true;
   }


   bool bind( const int &val ) {

      ++_wilcard;
      _pstmt->setInt( _wilcard, val );

      return true;
   }

   bool bind( const std::string &val ) {
      ++_wilcard;
      _pstmt->setString( _wilcard, val );

      return true;
   }

public:

   var loop_pagination;

   std::string type() {
      return "mysql";
   }

   mysql() : _driver(0), _pstmt(0), _res(0), _res_meta(0), _stmt(0) {

      _table_name = "";
   }

   ~mysql() {

      delete _pstmt;
      delete _res;
      delete _con;
   }

   std::string get_error() {
      return _error;
   }

   bool connect( const std::string &host, const std::string &user, const std::string &pass, const std::string &db ) {

      try {
         _driver = get_driver_instance();
      } catch (sql::SQLException e) {
         _error = e.what();
         _error += "\r\nCould not get a database driver\r\n";

         return false;
      }

      try {
         _con = _driver->connect( host, user, pass );
      } catch (sql::SQLException e) {
         _error = e.what();
         _error += "\r\nCould not connect to mysql\r\n";

         return false;         
      }

      try {
         _con->setSchema( db );
      } catch (sql::SQLException e) {
         _error = e.what();
         _error += "\r\nCould not select a database\r\n";
         return false;
      }

      return true;
   }

   //for sqlite switch to mysql
   bool select( const std::string &a ) {

   }

   bool statement( const std::string &sql ) {
      _stmt = _con->createStatement();
      try {
         _stmt->execute( sql );
      } catch (sql::SQLException e) {
         _error = e.what();
         _error += "\r\nCould execute statement\r\n";
         return false;
      }

      return true;
   }

   mysql &pagination( const int &number, const int &page ) {
      _paginate["n"] = number;
      _paginate["p"] = page;

      return *this;
   }

   mysql &table( const std::string &table_name ) {
      _wilcard = 0;
      _table_name = table_name;
      return *this;
   }

   mysql &sql( const std::string &sql ) {
      _wilcard = 0;
      _sql = sql;
      loop_pagination = "";

      return *this;
   }

   var find( var data = "" ) {
      _wilcard = 0;

      if( _paginate.count() > 0 ) {
         paginate( data );
      }

      if( bind(  data ) ) {
         if( exec_a() ) {
            var columns = get_column_name();
      
            var ret;
            int j = 0;
            while(  _res->next() ) {

            
               var data;
               for( auto key : columns ) {
                  var &value = columns[ key ];

                  if( value == "TEXT" || value == "VARCHAR" ) {
                     data[ key ] = (var)_res->getString( key.string() );
                  } else if( value == "INT" ) {
                     data[ key ] = (var)_res->getInt( key.string() );
                  } else if( value.string().find("CHAR") != std::string::npos ) {
                     data[ key ] = (var)_res->getString( key.string() );
                  } else {
                     data[ key ] = (var)_res->getInt( key.string() );
                  }
               }
               ret[ j ] = data;
               data.unset();
               j++;
            }

            return ret;            
         }

      }

      return {};
   }

   var find_one( var data = "" ) {
      _sql += " LIMIT 0,1";
      return find( data )[0];
   }

   int insert( var data ) {
      _wilcard = 0;
      var colum_name;
      var values;
      for( int i = 0; i < data.count(); i++ ) {
         colum_name[i] = (var)"`"+data.key( i )+(var)"`";
         values[i] = "?";
      }

      std::string column_str = colum_name.implode(",").string();
      std::string values_str = values.implode(",").string();

      _sql = "INSERT INTO `"+_table_name+"`("+column_str+") VALUES("+values_str+")";
      _table_name = "";
      
      if ( bind( data ) ) {
         if( exec_a() ) {
            return sql("SELECT LAST_INSERT_ID() AS id").find_one()["id"].to_int();
         }

      } else {
         return 0;
      }

   }

   bool exec( var data = "" ) {
      _wilcard = 0;

      if ( bind( data ) ) {
         if( exec_a() ) {

            return true;
         }
      }

      return false;
   }

};

