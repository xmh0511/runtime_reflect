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
    class element_data_class<A(Class::*)> final:public base_data_interface
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
            ClassType* desc_ptr = dynamic_cast<ClassType*>(ptr);
            return std::any(&(desc_ptr->*element_));
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
    struct is_has_reference:std::false_type
    {

    };

    template<typename T>
    struct is_has_reference<T&>:std::true_type
    {

    };

    template<typename T>
    struct is_has_reference<T&&>:std::true_type
    {

    };

    template<typename T>
    class method_invok;

    template<typename Ret,typename Class,typename...Args>
    class method_invok<Ret(Class::*)(Args...)> final:public base_method_interface
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
            try{
                auto tup = std::any_cast<std::tuple<Args...>>(ArgsTuple);
                return invoke_help(std::make_index_sequence<sizeof...(Args)>{},tup,instance);
            }catch (std::bad_any_cast& error){
                auto er = std::string("the invoke args's type is not as the same as method's");
                std::cout<<er<<std::endl;
                throw er;
            }
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
            if(is_has_reference<Ret>::value){
                return &(obj->*func_)(args...);
            }
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
        Convert& get(const std::string& name)
        {
            auto iter = meta_property_map_.find(name);
            if(iter!=meta_property_map_.end()){
                Convert* data_ptr = std::any_cast<Convert*>(iter->second->get(this));
                return *data_ptr;
            }
            return (Convert&)(*this);  //bad data
        }

        template<typename Convert,typename...Args>
        std::enable_if_t<!std::is_void_v<Convert>,Convert> invoke(const std::string& name,Args&&...args)
        {
            auto iter = meta_method_map_.find(name);
            if(iter!=meta_method_map_.end()){
                auto tup = std::tuple<Args...>(std::forward<Args>(args)...);
                if(is_has_reference<Convert>::value){
                    using pointType = typename std::remove_reference<Convert>::type* ;
                    auto ptr = std::any_cast<pointType>(iter->second->invoke(this,tup));
                    return *ptr;
                }
                return std::any_cast<Convert>(iter->second->invoke(this,tup));
            }
            return (Convert&)(*this);
        };

        template<typename Convert,typename...Args>
        std::enable_if_t<std::is_void_v<Convert>> invoke(const std::string& name,Args&&...args)
        {
            invoke(name,std::forward<Args>(args)...);
        };

        template<typename...Args>
        void invoke(const std::string& name,Args&&...args)
        {
            auto iter = meta_method_map_.find(name);
            if(iter!=meta_method_map_.end()){
                auto tup = std::tuple<Args...>(std::forward<Args>(args)...);
                iter->second->invoke(this,tup);
            }
        };

    private:
        std::map<std::string,std::shared_ptr<base_data_interface>> meta_property_map_;
        std::map<std::string,std::shared_ptr<base_method_interface>> meta_method_map_;
    };
}
#endif //UNTITLED78_RTTR_HPP
