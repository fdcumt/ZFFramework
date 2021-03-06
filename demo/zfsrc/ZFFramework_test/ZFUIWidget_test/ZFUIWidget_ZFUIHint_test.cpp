/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFUIWidget_test.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclass ZFUIWidget_ZFUIHint_test : zfextends ZFFramework_test_TestCase
{
    ZFOBJECT_DECLARE(ZFUIWidget_ZFUIHint_test, ZFFramework_test_TestCase)

protected:
    zfoverride
    virtual void testCaseOnStart(void)
    {
        zfsuper::testCaseOnStart();
        ZFFramework_test_protocolCheck(ZFUIView)
        ZFFramework_test_protocolCheck(ZFUISysWindow)

        zfautoObject hint = ZFUIHintShow(zfText("this is a normal hint"));
#if 0
        ZFLISTENER_LOCAL(hintOnHide, {
            zfLogT();
            userData->objectHolded<ZFTestCase *>()->testCaseStop();
        })
#else
        ZFLISTENER_LOCAL(hintOnHide, {
            for(zfindex i = 0; i < 3; ++i)
            {
                ZFUIHintShow(zfstringWithFormat(zfText("this is a stacked hint %zi"), i));
            }
            zfautoObject last = ZFUIHintShow(zfText("this is a very")
                zfText(" long long long long long long long long long long")
                zfText(" long long long long long long long long long long")
                zfText(" long long long long long long long long long hint"));
            ZFLISTENER_LOCAL(lastHintOnHide, {
                userData->objectHolded<ZFTestCase *>()->testCaseStop();
            })
            last.toObject()->observerAdd(ZFUIHint::EventHintOnHide(), lastHintOnHide, userData);
        })
#endif
        hint.toObject()->observerAdd(ZFUIHint::EventHintOnHide(), hintOnHide, this->objectHolder());
    }
};
ZFOBJECT_REGISTER(ZFUIWidget_ZFUIHint_test)

ZF_NAMESPACE_GLOBAL_END

