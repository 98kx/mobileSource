/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _SHARED_PTR_H
#define _SHARED_PTR_H

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;


template<typename T>
class SharedObj
{
public:
    SharedObj(T *ptr)
        : m_Ptr(ptr), m_RefCount(new unsigned int(0))
    {
        assert(m_Ptr != 0);
        assert(m_RefCount != 0);
        AddRef();
    }

    SharedObj(const SharedObj &rhs)
        : m_Ptr(rhs.m_Ptr), m_RefCount(rhs.m_RefCount)
    {
        assert(m_Ptr != 0);
        assert(m_RefCount != 0);
        AddRef();
    }

    template<typename Y>
    SharedObj(const SharedObj<Y> &rhs)
        : m_Ptr(rhs.m_Ptr), m_RefCount(rhs.m_RefCount)
    {
        assert(m_Ptr != 0);
        assert(m_RefCount != 0);
        AddRef();
    }

#if defined(__BORLANDC__)
    SharedObj& operator=(const SharedObj &rhs)
    {
        assert(rhs.m_Ptr != 0);
        assert(rhs.m_RefCount != 0);

        if (m_Ptr != rhs.m_Ptr)
        {
            Release();
            m_Ptr = rhs.m_Ptr;
            m_RefCount = rhs.m_RefCount;
            AddRef();
        }

        return *this;
    }
#endif

    template<typename Y>
    SharedObj& operator=(const SharedObj<Y> &rhs)
    {
        assert(rhs.m_Ptr != 0);
        assert(rhs.m_RefCount != 0);

        if (m_Ptr != rhs.m_Ptr)
        {
            Release();
            m_Ptr = rhs.m_Ptr;
            m_RefCount = rhs.m_RefCount;
            AddRef();
        }

        return *this;
    }

    ~SharedObj()
    {
        Release();
    }

    operator T*() const
    {
        return m_Ptr;
    }

    void AddRef()
    {
        assert(m_RefCount != 0);
        ++*m_RefCount;
    }

    void Release()
    {
        assert(m_RefCount != 0);
        assert(*m_RefCount > 0);
        assert(m_Ptr != 0);

        if (--*m_RefCount == 0)
        {
            delete m_Ptr;
            m_Ptr = 0;

            delete m_RefCount;
            m_RefCount = 0;
        }
    }

    void Dump()
    {
        cout << "ptr: " << m_Ptr << " refCnt: " << m_RefCount << " " << *m_RefCount << endl;
    }

    unsigned GetRefCount() const
    {
        assert(m_RefCount != 0);
        return *m_RefCount;
    }

private:
    template<typename Y> friend class SharedObj;

    T *m_Ptr;
    unsigned int *m_RefCount;
};

template<typename T>
class SharedPtr
{
public:
    template<typename Y>
    explicit SharedPtr(Y *ptr)
        : m_Obj(ptr)
    {
    }

    SharedPtr(const SharedPtr &rhs)
        : m_Obj(rhs.m_Obj)
    {
    }

    template<typename Y>
    SharedPtr(const SharedPtr<Y> &rhs)
        : m_Obj(rhs.m_Obj)
    {
    }

#if defined(__BORLANDC__)
    SharedPtr& operator=(const SharedPtr &rhs)
    {
        if (*this != rhs)
        {
            m_Obj = rhs.m_Obj;
        }

        return *this;
    }
#endif

    template<typename Y>
    SharedPtr& operator=(const SharedPtr<Y> &rhs)
    {
        if (*this != rhs)
        {
            m_Obj = rhs.m_Obj;
        }

        return *this;
    }

    ~SharedPtr()
    {
    }

    T* operator->() const
    {
        return m_Obj;
    }

    T* Get() const
    {
        return m_Obj;
    }

    unsigned int GetUseCount() const
    {
        return m_Obj.GetRefCount();
    }

    void Dump()
    {
        m_Obj.Dump();
    }

private:
    template<typename Y> friend class SharedPtr;

    SharedObj<T> m_Obj;
};

template<typename T, typename U>
inline bool operator==(const SharedPtr<T> &lhs, const SharedPtr<U> &rhs)
{
    return lhs.Get() == rhs.Get();
}

template<typename T, typename U>
inline bool operator!=(const SharedPtr<T> &lhs, const SharedPtr<U> &rhs)
{
    return lhs.Get() != rhs.Get();
}

/*
class Base
{
};

class Derived : public Base
{
};

void SharedPtrTest()
{
    SharedPtr<int> a(new int);
    assert(a.GetUseCount() == 1);

    SharedPtr<int> b(a);
    assert(a.GetUseCount() == 2);
    assert(b.GetUseCount() == 2);

    SharedPtr<int> c(new int);
    assert(c.GetUseCount() == 1);

    b = c;
    assert(a.GetUseCount() == 1);
    assert(b.GetUseCount() == 2);
    assert(c.GetUseCount() == 2);

    c = a;
    assert(a.GetUseCount() == 2);
    assert(b.GetUseCount() == 1);
    assert(c.GetUseCount() == 2);

    b = a;
    assert(a.GetUseCount() == 3);
    assert(b.GetUseCount() == 3);
    assert(c.GetUseCount() == 3);

    /////////////////////////////

    SharedPtr<Derived> d(new Derived);
    assert(d.GetUseCount() == 1);

    SharedPtr<Base> e(d);
    assert(d.GetUseCount() == 2);
    assert(e.GetUseCount() == 2);

    SharedPtr<Derived> f(new Derived);
    assert(f.GetUseCount() == 1);

    f = d;
    assert(d.GetUseCount() == 3);
    assert(e.GetUseCount() == 3);
    assert(f.GetUseCount() == 3);

    SharedPtr<Base> g(new Base);
    assert(g.GetUseCount() == 1);

    g = f;
    assert(d.GetUseCount() == 4);
    assert(e.GetUseCount() == 4);
    assert(g.GetUseCount() == 4);
    assert(f.GetUseCount() == 4);
}
*/

#endif // _SHARED_PTR_H
