/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFImpl_ZFLua_PathInfo.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
ZF_GLOBAL_INITIALIZER_INIT_WITH_LEVEL(ZFImpl_ZFLua_implPathInfoData, ZFLevelZFFrameworkEssential)
{
    this->needUpdateGlobalFunc = zftrue;
}
zfbool needUpdateGlobalFunc;
ZFCoreArray<zfstring> luaFuncName;
ZFCoreArray<zfstring> luaFuncBody;
ZF_GLOBAL_INITIALIZER_END(ZFImpl_ZFLua_implPathInfoData)

// ============================================================
static void _ZFP_ZFImpl_ZFLua_implPathInfoSetup_escape(ZF_OUT zfstring &ret,
                                                       ZF_IN const zfchar *p);
void ZFImpl_ZFLua_implPathInfoSetup(ZF_IN lua_State *L,
                                    ZF_OUT zfstring &ret,
                                    ZF_IN const ZFPathInfo *pathInfo)
{
    if(pathInfo == zfnull ||  pathInfo->pathType.isEmpty() || pathInfo->pathData.isEmpty())
    {
        return ;
    }
    ZF_GLOBAL_INITIALIZER_CLASS(ZFImpl_ZFLua_implPathInfoData) *d = ZF_GLOBAL_INITIALIZER_INSTANCE(ZFImpl_ZFLua_implPathInfoData);
    ZFCoreArray<zfstring> &luaFuncName = d->luaFuncName;

    if(d->needUpdateGlobalFunc)
    {
        zfstring cmd;
        if(luaFuncName.isEmpty())
        {
            cmd += zfText("_ZFP_l=nil;");
        }
        else
        {
            cmd += zfText("function _ZFP_l(zfl_l)");
            cmd += zfText("    return ");
            for(zfindex i = 0; i < d->luaFuncBody.count(); ++i)
            {
                cmd += d->luaFuncBody[i];
                cmd += ',';
            }
            cmd += zfText("    nil;");
            cmd += zfText("end;");
        }
        ZFImpl_ZFLua_execute(L, cmd);
    }
    if(luaFuncName.isEmpty())
    {
        return ;
    }

    // no endl, to prevent native lua error from having wrong line number
    ret += zfText("local ");
    for(zfindex i = 0; i < luaFuncName.count(); ++i)
    {
        if(i != 0)
        {
            ret += zfText(",");
        }
        ret += luaFuncName[i];
    }
    ret += zfText("=_ZFP_l(ZFPathInfo('");
    _ZFP_ZFImpl_ZFLua_implPathInfoSetup_escape(ret, pathInfo->pathType);
    ret += ZFSerializableKeyword_ZFPathInfo_separator;
    _ZFP_ZFImpl_ZFLua_implPathInfoSetup_escape(ret, pathInfo->pathData);
    ret += zfText("'));");
}
void _ZFP_ZFImpl_ZFLua_implPathInfoRegister(ZF_IN const zfchar *luaFuncName,
                                            ZF_IN const zfchar *luaFuncBody)
{
    ZF_GLOBAL_INITIALIZER_CLASS(ZFImpl_ZFLua_implPathInfoData) *d = ZF_GLOBAL_INITIALIZER_INSTANCE(ZFImpl_ZFLua_implPathInfoData);
    d->needUpdateGlobalFunc = zftrue;
    d->luaFuncName.add(luaFuncName);
    d->luaFuncBody.add(luaFuncBody);
}
void _ZFP_ZFImpl_ZFLua_implPathInfoUnregister(ZF_IN const zfchar *luaFuncName)
{
    ZF_GLOBAL_INITIALIZER_CLASS(ZFImpl_ZFLua_implPathInfoData) *d = ZF_GLOBAL_INITIALIZER_INSTANCE(ZFImpl_ZFLua_implPathInfoData);
    zfindex index = d->luaFuncName.find(luaFuncName);
    if(index == zfindexMax())
    {
        d->luaFuncName.remove(index);
        d->luaFuncBody.remove(index);
        d->needUpdateGlobalFunc = zftrue;
    }
}

// ============================================================
static void _ZFP_ZFImpl_ZFLua_implPathInfoSetup_escape(ZF_OUT zfstring &ret,
                                                       ZF_IN const zfchar *p)
{
    const zfchar *pL = p;
    while(*p)
    {
        if(*p == '\'')
        {
            ret.append(pL, p - pL);
            pL = p + 1;
            ret += zfText("\\'");
        }
        ++p;
    }
    if(p != pL)
    {
        ret.append(pL, p - pL);
    }
}

ZF_NAMESPACE_GLOBAL_END

