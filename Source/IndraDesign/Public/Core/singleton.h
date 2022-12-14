#pragma once
#include <utility>

namespace detail{
    void print_already_constructed_error();
}

template<class T, typename... Args>
void construct(T& instance, Args... args){
    instance = T {std::forward<Args>(args)...};
}

template<class T>
class Singleton{
private:
    static T instance;
public:
    static T& get(){
        return instance;
    }

    template<typename... Args>
    static void construct_once(Args... args){
        static bool called_once = false;
        if(!called_once){
            called_once = true;
            construct(instance, args...); //should deduce types
        } else {
            detail::print_already_constructed_error();
        }
    }
};

template<class T>
T Singleton<T>::instance;
