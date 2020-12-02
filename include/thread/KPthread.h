#ifndef _PTHREAD_HPP_
#define _PTHREAD_HPP_
#include <iostream>
#include <pthread.h>
#include <string>
#include <stdint.h>
#include "KMutex.h"
#include "KLockGuard.h"
#include "KException.h"
#include "KError.h"

namespace klib {
    class KPthread
    {
#define _Template1 template<typename ObjType, typename RetType, typename ArgType>
#define _Template2 template<typename RetType, typename ArgType>
#define _DeclareType1 \
    typedef RetType(ObjType::*RunFunc)(ArgType);\
    typedef void(ObjType::*LogFunc)(const std::string &);\
    struct PthreadData\
    {\
        ObjType *obj;\
        RunFunc rf;\
        LogFunc lf;\
        ArgType arg;\
        RetType *ret;\
        KPthread *self;\
    };

#define _DeclareType2 \
    typedef RetType(*RunFunc)(ArgType);\
    typedef void(*LogFunc)(const std::string &);\
    struct PthreadData\
    {\
        RunFunc rf;\
        LogFunc lf;\
        ArgType arg;\
        RetType *ret;\
        KPthread *self;\
    };
    public:
        enum ThreadError { AlreadRunning, InvalidArgument, CreateFailed, Success };
        KPthread(const std::string &name)
			:m_running(false),m_name(name)
		{
            memset(&m_tid, 0, sizeof(m_tid));
			int rc = pthread_attr_init(&m_attr);
			if (rc != 0)
			{
				throw KException(__FILE__, __LINE__, KError::StdErrorStr(rc).c_str());
			}
		}

        virtual ~KPthread()
        {
			int rc = pthread_attr_destroy(&m_attr);
			if (rc != 0)
			{
				throw KException(__FILE__, __LINE__, KError::StdErrorStr(rc).c_str());
			}
        }

        _Template1
         int Run(ObjType* obj, RetType(ObjType::* rf)(ArgType), ArgType arg, void(ObjType::* lf)(const std::string&) = NULL, RetType* ret = NULL)
        {
            KLockGuard<KMutex> lock(m_tmtx);
            if (m_running)
            {
                return AlreadRunning;
            }

            if (!obj || !rf)
            {
                return InvalidArgument;
            }
            _DeclareType1
            PthreadData* dat = new PthreadData;
            dat->obj = obj;
			dat->rf = rf;
			dat->lf = lf;
            dat->arg = arg;
            dat->ret = ret;
            dat->self = this;

            int rc = pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM);
            if (rc != 0)
            {
                throw KException(__FILE__, __LINE__, KError::StdErrorStr(rc).c_str());
            }
            //detach thread
            //pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
            rc = pthread_create(&m_tid, &m_attr, Work<ObjType, RetType, ArgType>, dat);
            if (rc != 0)
            {
                delete dat;
                return CreateFailed;
            }
            return Success;
        }

        _Template2
        int Run(RetType(*rf)(ArgType), ArgType arg, void(*lf)(const std::string&) = NULL, RetType* ret = NULL)
        {
            KLockGuard<KMutex> lock(m_tmtx);
            if (m_running)
                return AlreadRunning;

            if (!rf)
                return InvalidArgument;

            _DeclareType2
            PthreadData* dat = new PthreadData;
            dat->rf = rf;
            dat->lf = lf;
            dat->arg = arg;
            dat->ret = ret;
            dat->self = this;

            int rc = pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM);
            if (rc != 0)
                throw KException(__FILE__, __LINE__, KError::StdErrorStr(rc).c_str());
            //detach thread
            //pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
            rc = pthread_create(&m_tid, &m_attr, Work<RetType, ArgType>, dat);
			if (rc != 0)
			{
				delete dat;
				return CreateFailed;
			}
			return Success;
        }

        void Join()
        {
			int ds;
			int rc = pthread_attr_getdetachstate(&m_attr, &ds);
			if (rc == 0 && ds == PTHREAD_CREATE_JOINABLE)
			{
				pthread_join(m_tid, NULL);
			}
        }

        inline bool IsRunning() const
		{
			KLockGuard<KMutex> lock(m_tmtx);
			return m_running;;
		}

        static uint32_t GetThreadId()
        {
#ifdef WIN32
            return pthread_getw32threadid_np(pthread_self());
#else
            return pthread_self();
#endif
        }

		static uint32_t GetThreadId(pthread_t pid)
		{
#ifdef WIN32
			return pthread_getw32threadid_np(pid);
#else
			return pid;
#endif
		}

    private:
        _Template1
        static void* Work(void* arg)
        {
            _DeclareType1
            PthreadData* dat = reinterpret_cast<PthreadData*>(arg);
            {
				KLockGuard<KMutex> lock(dat->self->m_tmtx);
                dat->self->m_running = true;
            }
            if (dat->lf)
                (dat->obj->*dat->lf)(dat->self->m_name + " Running");

            if (dat->ret)
                *dat->ret = (dat->obj->*dat->rf)(dat->arg);
            else
                (dat->obj->*dat->rf)(dat->arg);
			{
				KLockGuard<KMutex> lock(dat->self->m_tmtx);
                dat->self->m_running = false;
			}
            
			if (dat->lf)
				(dat->obj->*dat->lf)(dat->self->m_name + " Stopped");

            delete dat;
            return 0;
        }

        _Template2
        static void* Work(void* arg)
        {
            _DeclareType2
            PthreadData* dat = reinterpret_cast<PthreadData*>(arg);
			{
				KLockGuard<KMutex> lock(dat->self->m_tmtx);
				dat->self->m_running = true;
			}
			if (dat->lf)
				(*dat->lf)(dat->self->m_name + " Running");

            if(dat->ret)
                *dat->ret = (*dat->rf)(dat->arg);
            else
                (*dat->rf)(dat->arg);
			{
				KLockGuard<KMutex> lock(dat->self->m_tmtx);
				dat->self->m_running = false;
			}
            
			if (dat->lf)
				(*dat->lf)(dat->self->m_name + " Stopped");

            delete dat;
            return 0;
        }

    private:
        pthread_attr_t m_attr;
        pthread_t m_tid;
        std::string m_name;
        KMutex m_tmtx;
        volatile bool m_running;
    };
};
#endif // !_PTHREAD_HPP_
