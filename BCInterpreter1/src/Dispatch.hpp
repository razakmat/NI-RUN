#ifndef __DISPATCH_H__
#define __DISPATCH_H__

#include "RuntimeObject.hpp"
#include "FMLVM.hpp"
#include <vector>

using namespace std;

class Dispatch
{
    public:
        Dispatch(vector<uint32_t> & args, FMLVM * vm,const string & name,uint32_t point_rec);
        void operator ()(ROBoolean & boolean);
        void operator ()(ROInteger & integer);
        void operator ()(RONull & nul);
        void operator ()(ROArray & arr);

        
        void operator ()(ROObject & obj);

    private:
        void RetBool(bool value);
        void RetInt(int32_t value);
        vector<uint32_t> & m_args;
        FMLVM * m_vm;
        const string & m_name;
        uint32_t m_point_rec;
};





#endif // __DISPATCH_H__