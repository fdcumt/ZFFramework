/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
#include "ZFProperty.h"
#include "ZFObjectImpl.h"

#include "ZFPropertyDeclare.h"
#include "ZFPropertyUserRegister.h"
#include "ZFSerializableUtil.h"

#include "../ZFSTLWrapper/zfstl_map.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
void ZFProperty::objectInfoT(ZF_IN_OUT zfstring &ret) const
{
    ret += '(';
    if(zfslen(this->propertyTypeId()) > 0)
    {
        ret += this->propertyTypeId();
    }
    else
    {
        ret += this->propertyTypeName();
    }
    ret += ')';
    ret += this->propertyOwnerClass()->className();
    ret += zfText("::");
    ret += this->propertyName();
}

zfbool ZFProperty::propertySerializable(void) const
{
    if(zfscmpTheSame(this->propertyTypeId(), ZFTypeId_none()))
    {
        return zffalse;
    }
    const ZFTypeIdBase *t = ZFTypeIdGet(this->propertyTypeId());
    if(t == zfnull || !t->typeIdSerializable())
    {
        return zffalse;
    }
    return zftrue;
}

/** @cond ZFPrivateDoc */
ZFProperty::ZFProperty(void)
: callbackIsValueAccessed(zfnull)
, callbackIsInitValue(zfnull)
, callbackValueSet(zfnull)
, callbackValueGet(zfnull)
, callbackValueReset(zfnull)
, callbackCompare(zfnull)
, callbackGetInfo(zfnull)
, callbackValueStore(zfnull)
, callbackValueRelease(zfnull)
, callbackSerializeFrom(zfnull)
, callbackSerializeTo(zfnull)
, callbackProgressUpdate(zfnull)
, callbackUserRegisterInitValueSetup(zfnull)
, _ZFP_ZFPropertyNeedInit(zftrue)
, _ZFP_ZFProperty_propertyIsUserRegister(zffalse)
, _ZFP_ZFProperty_propertyIsDynamicRegister(zffalse)
, _ZFP_ZFProperty_propertyDynamicRegisterUserData(zfnull)
, _ZFP_ZFProperty_propertyOwnerClass(zfnull)
, _ZFP_ZFProperty_name()
, _ZFP_ZFProperty_typeName()
, _ZFP_ZFProperty_typeId()
, _ZFP_ZFProperty_setterMethod(zfnull)
, _ZFP_ZFProperty_getterMethod(zfnull)
, _ZFP_ZFProperty_propertyClassOfRetainProperty(zfnull)
, _ZFP_ZFProperty_callbackDealloc(zfnull)
, _ZFP_ZFPropertyLifeCycle_OnInit()
, _ZFP_ZFPropertyLifeCycle_OnDealloc()
, _ZFP_ZFPropertyLifeCycle_OnVerify()
, _ZFP_ZFPropertyLifeCycle_OnAttach()
, _ZFP_ZFPropertyLifeCycle_OnDetach()
, _ZFP_ZFPropertyLifeCycle_OnUpdate()
{
}
ZFProperty::~ZFProperty(void)
{
}
/** @endcond */
void ZFProperty::_ZFP_ZFPropertyInit(ZF_IN zfbool propertyIsUserRegister,
                                     ZF_IN zfbool propertyIsDynamicRegister,
                                     ZF_IN ZFObject *propertyDynamicRegisterUserData,
                                     ZF_IN const ZFClass *propertyOwnerClass,
                                     ZF_IN const zfchar *name,
                                     ZF_IN const zfchar *typeName,
                                     ZF_IN const zfchar *typeIdName,
                                     ZF_IN const ZFMethod *setterMethod,
                                     ZF_IN const ZFMethod *getterMethod,
                                     ZF_IN const ZFClass *propertyClassOfRetainProperty)
{
    this->_ZFP_ZFProperty_propertyIsUserRegister = propertyIsUserRegister;
    this->_ZFP_ZFProperty_propertyIsDynamicRegister = propertyIsDynamicRegister;
    this->_ZFP_ZFProperty_propertyDynamicRegisterUserData = zfRetain(propertyDynamicRegisterUserData);
    this->_ZFP_ZFProperty_propertyOwnerClass = propertyOwnerClass;
    this->_ZFP_ZFProperty_name = name;
    this->_ZFP_ZFProperty_typeName = typeName;
    if(propertyClassOfRetainProperty == zfnull && ZFClass::classForName(typeIdName) != zfnull)
    { // assign property with ZFObject type, is not serializable
        this->_ZFP_ZFProperty_typeId = ZFTypeId_none();
    }
    else
    {
        this->_ZFP_ZFProperty_typeId = typeIdName;
    }
    this->_ZFP_ZFProperty_setterMethod = setterMethod;
    this->_ZFP_ZFProperty_getterMethod = getterMethod;
    setterMethod->_ZFP_ZFMethod_removeConst()->_ZFP_ZFMethod_methodOwnerProperty = this;
    getterMethod->_ZFP_ZFMethod_removeConst()->_ZFP_ZFMethod_methodOwnerProperty = this;
    this->_ZFP_ZFProperty_propertyClassOfRetainProperty = propertyClassOfRetainProperty;
}

// ============================================================
zfclassLikePOD _ZFP_ZFPropertyMapData
{
public:
    zfuint refCount;
    ZFProperty propertyInfo;
public:
    _ZFP_ZFPropertyMapData(void)
    : refCount(1)
    , propertyInfo()
    {
    }
};
ZF_STATIC_INITIALIZER_INIT(ZFPropertyDataHolder)
{
}
ZFCoreMap propertyMap; // _ZFP_ZFPropertyMapData *
ZF_STATIC_INITIALIZER_END(ZFPropertyDataHolder)
#define _ZFP_ZFPropertyMap (ZF_STATIC_INITIALIZER_INSTANCE(ZFPropertyDataHolder)->propertyMap)

// ============================================================
void ZFPropertyGetAllT(ZF_OUT ZFCoreArray<const ZFProperty *> &ret,
                       ZF_IN_OPT const ZFFilterForZFProperty *propertyFilter /* = zfnull */)
{
    zfCoreMutexLocker();
    const ZFCoreMap &m = _ZFP_ZFPropertyMap;
    if(propertyFilter != zfnull)
    {
        for(zfiterator it = m.iterator(); m.iteratorIsValid(it); )
        {
            _ZFP_ZFPropertyMapData *v = m.iteratorNextValue<_ZFP_ZFPropertyMapData *>(it);
            if(propertyFilter->filterCheckActive(&(v->propertyInfo)))
            {
                ret.add(&(v->propertyInfo));
            }
        }
    }
    else
    {
        for(zfiterator it = m.iterator(); m.iteratorIsValid(it); )
        {
            _ZFP_ZFPropertyMapData *v = m.iteratorNextValue<_ZFP_ZFPropertyMapData *>(it);
            ret.add(&(v->propertyInfo));
        }
    }
}

// ============================================================
static void _ZFP_ZFPropertyInstanceSig(ZF_OUT zfstring &ret,
                                       ZF_IN const zfchar *className,
                                       ZF_IN const zfchar *propertyName)
{
    if(className)
    {
        ret += className;
    }
    ret += ':';
    if(propertyName)
    {
        ret += propertyName;
    }
}
static ZFProperty *_ZFP_ZFPropertyInstanceFind(ZF_IN const zfchar *propertyInternalId)
{
    zfCoreMutexLocker();
    _ZFP_ZFPropertyMapData *v = _ZFP_ZFPropertyMap.get<_ZFP_ZFPropertyMapData *>(propertyInternalId);
    if(v == zfnull)
    {
        return zfnull;
    }
    else
    {
        return &(v->propertyInfo);
    }
}
static ZFProperty *_ZFP_ZFPropertyInstanceAccess(ZF_IN const zfchar *propertyInternalId)
{
    zfCoreMutexLocker();
    _ZFP_ZFPropertyMapData *v = _ZFP_ZFPropertyMap.get<_ZFP_ZFPropertyMapData *>(propertyInternalId);
    if(v == zfnull)
    {
        v = zfnew(_ZFP_ZFPropertyMapData);
        _ZFP_ZFPropertyMap.set(propertyInternalId, ZFCorePointerForObject<_ZFP_ZFPropertyMapData *>(v));
        v->propertyInfo._ZFP_ZFProperty_propertyInternalId = propertyInternalId;
    }
    else
    {
        ++(v->refCount);
    }
    return &(v->propertyInfo);
}
static zfbool _ZFP_ZFPropertyInstanceCleanup(ZF_IN const ZFProperty *property)
{
    zfCoreMutexLocker();
    _ZFP_ZFPropertyMapData *v = _ZFP_ZFPropertyMap.get<_ZFP_ZFPropertyMapData *>(property->propertyInternalId());
    if(v == zfnull)
    {
        return zffalse;
    }
    --(v->refCount);
    if(v->refCount == 0)
    {
        _ZFP_ZFPropertyMap.remove(property->propertyInternalId());
    }
    return zftrue;
}

ZFProperty *_ZFP_ZFPropertyRegister(ZF_IN zfbool propertyIsUserRegister
                                    , ZF_IN zfbool propertyIsDynamicRegister
                                    , ZF_IN ZFObject *propertyDynamicRegisterUserData
                                    , ZF_IN const ZFClass *propertyOwnerClass
                                    , ZF_IN const zfchar *name
                                    , ZF_IN const zfchar *typeName
                                    , ZF_IN const zfchar *typeIdName
                                    , ZF_IN const ZFMethod *setterMethod
                                    , ZF_IN const ZFMethod *getterMethod
                                    , ZF_IN const ZFClass *propertyClassOfRetainProperty
                                    , ZF_IN ZFPropertyCallbackIsValueAccessed callbackIsValueAccessed
                                    , ZF_IN ZFPropertyCallbackIsInitValue callbackIsInitValue
                                    , ZF_IN ZFPropertyCallbackValueSet callbackValueSet
                                    , ZF_IN ZFPropertyCallbackValueGet callbackValueGet
                                    , ZF_IN ZFPropertyCallbackValueReset callbackValueReset
                                    , ZF_IN ZFPropertyCallbackCompare callbackCompare
                                    , ZF_IN ZFPropertyCallbackGetInfo callbackGetInfo
                                    , ZF_IN ZFPropertyCallbackValueStore callbackValueStore
                                    , ZF_IN ZFPropertyCallbackValueRelease callbackValueRelease
                                    , ZF_IN ZFPropertyCallbackSerializeFrom callbackSerializeFrom
                                    , ZF_IN ZFPropertyCallbackSerializeTo callbackSerializeTo
                                    , ZF_IN ZFPropertyCallbackProgressUpdate callbackProgressUpdate
                                    , ZF_IN ZFPropertyCallbackUserRegisterInitValueSetup callbackUserRegisterInitValueSetup
                                    , ZF_IN _ZFP_ZFPropertyCallbackDealloc callbackDealloc
                                    )
{
    zfCoreMutexLocker();
    ZFProperty *propertyInfo = zfnull;

    zfCoreAssert(propertyOwnerClass != zfnull);
    zfCoreAssert(name != zfnull && *name != '\0');
    zfCoreAssert(typeName != zfnull && *typeName != '\0');
    zfCoreAssert(typeIdName != zfnull && *typeIdName != '\0');
    zfCoreAssert(setterMethod != zfnull);
    zfCoreAssert(getterMethod != zfnull);
    zfCoreAssert(callbackIsValueAccessed != zfnull);
    zfCoreAssert(callbackIsInitValue != zfnull);
    zfCoreAssert(callbackValueSet != zfnull);
    zfCoreAssert(callbackValueGet != zfnull);
    zfCoreAssert(callbackValueReset != zfnull);
    zfCoreAssert(callbackCompare != zfnull);
    zfCoreAssert(callbackGetInfo != zfnull);
    zfCoreAssert(callbackValueStore != zfnull);
    zfCoreAssert(callbackValueRelease != zfnull);
    zfCoreAssert(callbackSerializeFrom != zfnull);
    zfCoreAssert(callbackSerializeTo != zfnull);
    zfCoreAssert(callbackProgressUpdate != zfnull);

    zfstring propertyInternalId;
    _ZFP_ZFPropertyInstanceSig(propertyInternalId, propertyOwnerClass->className(), name);

    if(propertyIsUserRegister)
    {
        propertyInfo = _ZFP_ZFPropertyInstanceFind(propertyInternalId);
        zfCoreAssertWithMessageTrim(propertyInfo == zfnull,
            zfTextA("[ZFPropertyUserRegister] registering a property that already registered, class: %s, propertyName: %s"),
            zfsCoreZ2A(propertyOwnerClass->className()),
            zfsCoreZ2A(name));
    }
    else if(propertyIsDynamicRegister)
    {
        propertyInfo = _ZFP_ZFPropertyInstanceFind(propertyInternalId);
        zfCoreAssertWithMessageTrim(propertyInfo == zfnull,
            zfTextA("[ZFPropertyDynamicRegister] registering a property that already registered, class: %s, propertyName: %s"),
            zfsCoreZ2A(propertyOwnerClass->className()),
            zfsCoreZ2A(name));
    }
    propertyInfo = _ZFP_ZFPropertyInstanceAccess(propertyInternalId);

    if(propertyInfo->_ZFP_ZFPropertyNeedInit)
    {
        propertyInfo->_ZFP_ZFPropertyNeedInit = zffalse;
        propertyInfo->_ZFP_ZFPropertyInit(
            propertyIsUserRegister,
            propertyIsDynamicRegister,
            propertyDynamicRegisterUserData,
            propertyOwnerClass,
            name,
            typeName,
            typeIdName,
            setterMethod,
            getterMethod,
            propertyClassOfRetainProperty);
        propertyInfo->callbackIsValueAccessed = callbackIsValueAccessed;
        propertyInfo->callbackIsInitValue = callbackIsInitValue;
        propertyInfo->callbackValueSet = callbackValueSet;
        propertyInfo->callbackValueGet = callbackValueGet;
        propertyInfo->callbackValueReset = callbackValueReset;
        propertyInfo->callbackCompare = callbackCompare;
        propertyInfo->callbackGetInfo = callbackGetInfo;
        propertyInfo->callbackValueStore = callbackValueStore;
        propertyInfo->callbackValueRelease = callbackValueRelease;
        propertyInfo->callbackSerializeFrom = callbackSerializeFrom;
        propertyInfo->callbackSerializeTo = callbackSerializeTo;
        propertyInfo->callbackProgressUpdate = callbackProgressUpdate;
        propertyInfo->callbackUserRegisterInitValueSetup = callbackUserRegisterInitValueSetup;
        propertyInfo->_ZFP_ZFProperty_callbackDealloc = callbackDealloc;

        propertyOwnerClass->_ZFP_ZFClass_propertyRegister(propertyInfo);
        _ZFP_ZFClassDataChangeNotify(ZFClassDataChangeTypeAttach, zfnull, propertyInfo, zfnull);
    }

    return propertyInfo;
}
void _ZFP_ZFPropertyUnregister(ZF_IN const ZFProperty *propertyInfo)
{
    zfCoreMutexLocker();
    _ZFP_ZFClassDataChangeNotify(ZFClassDataChangeTypeDetach, zfnull, propertyInfo, zfnull);

    propertyInfo->setterMethod()->_ZFP_ZFMethod_removeConst()->_ZFP_ZFMethod_methodOwnerProperty = zfnull;
    propertyInfo->getterMethod()->_ZFP_ZFMethod_removeConst()->_ZFP_ZFMethod_methodOwnerProperty = zfnull;
    if(propertyInfo->propertyIsUserRegister())
    {
        propertyInfo->propertyOwnerClass()->_ZFP_ZFClass_propertyUnregister(propertyInfo);
        ZFMethodUserUnregister(propertyInfo->setterMethod());
        ZFMethodUserUnregister(propertyInfo->getterMethod());
    }
    else if(propertyInfo->propertyIsDynamicRegister())
    {
        propertyInfo->propertyOwnerClass()->_ZFP_ZFClass_propertyUnregister(propertyInfo);
        ZFMethodDynamicUnregister(propertyInfo->setterMethod());
        ZFMethodDynamicUnregister(propertyInfo->getterMethod());
        zfRetainChange(propertyInfo->_ZFP_ZFProperty_removeConst()->_ZFP_ZFProperty_propertyDynamicRegisterUserData, zfnull);
    }

    _ZFP_ZFPropertyInstanceCleanup(propertyInfo);
}

// ============================================================
zfclass _ZFP_I_ZFPropertyValueStoreHolder : zfextends ZFObject
{
    ZFOBJECT_DECLARE(_ZFP_I_ZFPropertyValueStoreHolder, ZFObject)

public:
    zfstlmap<const ZFProperty *, zfstlmap<void *, ZFCorePointerForObject<ZFCorePointerBase *> > > d;
};
void _ZFP_ZFPropertyValueStoreImpl(ZF_IN const ZFProperty *property,
                                   ZF_IN ZFObject *ownerObj,
                                   ZF_IN void *valueStored,
                                   ZF_IN ZFCorePointerBase *valueHolder)
{
    zfCoreMutexLocker();
    _ZFP_I_ZFPropertyValueStoreHolder *d = ownerObj->tagGet<_ZFP_I_ZFPropertyValueStoreHolder *>(
            _ZFP_I_ZFPropertyValueStoreHolder::ClassData()->className());
    if(d == zfnull)
    {
        d = zfAlloc(_ZFP_I_ZFPropertyValueStoreHolder);
        ownerObj->tagSet(_ZFP_I_ZFPropertyValueStoreHolder::ClassData()->className(), d);
        zfRelease(d);
    }
    d->d[property][valueStored] = ZFCorePointerForObject<ZFCorePointerBase *>(valueHolder);
}
void _ZFP_ZFPropertyValueReleaseImpl(ZF_IN const ZFProperty *property,
                                     ZF_IN ZFObject *ownerObj,
                                     ZF_IN void *valueStored)
{
    zfCoreMutexLocker();
    _ZFP_I_ZFPropertyValueStoreHolder *d = ownerObj->tagGet<_ZFP_I_ZFPropertyValueStoreHolder *>(
            _ZFP_I_ZFPropertyValueStoreHolder::ClassData()->className());
    if(d == zfnull)
    {
        return ;
    }
    zfstlmap<const ZFProperty *, zfstlmap<void *, ZFCorePointerForObject<ZFCorePointerBase *> > >::iterator it =
        d->d.find(property);
    if(it == d->d.end())
    {
        return ;
    }
    it->second.erase(valueStored);
    if(it->second.empty())
    {
        d->d.erase(it);
    }
}

// ============================================================
zfbool _ZFP_propCbDSerializeFrom_generic(ZF_IN const ZFProperty *propertyInfo,
                                         ZF_IN ZFObject *ownerObject,
                                         ZF_IN const ZFSerializableData &serializableData,
                                         ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */,
                                         ZF_OUT_OPT ZFSerializableData *outErrorPos /* = zfnull */)
{
    zfautoObject zfv;
    if(!_ZFP_propCbDSerializeFrom_impl(zfv, propertyInfo, serializableData, outErrorHint, outErrorPos))
    {
        return zffalse;
    }
    propertyInfo->callbackValueSet(propertyInfo, ownerObject, zfv.to<ZFTypeIdWrapper *>()->wrappedValue());
    return zftrue;
}
zfbool _ZFP_propCbDSerializeFrom_impl(ZF_OUT zfautoObject &zfv,
                                      ZF_IN const ZFProperty *propertyInfo,
                                      ZF_IN const ZFSerializableData &serializableData,
                                      ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */,
                                      ZF_OUT_OPT ZFSerializableData *outErrorPos /* = zfnull */)
{
    const ZFTypeIdBase *t = ZFTypeIdGet(propertyInfo->propertyTypeId());
    if(t == zfnull)
    {
        ZFSerializableUtil::errorOccurred(outErrorHint, outErrorPos, serializableData,
            zfText("no such type id: %s"),
            propertyInfo->propertyTypeId());
        return zffalse;
    }
    if(t->typeIdWrapper(zfv))
    {
        if(zfv == zfnull)
        {
            return ZFObjectFromData(zfv, serializableData, outErrorHint, outErrorPos);
        }
        ZFTypeIdWrapper *w = zfv;
        if(w != zfnull)
        {
            return w->wrappedValueFromData(serializableData, outErrorHint, outErrorPos);
        }
    }
    ZFSerializableUtil::errorOccurred(outErrorHint, outErrorPos, serializableData,
        zfText("unable to access type id wrapper: %s"),
        propertyInfo->propertyTypeId());
    return zffalse;
}
void _ZFP_propCbDSerializeFrom_errorOccurred(ZF_IN const ZFSerializableData &serializableData,
                                             ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */,
                                             ZF_OUT_OPT ZFSerializableData *outErrorPos /* = zfnull */)
{
    ZFSerializableUtil::errorOccurred(outErrorHint, outErrorPos, serializableData,
        zfText("unable to access type id wrapper value"));
}
zfbool _ZFP_propCbDSerializeTo_generic(ZF_IN const ZFProperty *propertyInfo,
                                       ZF_IN ZFObject *ownerObject,
                                       ZF_OUT ZFSerializableData &serializableData,
                                       ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */)
{
    const ZFTypeIdBase *t = ZFTypeIdGet(propertyInfo->propertyTypeId());
    if(t == zfnull)
    {
        ZFSerializableUtil::errorOccurred(outErrorHint,
            zfText("no such type id: %s"),
            propertyInfo->propertyTypeId());
        return zffalse;
    }
    zfautoObject zfv;
    if(t->typeIdWrapper(zfv))
    {
        const void *v = propertyInfo->callbackValueGet(propertyInfo, ownerObject);
        if(zfv == zfnull)
        {
            if(propertyInfo->propertyIsRetainProperty()
                || zfscmpTheSame(ZFTypeId_zfautoObject(), propertyInfo->propertyTypeId()))
            {
                return ZFObjectToData(serializableData, *(const zfautoObject *)v, outErrorHint);
            }
        }
        else
        {
            ZFTypeIdWrapper *w = zfv;
            if(w != zfnull)
            {
                w->wrappedValueSet(v);
                return w->wrappedValueToData(serializableData, outErrorHint);
            }
        }
    }
    ZFSerializableUtil::errorOccurred(outErrorHint,
        zfText("unable to access type id wrapper: %s"),
        propertyInfo->propertyTypeId());
    return zffalse;
}
zfbool _ZFP_propCbDSerializeTo_impl(ZF_IN const ZFProperty *propertyInfo,
                                    ZF_IN ZFObject *zfv,
                                    ZF_OUT ZFSerializableData &serializableData,
                                    ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */)
{
    const ZFTypeIdBase *t = ZFTypeIdGet(propertyInfo->propertyTypeId());
    if(t == zfnull)
    {
        ZFSerializableUtil::errorOccurred(outErrorHint,
            zfText("no such type id: %s"),
            propertyInfo->propertyTypeId());
        return zffalse;
    }
    ZFTypeIdWrapper *w = ZFCastZFObject(ZFTypeIdWrapper *, zfv);
    if(w == zfnull)
    {
        ZFSerializableUtil::errorOccurred(outErrorHint,
            zfText("unable to access type id wrapper: %s"),
            propertyInfo->propertyTypeId());
        return zffalse;
    }
    return w->wrappedValueToData(serializableData, outErrorHint);
}
void _ZFP_propCbDSerializeTo_errorOccurred(ZF_IN const ZFSerializableData &serializableData,
                                           ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */)
{
    ZFSerializableUtil::errorOccurred(outErrorHint,
        zfText("unable to access type id wrapper value"));
}

ZF_NAMESPACE_GLOBAL_END

#if _ZFP_ZFOBJECT_METHOD_REG
#include "../ZFObject.h"
ZF_NAMESPACE_GLOBAL_BEGIN

ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const zfchar *, propertyInternalId)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, zfbool, propertyIsUserRegister)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, zfbool, propertyIsDynamicRegister)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, ZFObject *, propertyDynamicRegisterUserData)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const ZFClass *, propertyOwnerClass)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const zfchar *, propertyName)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const zfchar *, propertyTypeName)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const zfchar *, propertyTypeId)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const ZFMethod *, setterMethod)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const ZFMethod *, getterMethod)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, zfbool, propertyIsRetainProperty)
ZFMETHOD_USER_REGISTER_FOR_WRAPPER_FUNC_0(v_ZFProperty, const ZFClass *, propertyClassOfRetainProperty)

ZFMETHOD_FUNC_USER_REGISTER_FOR_FUNC_2(void, ZFPropertyGetAllT, ZFMP_OUT(ZFCoreArray<const ZFProperty *> &, ret), ZFMP_IN_OPT(const ZFFilterForZFProperty *, propertyFilter, zfnull))
ZFMETHOD_FUNC_USER_REGISTER_FOR_FUNC_1(ZFCoreArrayPOD<const ZFProperty *>, ZFPropertyGetAll, ZFMP_IN_OPT(const ZFFilterForZFProperty *, propertyFilter, zfnull))

ZF_NAMESPACE_GLOBAL_END
#endif

