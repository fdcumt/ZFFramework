/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFPropertyDynamicRegister.h"
#include "ZFObjectImpl.h"
#include "ZFListenerDeclare.h"

#include "../ZFSTLWrapper/zfstl_map.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
ZF_GLOBAL_INITIALIZER_INIT_WITH_LEVEL(ZFPropertyDynamicRegisterAutoRemove, ZFLevelZFFrameworkHigh)
{
}
ZF_GLOBAL_INITIALIZER_DESTROY(ZFPropertyDynamicRegisterAutoRemove)
{
    zfstlmap<const ZFProperty *, zfbool> t;
    t.swap(m);
    for(zfstlmap<const ZFProperty *, zfbool>::iterator it = t.begin(); it != t.end(); ++it)
    {
        _ZFP_ZFPropertyUnregister(it->first);
    }
}
zfstlmap<const ZFProperty *, zfbool> m;
ZF_GLOBAL_INITIALIZER_END(ZFPropertyDynamicRegisterAutoRemove)

// ============================================================
zfclass _ZFP_I_PropDynRetainHolder : zfextends ZFObject
{
    ZFOBJECT_DECLARE(_ZFP_I_PropDynRetainHolder, ZFObject)
public:
    zfautoObject zfv;

public:
    zfoverride
    ZFCompareResult objectCompare(ZF_IN ZFObject *anotherObj)
    {
        return ZFObjectCompare(this->zfv.toObject(), anotherObj);
    }
};
zfclass _ZFP_I_PropDynRegData : zfextends ZFObject
{
    ZFOBJECT_DECLARE(_ZFP_I_PropDynRegData, ZFObject)

public:
    const ZFTypeIdBase *d;
    ZFPropertyDynamicRegisterInitValueCallback initValueCallback;
    /*
     * for assign property, store ZFTypeIdWrapper, ensured not null if accessed
     * for retain property, store _ZFP_I_PropDynRetainHolder, ensured not null if accessed
     */
    zfstring tagKey;
    zfstring valueStoreKey;
public:
    void objectAttach(ZF_IN ZFObject *obj)
    {
        if(this->_objAttached.find(obj) == this->_objAttached.end())
        {
            obj->observerAdd(ZFObject::EventObjectBeforeDealloc(), this->_objOnDeallocListener);
        }
        this->_objAttached[obj] = zftrue;
    }
    void objectDetach(void)
    {
        zfstlmap<ZFObject *, zfbool> t;
        t.swap(this->_objAttached);
        for(zfstlmap<ZFObject *, zfbool>::iterator it = t.begin(); it != t.end(); ++it)
        {
            it->first->observerRemove(ZFObject::EventObjectBeforeDealloc(), this->_objOnDeallocListener);
            it->first->tagRemove(this->tagKey);
            it->first->tagRemove(this->valueStoreKey);
        }
    }
    /*
     * if success, ret ensured:
     * * store ZFTypeIdWrapper for assign property
     * * store _ZFP_I_PropDynRetainHolder for retain property
     *   or typeIdWrapper return null with success state
     */
    zfbool initValue(ZF_OUT zfautoObject &ret,
                     ZF_IN const ZFProperty *property,
                     ZF_OUT_OPT zfstring *errorHint = zfnull)
    {
        if(property->propertyIsRetainProperty())
        {
            zfblockedAlloc(_ZFP_I_PropDynRetainHolder, holder);
            if(this->initValueCallback != zfnull)
            {
                holder->zfv = this->initValueCallback(property);
            }
            if(holder->zfv != zfnull && !holder->zfv.toObject()->classData()->classIsTypeOf(property->propertyClassOfRetainProperty()))
            {
                zfstringAppend(errorHint,
                    zfText("invalid init value %s, desired: %s"),
                    ZFObjectInfo(holder->zfv.toObject()).cString(),
                    property->propertyClassOfRetainProperty()->className());
                return zffalse;
            }
            ret = holder;
        }
        else
        {
            if(this->initValueCallback != zfnull)
            {
                ret = this->initValueCallback(property);
            }
            if(ret == zfnull)
            {
                this->d->typeIdWrapper(ret);
                if(ret == zfnull)
                {
                    zfblockedAlloc(_ZFP_I_PropDynRetainHolder, holder);
                    ret = holder;
                    return zftrue;
                }
            }
            ZFTypeIdWrapper *wrapper = ret;
            if(wrapper == zfnull)
            {
                zfstringAppend(errorHint,
                    zfText("invalid init value %s, desired: %s"),
                    ZFObjectInfo(ret.toObject()).cString(),
                    property->propertyTypeId());
                return zffalse;
            }
        }
        return zftrue;
    }
private:
    zfstlmap<ZFObject *, zfbool> _objAttached;
    ZFListener _objOnDeallocListener;
    ZFLISTENER_INLINE(_objOnDealloc)
    {
        this->_objAttached.erase(listenerData.sender);
    }
protected:
    zfoverride
    virtual void objectOnInit(void)
    {
        zfsuper::objectOnInit();
        this->_objOnDeallocListener = ZFCallbackForMemberMethod(this, ZFMethodAccess(zfself, _objOnDealloc));
    }
};
zfclass _ZFP_I_PropDynRegValueStore : zfextends ZFObject
{
    ZFOBJECT_DECLARE(_ZFP_I_PropDynRegValueStore, ZFObject)

public:
    zfstlmap<void *, zfautoObject> m;
};

// ============================================================
static zfbool _ZFP_PropDynReg_setterGI(ZFMETHOD_GENERIC_INVOKER_PARAMS)
{
    const ZFProperty *property = invokerMethod->methodOwnerProperty();
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject valueOld = invokerObject->tagGet(d->tagKey);

    zfautoObject value;
    zfautoObject &valueNew = paramList[0];
    do {
        if(property->propertyIsRetainProperty())
        {
            if(valueNew != zfnull && !valueNew.toObject()->classData()->classIsTypeOf(property->propertyClassOfRetainProperty()))
            {
                break;
            }
            zfblockedAlloc(_ZFP_I_PropDynRetainHolder, holder);
            holder->zfv = valueNew;
            value = holder;
        }
        else
        {
            if(d->d->typeIdWrapper(value) && value == zfnull)
            {
                zfblockedAlloc(_ZFP_I_PropDynRetainHolder, holder);
                holder->zfv = valueNew;
                value = holder;
            }
            else
            {
                ZFTypeIdWrapper *wrapper = ZFCastZFObject(ZFTypeIdWrapper *, valueNew);
                if(wrapper == zfnull
                    || !zfscmpTheSame(wrapper->wrappedValueTypeId(), property->propertyTypeId()))
                {
                    break;
                }
                value = valueNew;
            }
        }
    } while(zffalse);
    if(value == zfnull)
    {
        zfstringAppend(errorHint, zfText("invalid value: %s, desired: %s"),
            ZFObjectInfo(valueNew).cString(),
            property->propertyIsRetainProperty()
                ? property->propertyClassOfRetainProperty()->className()
                : property->propertyTypeId());
        return zffalse;
    }
    if(valueOld == zfnull)
    {
        d->objectAttach(invokerObject);
    }
    invokerObject->tagSet(d->tagKey, value);
    if(valueOld == zfnull)
    {
        invokerObject->_ZFP_ZFObject_objectPropertyValueOnUpdate(property, zfnull);
    }
    else
    {
        _ZFP_I_PropDynRetainHolder *holder = valueOld;
        if(holder != zfnull)
        {
            invokerObject->_ZFP_ZFObject_objectPropertyValueOnUpdate(property, &(holder->zfv));
        }
        else
        {
            invokerObject->_ZFP_ZFObject_objectPropertyValueOnUpdate(property,
                    ZFCastZFObject(ZFTypeIdWrapper *, valueOld)->wrappedValue()
                );
        }
    }
    return zftrue;
}
static zfbool _ZFP_PropDynReg_getterGI(ZFMETHOD_GENERIC_INVOKER_PARAMS)
{
    const ZFProperty *property = invokerMethod->methodOwnerProperty();
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    ZFObject *wrapper = invokerObject->tagGet(d->tagKey);
    zfbool firstTime = (wrapper == zfnull);
    if(firstTime)
    {
        zfautoObject initValue;
        if(!d->initValue(initValue, property, errorHint))
        {
            return zffalse;
        }
        wrapper = initValue;
        invokerObject->tagSet(d->tagKey, initValue);
    }
    _ZFP_I_PropDynRetainHolder *wrapperTmp = ZFCastZFObject(_ZFP_I_PropDynRetainHolder *, wrapper);
    if(wrapperTmp != zfnull)
    {
        ret = wrapperTmp->zfv;
    }
    else
    {
        ret = wrapper;
    }
    if(firstTime)
    {
        d->objectAttach(invokerObject);
        invokerObject->_ZFP_ZFObject_objectPropertyValueOnUpdate(property, zfnull);
    }
    return zftrue;
}

static zfbool _ZFP_PropDynReg_callbackIsValueAccessed(ZF_IN const ZFProperty *property,
                                                      ZF_IN ZFObject *ownerObj)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    return (ownerObj->tagGet(d->tagKey) != zfnull);
}
static zfbool _ZFP_PropDynReg_callbackIsInitValue(ZF_IN const ZFProperty *property,
                                                  ZF_IN ZFObject *ownerObj,
                                                  ZF_OUT_OPT void *outInitValue /* = zfnull */)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfbool ret = zffalse;
    ZFObject *tag = ownerObj->tagGet(d->tagKey);
    if(tag == zfnull)
    {
        ret = zftrue;
        if(outInitValue != zfnull)
        {
            zfautoObject initValue;
            if(d->initValue(initValue, property))
            {
                _ZFP_I_PropDynRetainHolder *wrapperTmp = initValue;
                if(wrapperTmp != zfnull)
                {
                    *(zfautoObject *)outInitValue = wrapperTmp->zfv;
                }
                else
                {
                    initValue.to<ZFTypeIdWrapper *>()->wrappedValueGet(outInitValue);
                }
            }
        }
    }
    else
    {
        zfautoObject initValue;
        if(d->initValue(initValue, property))
        {
            _ZFP_I_PropDynRetainHolder *wrapperTmp = initValue;
            if(wrapperTmp != zfnull)
            {
                if(outInitValue != zfnull)
                {
                    *(zfautoObject *)outInitValue = wrapperTmp->zfv;
                }
            }
            else
            {
                if(outInitValue != zfnull)
                {
                    initValue.to<ZFTypeIdWrapper *>()->wrappedValueGet(outInitValue);
                }
            }
            ret = (tag->objectCompare(initValue) == ZFCompareTheSame);
        }
    }
    return ret;
}
static void _ZFP_PropDynReg_callbackValueSet(ZF_IN const ZFProperty *property,
                                             ZF_IN ZFObject *dstObj,
                                             ZF_IN const void *value)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject ret;
    zfautoObject paramList[ZFMETHOD_MAX_PARAM];
    zfautoObject &valueNew = paramList[0];
    d->d->typeIdWrapper(valueNew);
    if(valueNew == zfnull)
    {
        valueNew = *(const zfautoObject *)value;
    }
    else
    {
        ZFTypeIdWrapper *wrapper = valueNew;
        if(wrapper != zfnull)
        {
            wrapper->wrappedValueSet(value);
        }
    }
    _ZFP_PropDynReg_setterGI(property->setterMethod(), dstObj, zfnull, ret, paramList);
}
static const void *_ZFP_PropDynReg_callbackValueGet(ZF_IN const ZFProperty *property,
                                                    ZF_IN ZFObject *ownerObj)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject paramList[ZFMETHOD_MAX_PARAM];
    zfautoObject value;
    _ZFP_PropDynReg_getterGI(property->setterMethod(), ownerObj, zfnull, value, paramList);
    ZFObject *tag = ownerObj->tagGet(d->tagKey);
    if(tag == zfnull)
    {
        return zfnull;
    }
    _ZFP_I_PropDynRetainHolder *wrapperTmp = ZFCastZFObject(_ZFP_I_PropDynRetainHolder *, tag);
    if(wrapperTmp != zfnull)
    {
        return &(wrapperTmp->zfv);
    }
    else
    {
        return tag->to<ZFTypeIdWrapper *>()->wrappedValue();
    }
}
static void _ZFP_PropDynReg_callbackValueReset(ZF_IN const ZFProperty *property,
                                               ZF_IN ZFObject *ownerObj)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    ownerObj->tagRemove(d->tagKey);
}
static ZFCompareResult _ZFP_PropDynReg_callbackCompare(ZF_IN const ZFProperty *property,
                                                       ZF_IN ZFObject *ownerObj,
                                                       ZF_IN const void *v0,
                                                       ZF_IN const void *v1)
{
    if(property->propertyIsRetainProperty())
    {
        return ZFObjectCompare(
            ((const zfautoObject *)v0)->toObject(),
            ((const zfautoObject *)v1)->toObject());
    }
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject holder;
    if(d->d->typeIdWrapper(holder) && holder == zfnull)
    {
        return ZFObjectCompare(
            ((const zfautoObject *)v0)->toObject(),
            ((const zfautoObject *)v1)->toObject());
    }
    ZFTypeIdWrapper *wrapper = holder;
    if(wrapper != zfnull)
    {
        return wrapper->wrappedValueCompare(v0, v1);
    }
    else
    {
        return ZFCompareUncomparable;
    }
}
static void _ZFP_PropDynReg_callbackGetInfo(ZF_IN const ZFProperty *property,
                                            ZF_IN ZFObject *ownerObj,
                                            ZF_IN const void *value,
                                            ZF_IN_OUT zfstring &ret)
{
    if(property->propertyIsRetainProperty())
    {
        ZFObjectInfoT(ret, ((const zfautoObject *)value)->toObject());
        return ;
    }
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject holder;
    if(d->d->typeIdWrapper(holder) && holder == zfnull)
    {
        ZFObjectInfoT(ret, ((const zfautoObject *)value)->toObject());
        return ;
    }
    ZFTypeIdWrapper *wrapper = holder;
    if(wrapper != zfnull)
    {
        wrapper->wrappedValueGetInfo(ret, value);
    }
    else
    {
        ret += ZFTOKEN_ZFCoreInfoGetterNotAvailable;
    }
}
static void *_ZFP_PropDynReg_callbackValueStore(ZF_IN const ZFProperty *property,
                                                ZF_IN ZFObject *ownerObj,
                                                ZF_IN_OPT const void *value /* = zfnull */)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    _ZFP_I_PropDynRegValueStore *holder = ownerObj->tagGet<_ZFP_I_PropDynRegValueStore *>(d->valueStoreKey);
    if(holder == zfnull)
    {
        holder = zfAlloc(_ZFP_I_PropDynRegValueStore);
        ownerObj->tagSet(d->valueStoreKey, holder);
        zfRelease(holder);
    }
    if(!property->propertyIsRetainProperty())
    {
        zfautoObject store;
        if(d->d->typeIdWrapper(store) && store != zfnull)
        {
            ZFTypeIdWrapper *wrapper = store;
            wrapper->wrappedValueSet(value);
            void *ret = wrapper->wrappedValue();
            holder->m[ret] = store;
            return ret;
        }
    }
    zfblockedAlloc(_ZFP_I_PropDynRetainHolder, store);
    store->zfv = *(const zfautoObject *)value;
    void *ret = (void *)&store->zfv;
    holder->m[ret] = store;
    return ret;
}
static void _ZFP_PropDynReg_callbackValueRelease(ZF_IN const ZFProperty *property,
                                                 ZF_IN ZFObject *ownerObj,
                                                 ZF_IN void *valueStored)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    _ZFP_I_PropDynRegValueStore *holder = ownerObj->tagGet<_ZFP_I_PropDynRegValueStore *>(d->valueStoreKey);
    if(holder == zfnull)
    {
        return ;
    }
    holder->m.erase(valueStored);
}
static zfbool _ZFP_PropDynReg_callbackProgressUpdate(ZF_IN const ZFProperty *property,
                                                     ZF_IN ZFObject *ownerObj,
                                                     ZF_IN_OPT const void *from /* = zfnull */,
                                                     ZF_IN_OPT const void *to /* = zfnull */,
                                                     ZF_IN_OPT zffloat progress /* = 1 */)
{
    _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
    zfautoObject paramList[ZFMETHOD_MAX_PARAM];
    zfautoObject valueOld;
    _ZFP_PropDynReg_getterGI(property->getterMethod(), ownerObj, zfnull, valueOld, paramList);
    valueOld = ownerObj->tagGet(d->tagKey);

    _ZFP_I_PropDynRetainHolder *retain = valueOld;
    if(retain != zfnull)
    {
        // dynamic registerd retain property does not support progress update
        // since lack of template type
        // but you can use ZFPropertyCallbackProgressUpdateChange to supply custom progress update logic
        // typically there's no need for retain property's progress update, though
        return zffalse;
    }

    ZFTypeIdWrapper *wrapper = valueOld;
    if(wrapper == zfnull)
    {
        return zffalse;
    }
    zfautoObject &valueNew = paramList[0];
    valueNew = wrapper->copy();
    if(!valueNew->to<ZFTypeIdWrapper *>()->wrappedValueProgressUpdate(from, to, progress))
    {
        return zffalse;
    }
    if(from == zfnull)
    {
        return zftrue;
    }
    _ZFP_PropDynReg_setterGI(property->setterMethod(), ownerObj, zfnull, valueOld, paramList);
    return zftrue;
}

// ============================================================
const ZFProperty *ZFPropertyDynamicRegister(ZF_IN const ZFPropertyDynamicRegisterParam &param,
                                            ZF_OUT_OPT zfstring *errorHint /* = zfnull */)
{
    if(param.propertyOwnerClass() == zfnull)
    {
        zfstringAppend(errorHint, zfText("propertyOwnerClass not set"));
        return zfnull;
    }
    if(param.propertyTypeId() == zfnull)
    {
        zfstringAppend(errorHint, zfText("propertyTypeId not set"));
        return zfnull;
    }
    if(param.propertyName() == zfnull)
    {
        zfstringAppend(errorHint, zfText("propertyName not set"));
        return zfnull;
    }
    if(param.propertyClassOfRetainProperty() == zfnull)
    {
        if(ZFClass::classForName(param.propertyTypeId()) != zfnull)
        {
            zfstringAppend(errorHint,
                zfText("propertyTypeId %s is ZFObject type but propertyClassOfRetainProperty not set, weak property not supported"),
                param.propertyTypeId());
            return zfnull;
        }
    }
    else
    {
        if(!zfscmpTheSame(param.propertyTypeId(), param.propertyClassOfRetainProperty()->className()))
        {
            zfstringAppend(errorHint,
                zfText("propertyTypeId must be same as propertyClassOfRetainProperty for retain property"));
            return zfnull;
        }
    }

    const ZFTypeIdBase *d = ZFTypeIdGet(param.propertyTypeId());
    if(d == zfnull)
    {
        zfstringAppend(errorHint,
            zfText("propertyTypeId %s not registered"));
        return zfnull;
    }

    const ZFProperty *existProperty = param.propertyOwnerClass()->propertyForName(param.propertyName());
    if(existProperty != zfnull)
    {
        zfstringAppend(errorHint,
            zfText("property with same name already exists: %s"),
            existProperty->objectInfo().cString());
        return zfnull;
    }

    zfblockedAlloc(_ZFP_I_PropDynRegData, userDataWrapper);
    userDataWrapper->d = d;
    userDataWrapper->initValueCallback = param.propertyInitValueCallback();
    zfstringAppend(userDataWrapper->tagKey, zfText("PropDyn_%s"), param.propertyName());
    zfstringAppend(userDataWrapper->valueStoreKey, zfText("PropDynV_%s"), param.propertyName());

    zfstring errorHintTmp;
    const ZFMethod *setterMethod = ZFMethodDynamicRegister(ZFMethodDynamicRegisterParam()
            .methodDynamicRegisterUserDataSet(userDataWrapper)
            .methodOwnerClassSet(param.propertyOwnerClass())
            .methodGenericInvokerSet(_ZFP_PropDynReg_setterGI)
            .methodTypeSet(ZFMethodTypeVirtual)
            .methodPrivilegeTypeSet(param.propertySetterType())
            .methodNameSet(zfstringWithFormat(zfText("%sSet"), param.propertyName()))
            .methodReturnTypeIdSet(ZFTypeId_void())
            .methodReturnTypeNameSet(zfText("void"))
            .methodParamAdd(
                param.propertyTypeId(),
                zfstringWithFormat(zfText("%s const &"), param.propertyTypeName()),
                zfnull)
        , &errorHintTmp);
    if(setterMethod == zfnull)
    {
        zfstringAppend(errorHint,
            zfText("failed to register setter method, reason: %s"),
            errorHintTmp.cString());
        return zfnull;
    }
    const ZFMethod *getterMethod = ZFMethodDynamicRegister(ZFMethodDynamicRegisterParam()
            .methodDynamicRegisterUserDataSet(userDataWrapper)
            .methodOwnerClassSet(param.propertyOwnerClass())
            .methodGenericInvokerSet(_ZFP_PropDynReg_getterGI)
            .methodTypeSet(ZFMethodTypeVirtual)
            .methodPrivilegeTypeSet(param.propertyGetterType())
            .methodNameSet(param.propertyName())
            .methodReturnTypeIdSet(param.propertyTypeId())
            .methodReturnTypeNameSet(zfstringWithFormat(zfText("%s const &"), param.propertyTypeName()))
        , &errorHintTmp);
    if(getterMethod == zfnull)
    {
        zfstringAppend(errorHint,
            zfText("failed to register getter method, reason: %s"),
            errorHintTmp.cString());
        ZFMethodDynamicUnregister(setterMethod);
        return zfnull;
    }

    const ZFProperty *property = _ZFP_ZFPropertyRegister(zffalse
        , zftrue
        , param.propertyDynamicRegisterUserData()
        , param.propertyOwnerClass()
        , param.propertyName()
        , param.propertyTypeName()
        , param.propertyTypeId()
        , setterMethod
        , getterMethod
        , param.propertyClassOfRetainProperty()
        , _ZFP_PropDynReg_callbackIsValueAccessed
        , _ZFP_PropDynReg_callbackIsInitValue
        , _ZFP_PropDynReg_callbackValueSet
        , _ZFP_PropDynReg_callbackValueGet
        , _ZFP_PropDynReg_callbackValueReset
        , _ZFP_PropDynReg_callbackCompare
        , _ZFP_PropDynReg_callbackGetInfo
        , _ZFP_PropDynReg_callbackValueStore
        , _ZFP_PropDynReg_callbackValueRelease
        , _ZFP_propCbDSerializeFrom_generic
        , _ZFP_propCbDSerializeTo_generic
        , _ZFP_PropDynReg_callbackProgressUpdate
        , zfnull
        , zfnull
        );
    ZF_GLOBAL_INITIALIZER_INSTANCE(ZFPropertyDynamicRegisterAutoRemove)->m[property] = zftrue;
    property->_ZFP_ZFProperty_removeConst()->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper = zfRetain(userDataWrapper);
    return property;
}
void ZFPropertyDynamicUnregister(ZF_IN const ZFProperty *property)
{
    if(property != zfnull)
    {
        zfCoreAssert(property->propertyIsDynamicRegister());
        zfCoreMutexLocker();
        zfblockedRelease(property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
        ZF_GLOBAL_INITIALIZER_INSTANCE(ZFPropertyDynamicRegisterAutoRemove)->m.erase(property);
        _ZFP_I_PropDynRegData *d = ZFCastZFObject(_ZFP_I_PropDynRegData *, property->_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper);
        d->objectDetach();
        _ZFP_ZFPropertyUnregister(property);
    }
}

ZF_NAMESPACE_GLOBAL_END

