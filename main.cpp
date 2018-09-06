#include <iostream>
#include "rttr.hpp"
#include <sstream>
using namespace rttr;
struct Test:public Object
{
    int m_a = 20;
    void show(double f,char c)
    {
       std::stringstream ss;
       ss<<"show()--->"<<f<<"=="<<c;
       std::cout<<ss.str()<<std::endl;
    }
};
int main()
{
    Test t;
    t.property("m_a",&Test::m_a);
    t.method("show",&Test::show);
    Object& obj = t;
    std::cout<<obj.get<int>("m_a")<<std::endl;
    obj.invoke("show",20.12,'c');
    return 0;
}