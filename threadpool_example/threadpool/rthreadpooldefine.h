#include <functional>

#ifndef _THREAD_POOL_DEFINE_H__
#define _THREAD_POOL_DEFINE_H__

#include "rthreadpoolconst.h"

typedef unsigned long long t_uint64;
typedef long long t_int64;


// #define WFMO

#if defined ATLASSERT
# define TPASSERT          ATLASSERT
#elif defined ASSERT
# define TPASSERT          ASSERT
#else
# define TPASSERT			assert 
#endif

//禁止拷贝构造和赋值操作符
#define DISABLE_COPY_AND_ASSIGNMENT(className)  \
private:\
	className(const className& ref);\
	className& operator = (const className& ref)

// _countof定义
#ifndef _countof
#  define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#define API_VERIFY(x)   \
	bRet = (x); \
	TPASSERT(true == bRet);

#define COM_VERIFY(x)   \
	hr = (x); \
	TPASSERT(SUCCEEDED(hr));

#define DX_VERIFY(x)   \
	hr = (x); \
	TPASSERT(SUCCEEDED(hr));

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)  if( NULL != (p) ){ (p)->Release(); (p) = NULL; }
#endif 

#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)	if(NULL != (p) ) { delete p; p = NULL; }
#endif

#define SAFE_DELETE_ARRAY(p) if( NULL != (p) ){ delete [] (p); (p) = NULL; }


#define CHECK_POINTER_RETURN_VALUE_IF_FAIL(p,r)    \
	if(NULL == p)\
	{\
	TPASSERT(NULL != p);\
	return r;\
	}

#define COMPARE_MEM_LESS(f, o) \
	if( f < o.f ) { return true; }\
		else if( f > o.f ) { return false; }

// 带锁的泛型类
template<typename T> 
class CFAutoLock
{
public:
	explicit CFAutoLock<T>(T* pLockObj)
	{
		m_pLockObj = pLockObj;
		m_pLockObj->Lock();
	}
	~CFAutoLock()
	{
		m_pLockObj->UnLock();
	}
private:
	T*   m_pLockObj;
};

//////////////////////////////////////////////////////////////////////////
// 比较大小
template <typename T>
struct UnreferenceLess : public std::binary_function<T, T, bool>
{
	bool operator()(const T& _Left, const T& _Right) const
	{
		return (*_Left < *_Right);
	}
};

typedef enum ThreadWaitType
{
	E_Th_Stop = 0, 
	E_Th_Continue,
	E_Th_TimeOut,
	E_Th_Error,
}EThreadWaitType;

typedef enum GetJobType
{
	E_Type_Stop = 0,
	E_Type_Subtract_Thread,
	E_Type_Get_Job,
	E_Type_Error,					//发生未知错误
}EGetJobType;

#define WFMO

#endif
