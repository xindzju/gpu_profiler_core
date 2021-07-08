#include <iostream>
#include "gpc_vtable_parser.h"
#include "gpc_utils.h"

class Base {
public:
    virtual void f() { std::cout << "Base::f" << std::endl; }
    virtual void g() { std::cout << "Base::g" << std::endl; }
    void h() { std::cout << "Base::h" << std::endl; }
};

class Derived : public Base {
    //virtual void f() { std::cout << "Derived::f" << std::endl; }
    virtual void h() { std::cout << "Derived::h" << std::endl; };
};

typedef void(*Fun)(void); //Function pointer  

int main() {
    Base b;
    Derived d;
    gpc::VTableParser parser;
    gpc::VTableManager manager;
    manager.GetDXVTables();
    parser.ParseVTable((uint64_t*)&d);
    std::vector<uint64_t> vFuncAddrs = parser.GetAllVFunAddrs();
    /*
    Fun f = (Fun)vFuncAddrs[0];
    f();
    Fun g = (Fun)vFuncAddrs[1];
    g();
    Fun h = (Fun)vFuncAddrs[2];
    h();
    */

    return 0;
}