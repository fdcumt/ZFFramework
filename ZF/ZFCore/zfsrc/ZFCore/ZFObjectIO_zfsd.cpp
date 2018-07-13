/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFObjectIO_zfsd.h"

ZF_NAMESPACE_GLOBAL_BEGIN

ZFOBJECTIO_DEFINE(zfsd, {
        return ZFObjectIOImplCheck(pathInfo, zfText("zfsd"));
    }, {
        ZFSerializableData data;
        if(!ZFSerializableDataFromInput(data, input, outErrorHint))
        {
            return zffalse;
        }
        return ZFObjectFromData(ret, data, outErrorHint);
    }, {
        ZFSerializableData data;
        if(!ZFObjectToData(data, obj, outErrorHint))
        {
            return zffalse;
        }
        return ZFSerializableDataToOutput(output, data, outErrorHint);
    })

ZF_NAMESPACE_GLOBAL_END

