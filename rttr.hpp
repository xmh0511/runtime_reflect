//
// Created by xmh on 18-9-6.
//

#ifndef UNTITLED78_RTTR_HPP
#define UNTITLED78_RTTR_HPP
#include <iostream>
#include <string>
#include <tuple>
#include <memory>
#include <functional>
#include <any>
#include <map>
#include <utility>
namespace  rttr
{
    class Object;
    class base_data_interface
    {
    public:
        virtual std::any get(std::any instance) = 0;
    };
    template<typename Type>
    class element_data_class;

    template<typename A,typename Class>
    class element_data_class<A(Class::*)>:public base_data_interface
    {
    public:
        using element_type = A(Class::*);
        using ClassType = Class;
    public:
        element_data_class(A(Class::*t)):element_(t)
        {

        }
    public:
        std::any get(std::any instance)
        {
            Object* ptr = std::any_cast<Object*>(instance);
            return std::any(dynamic_cast<ClassType*>(ptr)->*element_);
        }
    private:
        element_type element_;
    };

    class base_method_interface
    {
    public:
        virtual std::any invoke(std::any instance,std::any ArgsTuple) = 0;
    };

    template<typename T>
    class method_invok;

    template<typename Ret,typename Class,typename...Args>
    class method_invok<Ret(Class::*)(Args...)>:public base_method_interface
    {
    public:
        using Function =  Ret(Class::*)(Args...);
        using ClassType = Class;
    public:
        method_invok(Function f):func_(f)
        {

        }
    public:
        std::any invoke(std::any instance,std::any ArgsTuple)
        {
            auto tup = std::any_cast<std::tuple<Args...>>(ArgsTuple);
            return invoke_help(std::make_index_sequence<sizeof...(Args)>{},tup,instance);
        }

        template<std::size_t...Indexs,typename Tuple>
        std::any invoke_help(std::index_sequence<Indexs...>,Tuple tp,std::any instance)
        {
            return call(instance,std::get<Indexs>(tp)...);
        }

        std::any call(std::any instance,Args...args)
        {
            Object* obj =  std::any_cast<Object*>(instance);
            return call_help<Ret>(dynamic_cast<ClassType*>(obj),args...);
        }

        template<typename RType>
        std::enable_if_t<std::is_void_v<RType>,std::any>  call_help(ClassType* obj,Args...args)
        {
            (obj->*func_)(args...);
            return std::any(0);
        }

        template<typename RType>
        std::enable_if_t<!std::is_void_v<RType>,std::any>  call_help(ClassType* obj,Args...args)
        {
            return (obj->*func_)(args...);
        }

    private:
        Function func_;
    };


class Object
    {
    public:
        Object()
        {

        }
        virtual ~Object() = default;
    public:
        template<typename Type>
        void property(const std::string& name,Type meta_ptr)
        {
            auto ptr = std::make_shared<element_data_class<Type>>(meta_ptr);
            meta_property_map_.emplace(name,ptr);
        }
        template<typename Function>
        void method(const std::string& name,Function f)
        {
            auto ptr = std::make_shared<method_invok<Function>>(f);
            meta_method_map_.emplace(name,ptr);
        }

        template<typename Convert>
        Convert get(const std::string& name)
        {
            auto iter = meta_property_map_.find(name);
            if(iter!=meta_property_map_.end()){
                return std::any_cast<Convert>(iter->second->get(this));
            }
            return {};
        }

        template<typename Convert,typename...Args>
        Convert invoke(const std::string& name,Args...args)
        {
            auto iter = meta_method_map_.find(name);
            if(iter!=meta_method_map_.end()){
                auto tup = std::make_tuple(args...);
                return std::any_cast<Convert>(iter->second->invoke(this,tup));
            }
            return {};
        };

        template<typename...Args>
        void invoke(const std::string& name,Args...args)
        {
            auto iter = meta_method_map_.find(name);
            if(iter!=meta_method_map_.end()){
                auto tup = std::make_tuple(args...);
                iter->second->invoke(this,tup);
            }
        };

    private:
        std::map<std::string,std::shared_ptr<base_data_interface>> meta_property_map_;
        std::map<std::string,std::shared_ptr<base_method_interface>> meta_method_map_;
    };
}
#endif //UNTITLED78_RTTR_HPP
