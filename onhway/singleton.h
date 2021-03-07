#ifndef __ONHWAY_WINGLETON_H
#define __ONHWAY_WINGLETON_H

namespace onhway{

template<class T, class x=void, int N=0>
class Singleton{
public:
    static T* GetInstance(){
        static T v;
        return &v;
    }

};

template<class T, class x=void, int N=0>
class SingletonPtr{
public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> v(new T);
            return v;
        
    }
};



}

#endif

