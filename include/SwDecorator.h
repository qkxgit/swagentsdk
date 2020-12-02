#ifndef __SWDECORATOR__
#define __SWDECORATOR__


#include "SwContextManager.h"
#include "SwContextSnapshot.h"
#include "SwSpan.h"
#include "SwAgent.h"
/*
* V表示绑定函数的返回值为void，后缀数字表示绑定函数的参数个数，最多支持8个参数
成员函数修饰器
SwDecorator
SwDecoratorV
成员函数绑定绑定函数
Bind
BindV
全局函数修饰器
SwGDecorator
SwGDecoratorV
全局函数绑定函数
GBind
GBindV
*/
#define Template0 template<typename RetType, typename ObjectType>
#define Template1 template<typename RetType, typename ObjectType, typename Arg1>
#define Template2 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2>
#define Template3 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3>
#define Template4 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define Template5 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define Template6 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define Template7 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define Template8 template<typename RetType, typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>

#define TemplateV0 template<typename ObjectType>
#define TemplateV1 template<typename ObjectType, typename Arg1>
#define TemplateV2 template<typename ObjectType, typename Arg1, typename Arg2>
#define TemplateV3 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3>
#define TemplateV4 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define TemplateV5 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define TemplateV6 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define TemplateV7 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define TemplateV8 template<typename ObjectType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>

#define GTemplate0 template<typename RetType>
#define GTemplate1 template<typename RetType, typename Arg1>
#define GTemplate2 template<typename RetType, typename Arg1, typename Arg2>
#define GTemplate3 template<typename RetType, typename Arg1, typename Arg2, typename Arg3>
#define GTemplate4 template<typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define GTemplate5 template<typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define GTemplate6 template<typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define GTemplate7 template<typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define GTemplate8 template<typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>

#define GTemplateV0 
#define GTemplateV1 template<typename Arg1>
#define GTemplateV2 template<typename Arg1, typename Arg2>
#define GTemplateV3 template<typename Arg1, typename Arg2, typename Arg3>
#define GTemplateV4 template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
#define GTemplateV5 template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
#define GTemplateV6 template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
#define GTemplateV7 template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
#define GTemplateV8 template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>


#define ParamType0
#define ParamType1 Arg1
#define ParamType2 Arg1,Arg2
#define ParamType3 Arg1,Arg2,Arg3
#define ParamType4 Arg1,Arg2,Arg3,Arg4
#define ParamType5 Arg1,Arg2,Arg3,Arg4,Arg5
#define ParamType6 Arg1,Arg2,Arg3,Arg4,Arg5,Arg6
#define ParamType7 Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7
#define ParamType8 Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8

#define FormalParam0
#define FormalParam1 Arg1 arg1
#define FormalParam2 Arg1 arg1, Arg2 arg2
#define FormalParam3 Arg1 arg1, Arg2 arg2, Arg3 arg3
#define FormalParam4 Arg1 arg1, Arg2 arg2, Arg3 arg3,Arg4 arg4
#define FormalParam5 Arg1 arg1, Arg2 arg2, Arg3 arg3,Arg4 arg4,Arg5 arg5
#define FormalParam6 Arg1 arg1, Arg2 arg2, Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6
#define FormalParam7 Arg1 arg1, Arg2 arg2, Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6,Arg7 arg7
#define FormalParam8 Arg1 arg1, Arg2 arg2, Arg3 arg3,Arg4 arg4,Arg5 arg5,Arg6 arg6,Arg7 arg7,Arg8 arg8

#define ActualParam0
#define ActualParam1 arg1
#define ActualParam2 arg1,arg2
#define ActualParam3 arg1,arg2,arg3
#define ActualParam4 arg1,arg2,arg3,arg4
#define ActualParam5 arg1,arg2,arg3,arg4,arg5
#define ActualParam6 arg1,arg2,arg3,arg4,arg5,arg6
#define ActualParam7 arg1,arg2,arg3,arg4,arg5,arg6,arg7
#define ActualParam8 arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8

#define Jv(a,b) a##b

struct SwParameter
{
	SwEnumSpanKind kind;
	std::string operationName;
	SwContextSnapshot snapshot;
	SwContextCarrier carrier;
	std::string peer;

	SwEnumComponent component;
	SwEnumSpanLayer layer;
	StringStringMap tags;

	inline bool IsValid() const { return !operationName.empty() && component != CUnknown && layer != LUnknown; }

	SwParameter()
		:component(CUnknown),layer(LUnknown)
	{

	}

	// local span
	SwParameter(const SwContextSnapshot& ss, const std::string &op)
		:operationName(op), kind(Local), snapshot(ss), component(rpc), layer(RPCFramework)
	{
		
	}

	// exit span
	SwParameter(const std::string& op, const SwContextSnapshot& ss = SwContextSnapshot())
		:operationName(op), kind(Exit), snapshot(ss), component(rpc), layer(RPCFramework)
	{
		peer = AgentInst::GetRef().GetLocalIp();
	}

	// entry span
	SwParameter(const std::string& op, const StringStringMap& c)
		:operationName(op), kind(Entry), component(rpc), layer(RPCFramework)
	{
		carrier.FromIceContext(c);
	}
};



template<typename ArgType>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, const ArgType& t) {}

template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, const StringStringMap& t);

template<typename ArgType>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, ArgType& t) {}

template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, StringStringMap& t);

SwSpan* CreateSpan(const SwParameter& dat, StringStringMap& ctx, SwContextSnapshot& ss);

#define StartSpan(p, s) \
StringStringMap ctx;\
SwSpan* span = CreateSpan(p, ctx, s);\
try\
{

#define FinishSpan(op) \
}\
catch (const std::exception& e)\
{\
	span->SetError(op, e.what());\
	span->Stop();\
	throw;\
}\
catch (...)\
{\
	span->SetError(op, "unknown exception");\
	span->Stop();\
	throw;\
}\
span->Stop();

Template0
class SwDecorator0
{
	typedef RetType(ObjectType::* NormFunc)();
	typedef RetType(ObjectType::* ConstFunc)() const;
public:
	SwDecorator0(ObjectType* obj, NormFunc nf, const SwParameter& dat)
		: object(obj), normFunc(nf), constFunc(NULL), dat(dat) {}

	SwDecorator0(ObjectType* obj, ConstFunc cf, const SwParameter& dat)
		: object(obj), normFunc(NULL), constFunc(cf), dat(dat) {}

	SwDecorator0():object(NULL), normFunc(NULL), constFunc(NULL) {}
	inline bool IsValid() const { return object != NULL && !(normFunc == NULL && constFunc == NULL) && dat.IsValid(); }

	RetType operator()() const
	{
		assert(IsValid());
		RetType rc = RetType();
		StartSpan(dat, snapshot)
			rc = InnerCaller();
		FinishSpan(dat.operationName)
			return rc;
	}
	const SwContextSnapshot& GetSnapshot() const { return snapshot; }
private:
	RetType InnerCaller() const
	{
		if (normFunc)
			return (object->*normFunc)();
		else if (constFunc)
			return (object->*constFunc)();
		else
			throw "function pointer is null";
	}
private:
	NormFunc normFunc;
	ConstFunc constFunc;
	ObjectType* object;
	SwParameter dat;
	mutable SwContextSnapshot snapshot;
};

#define DecoratorTemplate(ArgCount) \
Jv(Template,ArgCount)\
class Jv(SwDecorator,ArgCount)\
{\
	typedef RetType(ObjectType::* NormFunc)(Jv(ParamType,ArgCount));\
	typedef RetType(ObjectType::* ConstFunc)(Jv(ParamType,ArgCount)) const;\
public:\
	Jv(SwDecorator,ArgCount)(ObjectType* obj, NormFunc nf, const SwParameter& dat)\
		:object(obj), normFunc(nf), constFunc(NULL), dat(dat) {}\
\
	Jv(SwDecorator,ArgCount)(ObjectType* obj, ConstFunc cf, const SwParameter& dat)\
		:object(obj), normFunc(NULL), constFunc(cf), dat(dat) {}\
	Jv(SwDecorator,ArgCount)():object(NULL), normFunc(NULL), constFunc(NULL) {}\
	inline bool IsValid() const { return object != NULL && !(normFunc == NULL && constFunc == NULL) && dat.IsValid(); }\
	RetType operator()(Jv(FormalParam,ArgCount)) const\
	{\
		assert(IsValid());\
		RetType rc = RetType();\
		StartSpan(dat, snapshot)\
		SetCarrier(dat.kind, ctx, Jv(arg, ArgCount));\
		rc = InnerCaller(Jv(ActualParam,ArgCount));\
		FinishSpan(dat.operationName)\
		return rc;\
	}\
	const SwContextSnapshot &GetSnapshot() const {return snapshot; }\
private:\
	RetType InnerCaller(Jv(FormalParam,ArgCount)) const\
	{\
		if (normFunc)\
			return (object->*normFunc)(Jv(ActualParam,ArgCount));\
		else if (constFunc)\
			return (object->*constFunc)(Jv(ActualParam,ArgCount));\
		else\
			throw "function pointer is null";\
	}\
private:\
	NormFunc normFunc;\
	ConstFunc constFunc;\
	ObjectType* object;\
	SwParameter dat;\
	mutable SwContextSnapshot snapshot;\
};
DecoratorTemplate(1)
DecoratorTemplate(2)
DecoratorTemplate(3)
DecoratorTemplate(4)
DecoratorTemplate(5)
DecoratorTemplate(6)
DecoratorTemplate(7)
DecoratorTemplate(8)

#define BindTemplate(ArgCount) \
Jv(Template, ArgCount)\
Jv(SwDecorator, ArgCount)<RetType, ObjectType, Jv(ParamType, ArgCount)> \
Jv(Bind, ArgCount)(ObjectType* obj, RetType(ObjectType::* nf)(Jv(ParamType, ArgCount)), const SwParameter& dat)\
{\
	assert(obj != NULL && nf != NULL);\
	return Jv(SwDecorator, ArgCount)<RetType, ObjectType, Jv(ParamType, ArgCount)>(obj, nf, dat);\
};
Template0
SwDecorator0<RetType, ObjectType> Bind0(ObjectType* obj, RetType(ObjectType::* nf)(), const SwParameter& dat)
{
	assert(obj != NULL && nf != NULL);
	return SwDecorator0<RetType, ObjectType>(obj, nf, dat);
};
BindTemplate(1)
BindTemplate(2)
BindTemplate(3)
BindTemplate(4)
BindTemplate(5)
BindTemplate(6)
BindTemplate(7)
BindTemplate(8)

#define BindConstTemplate(ArgCount) \
Jv(Template, ArgCount)\
Jv(SwDecorator, ArgCount)<RetType, ObjectType, Jv(ParamType, ArgCount)> \
Jv(BindC, ArgCount)(ObjectType* obj, RetType(ObjectType::* nf)(Jv(ParamType, ArgCount)) const, const SwParameter& dat)\
{\
	assert(obj != NULL && nf != NULL);\
	return Jv(SwDecorator, ArgCount)<RetType, ObjectType, Jv(ParamType, ArgCount)>(obj, nf, dat);\
};
Template0
SwDecorator0<RetType, ObjectType> BindC0(ObjectType* obj, RetType(ObjectType::* nf)() const, const SwParameter& dat)
{
	assert(obj != NULL && nf != NULL); 
	return SwDecorator0<RetType, ObjectType>(obj, nf, dat);
}
BindConstTemplate(1)
BindConstTemplate(2)
BindConstTemplate(3)
BindConstTemplate(4)
BindConstTemplate(5)
BindConstTemplate(6)
BindConstTemplate(7)
BindConstTemplate(8)


TemplateV0
class SwDecoratorV0
{
	typedef void(ObjectType::* NormFunc)();
	typedef void(ObjectType::* ConstFunc)() const;
public:
	SwDecoratorV0(ObjectType* obj, NormFunc nf, const SwParameter& dat)
		:object(obj), normFunc(nf), constFunc(NULL), dat(dat) {}
	SwDecoratorV0(ObjectType* obj, ConstFunc cf, const SwParameter& dat)
		:object(obj), normFunc(NULL), constFunc(cf), dat(dat) {}
	SwDecoratorV0() :object(NULL), normFunc(NULL), constFunc(NULL) {}
	inline bool IsValid() const { return object != NULL && !(normFunc == NULL && constFunc == NULL) && dat.IsValid(); }
	void operator()() const
	{
		assert(IsValid());
		StartSpan(dat, snapshot)
			InnerCaller();
		FinishSpan(dat.operationName)
	}
	const SwContextSnapshot& GetSnapshot() const { return snapshot; }
private:
	void InnerCaller() const
	{
		if (normFunc)
			(object->*normFunc)();
		else if (constFunc)
			(object->*constFunc)();
		else
			throw "function pointer is null";
	}

private:
	NormFunc normFunc;
	ConstFunc constFunc;
	ObjectType* object;
	SwParameter dat;
	mutable SwContextSnapshot snapshot;
};

#define DecoratorVTemplate(ArgCount) \
Jv(TemplateV,ArgCount)\
class Jv(SwDecoratorV,ArgCount)\
{\
	typedef void(ObjectType::* NormFunc)(Jv(ParamType,ArgCount));\
	typedef void(ObjectType::* ConstFunc)(Jv(ParamType,ArgCount)) const;\
public:\
	Jv(SwDecoratorV,ArgCount)(ObjectType* obj, NormFunc nf, const SwParameter& dat)\
		:object(obj), normFunc(nf), constFunc(NULL), dat(dat) {}\
\
	Jv(SwDecoratorV,ArgCount)(ObjectType* obj, ConstFunc cf, const SwParameter& dat)\
		:object(obj), normFunc(NULL), constFunc(cf), dat(dat) {}\
	Jv(SwDecoratorV,ArgCount)():object(NULL), normFunc(NULL), constFunc(NULL) {}\
	inline bool IsValid() const { return object != NULL && !(normFunc == NULL && constFunc == NULL) && dat.IsValid(); }\
	void operator()(Jv(FormalParam,ArgCount)) const\
	{\
		assert(IsValid());\
		StartSpan(dat, snapshot)\
		SetCarrier(dat.kind, ctx, Jv(arg, ArgCount));\
		InnerCaller(Jv(ActualParam,ArgCount));\
		FinishSpan(dat.operationName)\
	}\
	const SwContextSnapshot &GetSnapshot() const {return snapshot; }\
private:\
	void InnerCaller(Jv(FormalParam,ArgCount)) const\
	{\
		if (normFunc)\
			(object->*normFunc)(Jv(ActualParam,ArgCount));\
		else if (constFunc)\
			(object->*constFunc)(Jv(ActualParam,ArgCount));\
		else\
			throw "function pointer is null";\
	}\
\
private:\
	NormFunc normFunc;\
	ConstFunc constFunc;\
	ObjectType* object;\
	SwParameter dat;\
	mutable SwContextSnapshot snapshot;\
};
DecoratorVTemplate(1)
DecoratorVTemplate(2)
DecoratorVTemplate(3)
DecoratorVTemplate(4)
DecoratorVTemplate(5)
DecoratorVTemplate(6)
DecoratorVTemplate(7)
DecoratorVTemplate(8)

#define BindVTemplate(ArgCount) \
Jv(TemplateV, ArgCount)\
Jv(SwDecoratorV, ArgCount)<ObjectType, Jv(ParamType, ArgCount)> \
Jv(BindV, ArgCount)(ObjectType* obj, void(ObjectType::* nf)(Jv(ParamType, ArgCount)), const SwParameter& dat)\
{\
	assert(obj != NULL && nf != NULL);\
	return Jv(SwDecoratorV, ArgCount)<ObjectType, Jv(ParamType, ArgCount)>(obj, nf, dat);\
};
TemplateV0
SwDecoratorV0<ObjectType> BindV0(ObjectType* obj, void(ObjectType::* nf)(), const SwParameter& dat)
{
	assert(obj != NULL && nf != NULL);
	return SwDecoratorV0<ObjectType>(obj, nf, dat);
};
BindVTemplate(1)
BindVTemplate(2)
BindVTemplate(3)
BindVTemplate(4)
BindVTemplate(5)
BindVTemplate(6)
BindVTemplate(7)
BindVTemplate(8)

#define BindVConstTemplate(ArgCount) \
Jv(TemplateV, ArgCount)\
Jv(SwDecoratorV, ArgCount)<ObjectType, Jv(ParamType, ArgCount)> \
Jv(BindVC, ArgCount)(ObjectType* obj, void(ObjectType::* nf)(Jv(ParamType, ArgCount)) const, const SwParameter& dat)\
{\
	assert(obj != NULL && nf != NULL);\
	return Jv(SwDecoratorV, ArgCount)<ObjectType, Jv(ParamType, ArgCount)>(obj, nf, dat);\
};
TemplateV0
SwDecoratorV0<ObjectType> BindVC0(ObjectType* obj, void(ObjectType::* nf)() const, const SwParameter& dat)
{
	assert(obj != NULL && nf != NULL);
	return SwDecoratorV0<ObjectType>(obj, nf, dat);
};
BindVConstTemplate(1)
BindVConstTemplate(2)
BindVConstTemplate(3)
BindVConstTemplate(4)
BindVConstTemplate(5)
BindVConstTemplate(6)
BindVConstTemplate(7)
BindVConstTemplate(8)


// global function template and binder
GTemplate0
class SwGDecorator0
{
	typedef RetType(*NormFunc)();
public:
	SwGDecorator0(NormFunc nf, const SwParameter& dat)
		:normFunc(nf), dat(dat) {}
	SwGDecorator0() :normFunc(NULL){}
	inline bool IsValid() const { return normFunc != NULL && dat.IsValid(); }
	RetType operator()() const
	{
		assert(IsValid());
		RetType rc = RetType();
		StartSpan(dat, snapshot)
			rc = InnerCaller();
		FinishSpan(dat.operationName)
			return rc;
	}

	const SwContextSnapshot& GetSnapshot() const { return snapshot; }
private:
	RetType InnerCaller() const
	{
		if (normFunc)
			return (*normFunc)();
		else
			throw "function pointer is null";
	}
private:
	NormFunc normFunc;
	SwParameter dat;
	mutable SwContextSnapshot snapshot;
};

#define GDecoratorTemplate(ArgCount) \
Jv(GTemplate,ArgCount)\
class Jv(SwGDecorator,ArgCount)\
{\
	typedef RetType(* NormFunc)(Jv(ParamType,ArgCount));\
public:\
	Jv(SwGDecorator,ArgCount)(NormFunc nf, const SwParameter& dat)\
		:normFunc(nf), dat(dat) {}\
	Jv(SwGDecorator,ArgCount)():normFunc(NULL) {}\
	inline bool IsValid() const { return normFunc != NULL && dat.IsValid(); }\
	RetType operator()(Jv(FormalParam,ArgCount)) const\
	{\
		assert(IsValid());\
		RetType rc = RetType();\
		StartSpan(dat, snapshot)\
		SetCarrier(dat.kind, ctx, Jv(arg, ArgCount));\
		rc = InnerCaller(Jv(ActualParam,ArgCount));\
		FinishSpan(dat.operationName)\
		return rc;\
	}\
\
	const SwContextSnapshot &GetSnapshot() const {return snapshot; }\
private:\
	RetType InnerCaller(Jv(FormalParam,ArgCount)) const\
	{\
		if (normFunc)\
			return (*normFunc)(Jv(ActualParam,ArgCount));\
		else\
			throw "function pointer is null";\
	}\
private:\
	NormFunc normFunc;\
	SwParameter dat;\
	mutable SwContextSnapshot snapshot;\
};
GDecoratorTemplate(1)
GDecoratorTemplate(2)
GDecoratorTemplate(3)
GDecoratorTemplate(4)
GDecoratorTemplate(5)
GDecoratorTemplate(6)
GDecoratorTemplate(7)
GDecoratorTemplate(8)

#define GBindTemplate(ArgCount) \
Jv(GTemplate, ArgCount)\
Jv(SwGDecorator, ArgCount)<RetType, Jv(ParamType, ArgCount)> \
Jv(GBind, ArgCount)(RetType(* nf)(Jv(ParamType, ArgCount)), const SwParameter& dat)\
{\
	assert(nf != NULL);\
	return Jv(SwGDecorator, ArgCount)<RetType, Jv(ParamType, ArgCount)>(nf, dat);\
};
GTemplate0
SwGDecorator0<RetType> GBind0(RetType(* nf)(), const SwParameter& dat)
{
	assert(nf != NULL);
	return SwGDecorator0<RetType>(nf, dat);
};
GBindTemplate(1)
GBindTemplate(2)
GBindTemplate(3)
GBindTemplate(4)
GBindTemplate(5)
GBindTemplate(6)
GBindTemplate(7)
GBindTemplate(8)


GTemplateV0
class SwGDecoratorV0
{
	typedef void(*NormFunc)();
public:
	SwGDecoratorV0(NormFunc nf, const SwParameter& dat)
		:normFunc(nf), dat(dat) {}
	SwGDecoratorV0() :normFunc(NULL) {}
	inline bool IsValid() const { return normFunc != NULL && dat.IsValid(); }
	void operator()() const
	{
		assert(IsValid());
		StartSpan(dat, snapshot)
			InnerCaller();
		FinishSpan(dat.operationName)
	}

private:
	void InnerCaller() const
	{
		if (normFunc)
			(*normFunc)();
		else
			throw "function pointer is null";
	}

	const SwContextSnapshot& GetSnapshot() const { return snapshot; }
private:
	NormFunc normFunc;
	SwParameter dat;
	mutable SwContextSnapshot snapshot;
};

#define GDecoratorVTemplate(ArgCount) \
Jv(GTemplateV,ArgCount)\
class Jv(SwGDecoratorV,ArgCount)\
{\
	typedef void(* NormFunc)(Jv(ParamType,ArgCount));\
public:\
	Jv(SwGDecoratorV,ArgCount)(NormFunc nf, const SwParameter& dat)\
		:normFunc(nf), dat(dat) {}\
	Jv(SwGDecoratorV,ArgCount)():normFunc(NULL) {}\
	inline bool IsValid() const { return normFunc != NULL && dat.IsValid(); }\
	void operator()(Jv(FormalParam,ArgCount)) const\
	{\
		assert(IsValid());\
		StartSpan(dat, snapshot)\
		SetCarrier(dat.kind, ctx, Jv(arg, ArgCount));\
		InnerCaller(Jv(ActualParam,ArgCount));\
		FinishSpan(dat.operationName)\
	}\
\
private:\
	void InnerCaller(Jv(FormalParam,ArgCount)) const\
	{\
		if (normFunc)\
			(*normFunc)(Jv(ActualParam,ArgCount));\
		else\
			throw "function pointer is null";\
	}\
\
	const SwContextSnapshot &GetSnapshot() const {return snapshot; }\
private:\
	NormFunc normFunc;\
	SwParameter dat;\
	mutable SwContextSnapshot snapshot;\
};

GDecoratorVTemplate(1)
GDecoratorVTemplate(2)
GDecoratorVTemplate(3)
GDecoratorVTemplate(4)
GDecoratorVTemplate(5)
GDecoratorVTemplate(6)
GDecoratorVTemplate(7)
GDecoratorVTemplate(8)

#define GBindVTemplate(ArgCount) \
Jv(GTemplateV, ArgCount)\
Jv(SwGDecoratorV, ArgCount)<Jv(ParamType, ArgCount)> \
Jv(GBindV, ArgCount)(void(* nf)(Jv(ParamType, ArgCount)), const SwParameter& dat)\
{\
	assert(nf != NULL);\
	return Jv(SwGDecoratorV, ArgCount)<Jv(ParamType, ArgCount)>(nf, dat);\
};
SwGDecoratorV0 BindV0(void(*nf)(), const SwParameter& dat);
GBindVTemplate(1)
GBindVTemplate(2)
GBindVTemplate(3)
GBindVTemplate(4)
GBindVTemplate(5)
GBindVTemplate(6)
GBindVTemplate(7)
GBindVTemplate(8)
#endif // !__SWDECORATOR__