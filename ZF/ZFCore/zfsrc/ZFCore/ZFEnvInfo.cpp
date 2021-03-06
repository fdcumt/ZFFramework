/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFEnvInfo.h"
#include "protocol/ZFProtocolZFEnvInfo.h"

#include "ZFSTLWrapper/zfstl_map.h"
#include "ZFSTLWrapper/zfstl_string.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
ZF_GLOBAL_INITIALIZER_INIT_WITH_LEVEL(ZFEnvSummaryDataHolder, ZFLevelZFFrameworkStatic)
{
}
public:
    zfstlmap<zfstlstringZ, ZFEnvSummaryCallback> envSummaryCallbackMap;
ZF_GLOBAL_INITIALIZER_END(ZFEnvSummaryDataHolder)

// ============================================================
ZF_NAMESPACE_BEGIN(ZFEnvInfo)
/** @cond ZFPrivateDoc */ // ZFTAG_DOXYGEN_BUG: all uppercase macro not skipped within namespace

void envSummaryCallbackRegister(ZF_IN const zfchar *name, ZF_IN ZFEnvSummaryCallback callback)
{
    ZF_GLOBAL_INITIALIZER_INSTANCE(ZFEnvSummaryDataHolder)->envSummaryCallbackMap[name] = callback;
}
void envSummaryCallbackUnregister(ZF_IN const zfchar *name)
{
    ZF_GLOBAL_INITIALIZER_INSTANCE(ZFEnvSummaryDataHolder)->envSummaryCallbackMap.erase(name);
}

ZFMETHOD_FUNC_DEFINE_1(void, envSummary,
                       ZFMP_IN_OUT(zfstring &, ret))
{
    zfstring tmp;

    zfbool first = zftrue;
    zfstlmap<zfstlstringZ, ZFEnvSummaryCallback> &m = ZF_GLOBAL_INITIALIZER_INSTANCE(ZFEnvSummaryDataHolder)->envSummaryCallbackMap;
    for(zfstlmap<zfstlstringZ, ZFEnvSummaryCallback>::iterator it = m.begin(); it != m.end(); ++it)
    {
        if(first)
        {
            first = zffalse;
        }
        else
        {
            ret += zfText(", ");
        }
        ret += it->first.c_str();
        ret += zfText(": ");
        it->second(ret);
    }
}
ZFMETHOD_FUNC_DEFINE_0(zfstring, envSummary)
{
    zfstring ret;
    ZFEnvInfo::envSummary(ret);
    return ret;
}

// ============================================================
ZFMETHOD_FUNC_DEFINE_1(void, systemName,
                       ZFMP_IN_OUT(zfstring &, ret))
{
    ZFPROTOCOL_INTERFACE_CLASS(ZFEnvInfo_systemInfo) *impl = ZFPROTOCOL_TRY_ACCESS(ZFEnvInfo_systemInfo);
    if(impl != zfnull)
    {
        impl->systemName(ret);
    }
}
ZFMETHOD_FUNC_DEFINE_0(zfstring, systemName)
{
    zfstring ret;
    ZFEnvInfo::systemName(ret);
    return ret;
}
ZFMETHOD_FUNC_DEFINE_1(void, systemVersion,
                       ZFMP_IN_OUT(zfstring &, ret))
{
    ZFPROTOCOL_INTERFACE_CLASS(ZFEnvInfo_systemInfo) *impl = ZFPROTOCOL_TRY_ACCESS(ZFEnvInfo_systemInfo);
    if(impl != zfnull)
    {
        impl->systemVersion(ret);
    }
}
ZFMETHOD_FUNC_DEFINE_0(zfstring, systemVersion)
{
    zfstring ret;
    ZFEnvInfo::systemVersion(ret);
    return ret;
}

// ============================================================
ZFMETHOD_FUNC_DEFINE_1(void, frameworkName,
                       ZFMP_IN_OUT(zfstring &, ret))
{
    ZFPROTOCOL_INTERFACE_CLASS(ZFEnvInfo_frameworkInfo) *impl = ZFPROTOCOL_TRY_ACCESS(ZFEnvInfo_frameworkInfo);
    if(impl != zfnull)
    {
        impl->frameworkName(ret);
    }
}
ZFMETHOD_FUNC_DEFINE_0(zfstring, frameworkName)
{
    zfstring ret;
    ZFEnvInfo::frameworkName(ret);
    return ret;
}
ZFMETHOD_FUNC_DEFINE_1(void, frameworkVersion,
                       ZFMP_IN_OUT(zfstring &, ret))
{
    ZFPROTOCOL_INTERFACE_CLASS(ZFEnvInfo_frameworkInfo) *impl = ZFPROTOCOL_TRY_ACCESS(ZFEnvInfo_frameworkInfo);
    if(impl != zfnull)
    {
        impl->frameworkVersion(ret);
    }
}
ZFMETHOD_FUNC_DEFINE_0(zfstring, frameworkVersion)
{
    zfstring ret;
    ZFEnvInfo::frameworkVersion(ret);
    return ret;
}

/** @endcond */
ZF_NAMESPACE_END_WITH_REGISTER(ZFEnvInfo, ZF_NAMESPACE_GLOBAL)

// ============================================================
ZF_GLOBAL_INITIALIZER_INIT_WITH_LEVEL(ZFEnvSummary_common, ZFLevelZFFrameworkNormal)
{
    ZFEnvInfo::envSummaryCallbackRegister(zfText("systemName"), ZFEnvInfo::systemName);
    ZFEnvInfo::envSummaryCallbackRegister(zfText("systemVersion"), ZFEnvInfo::systemVersion);
    ZFEnvInfo::envSummaryCallbackRegister(zfText("frameworkName"), ZFEnvInfo::frameworkName);
    ZFEnvInfo::envSummaryCallbackRegister(zfText("frameworkVersion"), ZFEnvInfo::frameworkVersion);
}
ZF_GLOBAL_INITIALIZER_DESTROY(ZFEnvSummary_common)
{
    ZFEnvInfo::envSummaryCallbackUnregister(zfText("systemName"));
    ZFEnvInfo::envSummaryCallbackUnregister(zfText("systemVersion"));
    ZFEnvInfo::envSummaryCallbackUnregister(zfText("frameworkName"));
    ZFEnvInfo::envSummaryCallbackUnregister(zfText("frameworkVersion"));
}
ZF_GLOBAL_INITIALIZER_END(ZFEnvSummary_common)

ZF_NAMESPACE_GLOBAL_END

