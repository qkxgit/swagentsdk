/*
工具类，用于绑定待调用的函数，简化编码
date:2020/12/08
author:qkx
*/
#include "SwDecorator.h"

// 在ice context中写入SwCarrier 数据
template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, const StringStringMap& t)
{
	if (k == Exit && !c.empty())
	{
		StringStringMap& m = const_cast<StringStringMap&>(t);
		m.insert(c.begin(), c.end());
	}
}

// 在ice context中写入SwCarrier 数据
template<>
void SetCarrier(SwEnumSpanKind k, const StringStringMap& c, StringStringMap& t)
{
	if (k == Exit && !c.empty())
		t.insert(c.begin(), c.end());
}

// 创建span
// dat 绑定函数时传入的参数
// ctx ice context
// ss 待关联的snapshot
SwSpan* CreateSpan(const SwParameter& dat, StringStringMap& ctx, SwSnapshot& ss)
{
	SwSpan* span = NULL;
	SwContext& swCtx = SwContext::GetContext();
	switch (dat.kind)
	{
	case Entry:
	{
		span = swCtx.CreateEntrySpan(dat.operationName, dat.peer, dat.carrier);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		span->SetTag("LocalHost", swCtx.GetAgent()->GetLocalHost());
		break;
	}
	case Exit:
	{
		span = swCtx.CreateExitSpan(dat.operationName, dat.peer);
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			swCtx.Continued(dat.snapshot);
		SwCarrier carrier;
		span->Inject(carrier);
		ctx = carrier.ToIceContext();
		span->SetTag("LocalHost", swCtx.GetAgent()->GetLocalHost());
		break;
	}
	default:
		span = swCtx.CreateLocalSpan(dat.operationName, swCtx.GetAgent()->GetLocalHost());
		span->SetTags(dat.tags);
		span->SetLayer(dat.layer);
		span->SetComponent(dat.component);
		span->Start();
		if (dat.snapshot.IsValid())
			swCtx.Continued(dat.snapshot);
		span->SetTag("LocalHost", swCtx.GetAgent()->GetLocalHost());
		break;
	}
	ss = swCtx.Capture();
	return span;
}

// 绑定返回值为void无参数的全局函数
// nf 待绑定的函数
// dat 创建span时需要的参数
SwGDecoratorV0 BindV0(void(*nf)(), const SwParameter& dat)
{
	assert(nf != NULL);
	return SwGDecoratorV0(nf, dat);
};
