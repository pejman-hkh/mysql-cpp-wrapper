#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <initializer_list>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <functional>
#include <memory>
#include "dtoa_milo.h"
#include "itoa_milo.h"

namespace vars {

double microtime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    double microsec = (double)time.tv_usec / (double)1000000 ;//time.tv_sec ;

    return microsec;
}


enum VAR_TYPE {
    VAR_STRING, VAR_INT, VAR_ARRAY, VAR_FUNCTION, VAR_CLASS, VAR_BOOL, VAR_DOUBLE
};



template <typename T>
std::string to_string(T const& value) {
    std::stringstream sstr;
    sstr << value;
    return sstr.str();
}


std::string int_to_string( const int &a ) {
	char ret[11];
	i32toa_sse2( a, ret );
	return ret;  
}

std::string double_to_string( const double &a ) {
	char ret[11];
	dtoa_milo( a, ret );
	return ret;
}


template <typename T>
T to_number( const std::string& str ) {
    std::istringstream ss(str);
    T result;
    ss >> result;
    return result;
}
using namespace std::placeholders;

template<class T>
class refrence {
public:
    refrence() {}  
    int set( const T &t ) {
        vect.push_back( t );
        return vect.size() - 1;
    }

    T &get( int index ) {
        return vect[index];
    }
private:
    std::vector< T > vect; 
};


class var {
public:
    typedef var (*func)(var&);
    typedef var (*func1)(var);
    func function;
    func1 function1;
    std::function <var(var&)> methods;
    std::function <var(var)> methods1;

	friend bool empty( var a );

	const char * c_str() {
		return _string.c_str();
	}

    std::string string() const {
    	if( _type == VAR_DOUBLE ){
    		return double_to_string(_int);
    	} else if( _type == VAR_INT ) {

    		return int_to_string( _int );    		
    	}
		else
    		return _string;  
    }

    var(): _type(VAR_STRING), _string("") {}

	var( const std::function <var(var&)> &a ) : _type( VAR_CLASS ), _string("Function"), methods( a ) {}

	var( const std::function <var(var)> &a ) :_type( VAR_CLASS ), _string("Function"), methods1(a) {}

	var( const func &a ) : _type(VAR_FUNCTION), function(a), _string("Function") {}

	var( const func1 &a ) : _type(VAR_FUNCTION), function1(a), _string("Function") {}

	var( const int &a ): _type(VAR_INT), _int( a ) {}

	var( const long int &a ): _type(VAR_INT), _int( a ) {}

	var( const unsigned int &a ): _type(VAR_INT), _int( a ) {}

	var( const long unsigned int &a ): _type(VAR_INT), _int( a ) {}

	var( const float &a ): _type(VAR_DOUBLE), _int( a ) {}

	var( const double &a ): _type(VAR_DOUBLE), _int( a ) {}

	var( const long double &a ): _type(VAR_DOUBLE), _int( a ) {}
	

	var( const std::string &a ): _type(VAR_STRING), _string( a ) {}

	var( const char * const &a ) : _type(VAR_STRING), _string( to_string(a) ) {}

	var( const unsigned char* const &a ) : _type(VAR_STRING), _string( to_string(a) ) {}

	var( char *a ) : _type(VAR_STRING), _string( a )  {}

    var( const bool &a ) : _type(VAR_INT), _int( a ? 1 : 0 ) {}

	explicit operator bool() 
	{

		switch( _type ) {
			case VAR_STRING : {
				if( ( _string != "0" && _string != "" ) )
					return true;
			} break;


			case VAR_ARRAY : {
				if( this->count() > 0 )
					return true;
			}

			default : {
				if( _int != 0 )
					return true;
			}  break;

		}


	    return false;
	}

    var key( var i ) {
        return keys[atol(i.string().c_str())];
    }
            
    var value( var i ) {
        return data[atol(i.string().c_str())];
    }

	void unset( var index = "" ) {
		if( index == "") {
			keys.clear();
			data.clear();
		} else {
			for( int i = 0; i < keys.size(); i++ ) {
				if( keys[i].string() == index.string() ) {
					keys.erase( keys.begin() + i );
					data.erase( data.begin() + i );
				}
			}			
		}
	}

	func get() const {
		return function;
	}

	func1 get_s() const {
		return function1;
	}

	std::function <var(var&)> get_m() const {
		return methods;
	}

	std::function <var(var)> get_ms() const {
		return methods1;
	}


    var explode( const std::string &delim ) {
	    var out;
		size_t pos = 0;
		size_t start = 0;
		int i = 0;
		while ( ( pos = string().find( delim, pos ) ) != std::string::npos ) {

			out[ i++ ] = string().substr( start, ( pos - start ) );

			pos += delim.length();

			start = pos;

		}	
	
		out[ i ] = string().substr( start, string().length() );
		return std::move( out );

    }

    var implode( const std::string &pre ) {
    	std::string p;
    	std::string pre1 = "";
    	for( int i = 0;i < data.size(); ++i) {
    		p += pre1 + data[i].string();
    		pre1 = pre;
    	}

    	return p;
    }

    var replace( var a )
	{

		std::string out = _string;
		for( auto x : a ) {
			
			size_t pos = 0;
    		while ( ( pos = out.find( x.string(), pos ) ) != std::string::npos ) {
				out.replace( pos, x.string().length(), a[x].string() );
				pos += a[x].string().length();
			}	
		}
		_string = out;
		return out;
	}

    
	bool isset( var index ) const {
		for( int i = 0; i < keys.size(); i++ ) {

     		if( index._type == keys[i]._type ) {
        		if( index._type == VAR_STRING ) {
        			if( index._string == keys[i]._string )
        				return true;
        		} else {
        			if( index._int == keys[i]._int )
        				return true;
        		}
        	}
        	else {
        		if(  index.string() == keys[i].string() )
                	return true;
            }

		}

		return false;			
	}

    

	var type() {
		return _type;
	}
	
    var* begin() {
        return &keys[0];
    }
    
    var* end() {
    	return &keys[ keys.size() ];
    }

    var in_array( var arr ) {
   		for( auto x : arr ) {
   			if( string() == arr[x].string() ) {
   				return true;
   			}

   		}
    	return false;
    }

    int count() {
        if( _type == VAR_ARRAY ) {
            return keys.size();
        } else {
            return 0;
        }
    }

    int length() {
        return string().size();
    }

	double to_num() {
		return _int;
	}

	int to_int() {
		if( _type == VAR_STRING )
			return to_number<int>( _string );
		else
			return (int)_int;
	}

    int size() {
       return count();
    }


	var to_kv() {
		var out;
		for( int i = 0; i < keys.size(); ++i ) {
			var value = data[i];

			if( value[0]._type == VAR_ARRAY ) {
				for( auto x : value ) {
					out[ i ][ value[x][0] ] = value[x][1];
				}
			} else {
				var val = data[i];
				if( val._type == VAR_ARRAY ) {
					out[ val[0] ] = val[1];
				} else {
					out[i] = val;
				}	
			}
		}
		
		return out;
	}

	var( std::initializer_list<var> a ) {
		_type = VAR_ARRAY;

		int i = 0;
		for (auto n : a ) {
			keys.push_back( i );
			data.push_back( n );
			i++;
		}

	}

	var concat( var a ) {

        _string =  string() + a.string();

		return _string;
	}


	var operator+( var a ) {
   		var out;
   		if( _type == VAR_STRING || a._type == VAR_STRING ) {
        	out = string() + a.string();
        } else {
       		out = _int + a._int;
       	}

		return out;
	}	


	var operator-( var a ) {
		return _int - a._int;
	}

	var operator+=( var a ) {
        var out;

        if( _type == VAR_STRING || a._type == VAR_STRING ) {

        	_string = string() + a.string();

        	out = _string;
        } else {
        	_int = _int + a._int;
        	out = _int;
        }

		return out;

	}

	var operator-=( var a ) {

		_int = _int - a._int;

		return _int;

	}

	var operator*=( var a ) {

		_int = _int * a._int;

		return _int;

	}

	var operator/=( var a ) {
	
		_int = _int / a._int;

		return _int;
	}

	var operator%=( var a ) {
		_int = (int)_int % (int)a._int;

		return _int;
	}

    int operator++() {
        _int = _int + 1;
    
        return _int;
    }
        
    var operator*(var a) {
		return _int * a._int;
    }

	var operator/(var a) {
		return _int / a._int;
	}
		     
	bool operator&&( var a ) {
		
		
		return _int && a._int;
	}	

	bool operator||( var a ) {
		return _int || a._int;
	}	


	bool operator<( var a ) {
	
		return _int < a._int;
	}
				     
	bool operator>( var a ) {
		return _int > a._int;
	}
		     
	bool operator<=( var a ) {
		return _int <= a._int;
	}

	bool operator>=( var a ) {
		return _int >= a._int;
	}	
		     
	bool operator==( const char * const &a ) {
		return a == string();
	}

	bool operator==( const std::string &a ) {
		
		return a == string();
	}
		     
	bool operator==( const int &a ) {
		if( _type != VAR_STRING ){
			return a == _int;
		}
		
		return false;
	}

	bool operator==( var a ) {
		if( _type == a._type ){
			if( _type == VAR_STRING ){
				return _string == a._string;
			} else {
				return _int == a._int ;
			}
		}
		
		return false;
	}	

	bool operator!=( var a ) {
		return _int != a._int;
	}

	var index( var a ) const {
        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {
        	if( _type == keys[i]._type ) {
        		if( _type == VAR_STRING ) {
        			if( _string == keys[i]._string )
        				return data[i];
        		} else {
        			if( _int == keys[i]._int )
        				return data[i];
        		}
        	}
        	else {
        		if(  a.string() == keys[i].string() )
                	return data[i];
            }
        }

        return -1;
	}

	var &operator[]( const std::string &a ) {

		_type = VAR_ARRAY;

        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {
        	if(  a == keys[i].string() )
                return data[i];
        }

        var temp;
	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}

	var &operator[]( const char * const &a ) {

		_type = VAR_ARRAY;

        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {
        	if(  a == keys[i].string() )
                return data[i];
        }

        var temp;
	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}

	var &operator[]( const int &a ) {

		_type = VAR_ARRAY;

	
        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {

        	switch( keys[i]._type ) {
	            case VAR_STRING :
	            	if(  int_to_string( a ) == keys[i].string() )
                	return data[i];
                break;
                default:
	                if(  a == keys[i]._int )
		                return data[i];
        	}

        
        }

        var temp;

	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}


	var &operator[]( const unsigned int &a ) {

		_type = VAR_ARRAY;

	
        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {

        	switch( keys[i]._type ) {
	            case VAR_STRING :
	            	if(  int_to_string( a ) == keys[i].string() )
                	return data[i];
                break;
                default:
	                if(  a == keys[i]._int )
		                return data[i];
        	}

        
        }

        var temp;

	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}

	var &operator[]( const double &a ) {

		_type = VAR_ARRAY;

        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {
        	switch( keys[i]._type ) {
	            case VAR_STRING :
	            	if(  double_to_string( a ) == keys[i].string() )
                	return data[i];
                break;
                default:
	                if(  a == keys[i]._int )
		                return data[i];
        	}
        }

        var temp;
	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}

	var &operator[]( var a ) {

		_type = VAR_ARRAY;

        int i = 0;
        for(i = 0;i < keys.size(); ++i)
        {
        	if( _type == keys[i]._type ) {
        		if( _type == VAR_STRING ) {
        			if( _string == keys[i]._string )
        				return data[i];
        		} else {
        			if( _int == keys[i]._int )
        				return data[i];
        		}
        	}
        	else {
        		if(  a.string() == keys[i].string() )
                	return data[i];
            }
        }

        var temp;
	    keys.push_back( a );
        data.push_back( temp );

		return data[i];		
	}

	~var() {

	}


private:
	std::string _string;
	int _type;
	double _int;

    std::vector<var> keys;
    std::vector<var> data;
};


void echo( var i ) {
    if( i.type() == VAR_ARRAY ) {
    	printf("Array\n"); 
    } else {
    	std::string out = i.string();
    	fwrite(out.c_str(), 1, out.length(), stdout);

    }
}

bool empty( var a ) {
    if( a.string() == "" || a.string() == "0" ) {
        return true;
    }
    
    return false;
}

void print_r( var a, std::string &ret_str, const std::string &tab = "" ) {
    if( a.type() == VAR_ARRAY ) {
        int i = 0;
     
        ret_str += "Array (\n";
    
        for( auto &x : a ) {
			
			ret_str += tab + "    [" + x.string() + "] => ";
		
            if( a[x].type() == VAR_ARRAY ) {
                print_r( a[x], ret_str, tab + "    " );
            } else {
                ret_str += a[x].string() +"\n";
            }
        }

        ret_str += tab + ")\n";
    } else if ( a.type() == VAR_FUNCTION ) {
    	ret_str += "function";
    } else {
        ret_str += a.string();
    }
}

var print_r( const var &a, bool ret = false ) {
	std::string out;
	print_r( a, out );
	if( ret )
		return out;
	else
		fwrite(out.c_str(), 1, out.length(), stdout);

	return 0;
}

}