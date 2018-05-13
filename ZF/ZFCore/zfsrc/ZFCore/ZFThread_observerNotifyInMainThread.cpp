/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFThread_observerNotifyInMainThread.h"
#include "ZFCacheable.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
zfclass _ZFP_I_ZFObserverNotifyInMainThreadTaskData : zfextends ZFObject, zfimplements ZFCacheable
{
    ZFOBJECT_DECLARE_WITH_CUSTOM_CTOR(_ZFP_I_ZFObserverNotifyInMainThreadTaskData, ZFObject)
    ZFIMPLEMENTS_DECLARE(ZFCacheable)
    ZFCACHEABLE_DECLARE(_ZFP_I_ZFObserverNotifyInMainThreadTaskData)

public:
    _ZFP_I_ZFObserverNotifyInMainThreadTaskData(void)
    : obj(zfnull)
    , customSender(zfnull)
    , param0(zfnull)
    , param1(zfnull)
    {
    }

public: // use direct pointer for performance
    ZFObject *obj;
    inline void objSet(ZF_IN ZFObject *obj) {zfRetain(obj); zfRelease(this->obj); this->obj = obj;}

    ZFObject *customSender;
    inline void customSenderSet(ZF_IN ZFObject *customSender) {zfRetain(customSender); zfRelease(this->customSender); this->customSender = customSender;}

    zfidentity eventId;

    ZFObject *param0;
    inline void param0Set(ZF_IN ZFObject *param0) {zfRetain(param0); zfRelease(this->param0); this->param0 = param0;}

    ZFObject *param1;
    inline void param1Set(ZF_IN ZFObject *param1) {zfRetain(param1); zfRelease(this->param1); this->param1 = param1;}

private:
    inline void removeAll(void)
    {
        this->objSet(zfnull);
        this->customSenderSet(zfnull);
        this->param0Set(zfnull);
        this->param1Set(zfnull);
    }

public:
    zfoverride
    virtual void objectOnDealloc(void)
    {
        this->removeAll();
        zfsuper::objectOnDealloc();
    }

public:
    zfoverride
    virtual void cacheableOnReset(void)
    {
        zfsuperI(ZFCacheable)::cacheableOnReset();
        this->removeAll();
    }
};
ZFOBJECT_REGISTER(_ZFP_I_ZFObserverNotifyInMainThreadTaskData)
ZFCACHEABLE_DEFINE(_ZFP_I_ZFObserverNotifyInMainThreadTaskData, _ZFP_I_ZFObserverNotifyInMainThreadTaskData)

// ============================================================
static ZFListener *_ZFP_ZFObserverNotifyInMainThreadCallback = zfnull;
ZF_GLOBAL_INITIALIZER_INIT_WITH_LEVEL(ZFObserverNotifyInMainThreadDataHolder, ZFLevelZFFrameworkEssential)
{
    this->callback = ZFCallbackForFunc(zfself::callbackAction);
    _ZFP_ZFObserverNotifyInMainThreadCallback = &this->callback;
}
ZF_GLOBAL_INITIALIZER_DESTROY(ZFObserverNotifyInMainThreadDataHolder)
{
    _ZFP_ZFObserverNotifyInMainThreadCallback = zfnull;
}
private:
    ZFListener callback;
    static ZFLISTENER_PROTOTYPE_EXPAND(callbackAction)
    {
        _ZFP_I_ZFObserverNotifyInMainThreadTaskData *taskData = userData->to<_ZFP_I_ZFObserverNotifyInMainThreadTaskData *>();
        taskData->obj->observerNotifyWithCustomSender(
            taskData->customSender,
            taskData->eventId,
            taskData->param0,
            taskData->param1);
        _ZFP_I_ZFObserverNotifyInMainThreadTaskData::cacheAdd(taskData);
    }
ZF_GLOBAL_INITIALIZER_END(ZFObserverNotifyInMainThreadDataHolder)

ZFMETHOD_FUNC_DEFINE_5(zfidentity, ZFObserverNotifyInMainThreadWithCustomSender,
                       ZFMP_IN(ZFObject *, customSender),
                       ZFMP_IN(ZFObject *, obj),
                       ZFMP_IN(const zfidentity &, eventId),
                       ZFMP_IN_OPT(ZFObject *, param0, zfnull),
                       ZFMP_IN_OPT(ZFObject *, param1, zfnull))
{
    if(obj == zfnull)
    {
        return zfidentityInvalid();
    }
    if(_ZFP_ZFObserverNotifyInMainThreadCallback)
    {
        _ZFP_I_ZFObserverNotifyInMainThreadTaskData *taskData = _ZFP_I_ZFObserverNotifyInMainThreadTaskData::cacheGet();
        taskData->objSet(obj);
        taskData->customSenderSet(customSender);
        taskData->eventId = eventId;
        taskData->param0Set(param0);
        taskData->param1Set(param1);
        return ZFThreadTaskRequest(*_ZFP_ZFObserverNotifyInMainThreadCallback,
            taskData,
            zfnull,
            zfnull,
            zfnull,
            ZFThreadTaskRequestMergeCallbackDoNotMerge());
    }
    else
    {
        obj->observerNotifyWithCustomSender(customSender, eventId, param0, param1);
        return zfidentityInvalid();
    }
}
ZFMETHOD_FUNC_DEFINE_INLINE_4(zfidentity, ZFObserverNotifyInMainThread,
                              ZFMP_IN(ZFObject *, obj),
                              ZFMP_IN(const zfidentity &, eventId),
                              ZFMP_IN_OPT(ZFObject *, param0, zfnull),
                              ZFMP_IN_OPT(ZFObject *, param1, zfnull))
ZFMETHOD_FUNC_DEFINE_1(void, ZFObserverNotifyInMainThreadCancel,
                       ZFMP_IN(zfidentity, taskId))
{
    ZFThreadTaskCancel(taskId);
}

ZF_NAMESPACE_GLOBAL_END

