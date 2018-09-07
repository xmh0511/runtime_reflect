#include <iostream>
#include "rttr.hpp"
#include <sstream>
using namespace rttr;
struct Test:public Object
{
    int m_a = 20;
    void show(double f,char& c)
    {
       std::stringstream ss;
       ss<<"show()--->"<<f<<"=="<<c;
       c = 'a';
       std::cout<<ss.str()<<std::endl;
    }

    int& get_value()
    {
        return m_a;
    }
};

int main()
{
    Test t;
    t.property("m_a",&Test::m_a);
    t.method("show",&Test::show);
    t.method("get_value",&Test::get_value);
    Object& obj = t;
    std::cout<<obj.get<int>("m_a")<<std::endl;
    auto& value = obj.get<int>("m_a");
    value=value+1;
    std::cout<<t.m_a<<std::endl;
    char c = 'c';
    obj.invoke("show",20.12,c);
    std::cout<<c<<std::endl;

    obj.invoke<int&>("get_value")+=10;
    std::cout<<t.m_a<<std::endl;


    std::shared_ptr<Test> smart_t = std::make_shared<Test>();
    smart_t->property("m_a",&Test::m_a);
    smart_t->method("show",&Test::show);
    std::shared_ptr<Object> ob = smart_t;
    std::cout<<ob->get<int>("m_a")<<std::endl;
    ob->invoke("show",20.12,c);

    ///--------------cast

    std::shared_ptr<Test> t2 = std::dynamic_pointer_cast<Test>(ob);
    std::cout<<t2->m_a<<std::endl;

    return 0;
}