// Use a non-ignorable value with status_value.

#include "nonstd/status_value.hpp"
#include <iostream>

using namespace nonstd;

template< typename T >
class required
{
public:
    required( T const & value)
    : content( value ) {}

    required( required && other )
    : content( other.content )
    , ignored( other.ignored )
    {
        other.ignored = false;
    }

    required( required const & other ) = delete;

    ~required() noexcept( false )
    {
        if ( ignored )
            throw std::runtime_error("required: content unobserved");
    };

    T const & operator *() const { ignored = false; return content; }

private:
    T content;
    mutable bool ignored = true;
};

template< typename T >
auto make_required( T value ) -> required<T>
{
    return required<T>( value );
}

auto produce( int value ) -> status_value< int, required<int> >
{
    return { 0, make_required( value ) };
}

int main( int argc, char * argv[] )
{
    try
    {
        auto svr42 = produce( 42 );
        auto svr13 = produce( 13 );

        std::cout << "value: " << **svr42 << "\n";
    }
    catch ( std::exception const & e )
    {
        std::cout << "Error: " << e.what();
    }
}

// cl -EHsc -wd4814 -Zc:implicitNoexcept- -I../include/nonstd 02-required.cpp && 02-required.exe
// g++ -std=c++14 -Wall -I../include/nonstd -o 02-required.exe 02-required.cpp && 02-required.exe
// value: 42
// Error: required: content unobserved
