/* ====================================================================== *
 * Copyright (c) 2010-2018 ZFFramework
 * Github repo: https://github.com/ZFFramework/ZFFramework
 * Home page: http://ZFFramework.com
 * Blog: http://zsaber.com
 * Contact: master@zsaber.com (Chinese and English only)
 * Distributed under MIT license:
 *   https://github.com/ZFFramework/ZFFramework/blob/master/LICENSE
 * ====================================================================== */
/**
 * @file ZFProperty.h
 * @brief property for ZFObject type
 */

#ifndef _ZFI_ZFProperty_h_
#define _ZFI_ZFProperty_h_

#include "ZFPropertyFwd.h"
#include "ZFTypeIdFwd.h"
#include "ZFMethod.h"
#include "ZFAny.h"
ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
zfclassFwd ZFProperty;
typedef void (*_ZFP_ZFPropertyCallbackDealloc)(ZF_IN const ZFProperty *property, ZF_IN ZFObject *owner);
/**
 * @brief info for a property for ZFObject, see #ZFPROPERTY_RETAIN for more info
 */
zffinal zfclassNotPOD ZF_ENV_EXPORT ZFProperty
{
    ZFCLASS_DISALLOW_COPY_CONSTRUCTOR(ZFProperty)

public:
    /** @brief see #objectInfo */
    void objectInfoT(ZF_IN_OUT zfstring &ret) const;
    /**
     * @brief get info about this property
     */
    zfstring objectInfo(void) const
    {
        zfstring ret;
        this->objectInfoT(ret);
        return ret;
    }

public:
    /**
     * @brief internal property id, for debug use only
     */
    inline const zfchar *propertyInternalId(void) const
    {
        return this->_ZFP_ZFProperty_propertyInternalId;
    }
    /**
     * @brief true if this property is registered by #ZFPropertyUserRegisterRetain
     */
    inline zfbool propertyIsUserRegister(void) const
    {
        return this->_ZFP_ZFProperty_propertyIsUserRegister;
    }
    /**
     * @brief true if this property is registered by #ZFPropertyDynamicRegister
     */
    inline zfbool propertyIsDynamicRegister(void) const
    {
        return this->_ZFP_ZFProperty_propertyIsDynamicRegister;
    }
    /**
     * @brief see #ZFPropertyDynamicRegister
     */
    inline ZFObject *propertyDynamicRegisterUserData(void) const
    {
        return this->_ZFP_ZFProperty_propertyDynamicRegisterUserData;
    }
    /**
     * @brief true if the property is serializable
     *
     * property are serializable except:
     * -  #propertyTypeId is #ZFTypeId_none
     * -  assign property with ZFObject type
     * -  #ZFTypeIdBase::typeIdSerializable returned false
     *
     * @note this property would be calculated at runtime,
     *   take care of performance
     */
    zfbool propertySerializable(void) const;
    /**
     * @brief get the property's owner class
     */
    inline const ZFClass *propertyOwnerClass(void) const
    {
        return this->_ZFP_ZFProperty_propertyOwnerClass;
    }
    /**
     * @brief name for the property
     *
     * assume property's name is "myProperty",
     * then the setter would be "myPropertySet",
     * and the getter would be "myProperty",
     * and getter name would return "myProperty"
     */
    inline const zfchar *propertyName(void) const
    {
        return this->_ZFP_ZFProperty_name.cString();
    }

    /**
     * @brief type string for the property
     *
     * note the type string is the macro expand from the type,
     * it may or may not be same for same type,
     * usually for debug use only
     */
    inline const zfchar *propertyTypeName(void) const
    {
        return this->_ZFP_ZFProperty_typeName.cString();
    }
    /**
     * @brief type id string declared in ZFPROPERTY_XXX
     *
     * this value should be ensured the type id for the type or #ZFTypeId_none if no known type,
     * this value is used for property's advanced serialize and copy logic,
     * see #ZFTypeIdBase
     * @note for retain property, this value is always the class name of the #propertyClassOfRetainProperty
     */
    inline const zfchar *propertyTypeId(void) const
    {
        return this->_ZFP_ZFProperty_typeId.cString();
    }
    /**
     * @brief get the getter method
     */
    inline const ZFMethod *setterMethod(void) const
    {
        return this->_ZFP_ZFProperty_setterMethod;
    }
    /**
     * @brief get the getter method
     */
    inline const ZFMethod *getterMethod(void) const
    {
        return this->_ZFP_ZFProperty_getterMethod;
    }

    /**
     * @brief true if the property is retain property
     *
     * note that bool property is also a assign property,
     * it also have a getter method named "propertyName"
     */
    inline zfbool propertyIsRetainProperty(void) const
    {
        return (this->_ZFP_ZFProperty_propertyClassOfRetainProperty != zfnull);
    }
    /**
     * @brief for retain property only, get the retain property's declared class
     *
     * note it's the declared class, property's value may be subclass of it
     */
    inline const ZFClass *propertyClassOfRetainProperty(void) const
    {
        return this->_ZFP_ZFProperty_propertyClassOfRetainProperty;
    }

public:
    /** @brief see #ZFPropertyCallbackIsValueAccessed */
    ZFPropertyCallbackIsValueAccessed callbackIsValueAccessed;
    /** @brief see #ZFPropertyCallbackIsInitValue */
    ZFPropertyCallbackIsInitValue callbackIsInitValue;
    /** @brief see #ZFPropertyCallbackValueSet */
    ZFPropertyCallbackValueSet callbackValueSet;
    /** @brief see #ZFPropertyCallbackValueGet */
    ZFPropertyCallbackValueGet callbackValueGet;
    /** @brief see #ZFPropertyCallbackValueReset */
    ZFPropertyCallbackValueReset callbackValueReset;
    /** @brief see #ZFPropertyCallbackCompare */
    ZFPropertyCallbackCompare callbackCompare;
    /** @brief see #ZFPropertyCallbackGetInfo */
    ZFPropertyCallbackGetInfo callbackGetInfo;
    /** @brief see #ZFPropertyCallbackValueStore */
    ZFPropertyCallbackValueStore callbackValueStore;
    /** @brief see #ZFPropertyCallbackValueRelease */
    ZFPropertyCallbackValueRelease callbackValueRelease;
    /** @brief see #ZFPropertyCallbackSerializeFrom */
    ZFPropertyCallbackSerializeFrom callbackSerializeFrom;
    /** @brief see #ZFPropertyCallbackSerializeTo */
    ZFPropertyCallbackSerializeTo callbackSerializeTo;
    /** @brief see #ZFPropertyCallbackProgressUpdate */
    ZFPropertyCallbackProgressUpdate callbackProgressUpdate;

    /**
     * @brief see #ZFPropertyUserRegisterAssign
     */
    ZFPropertyCallbackUserRegisterInitValueSetup callbackUserRegisterInitValueSetup;

public:
    /** @cond ZFPrivateDoc */
    ZFProperty(void);
    ~ZFProperty(void);
    /** @endcond */
    zfbool _ZFP_ZFPropertyNeedInit;
    void _ZFP_ZFPropertyInit(ZF_IN zfbool propertyIsUserRegister,
                             ZF_IN zfbool propertyIsDynamicRegister,
                             ZF_IN ZFObject *propertyDynamicRegisterUserData,
                             ZF_IN const ZFClass *propertyOwnerClass,
                             ZF_IN const zfchar *name,
                             ZF_IN const zfchar *typeName,
                             ZF_IN const zfchar *typeIdName,
                             ZF_IN const ZFMethod *setterMethod,
                             ZF_IN const ZFMethod *getterMethod,
                             ZF_IN const ZFClass *propertyClassOfRetainProperty);
    ZFProperty *_ZFP_ZFProperty_removeConst(void) const
    {
        return const_cast<ZFProperty *>(this);
    }
public:
    zfstring _ZFP_ZFProperty_propertyInternalId;
    zfbool _ZFP_ZFProperty_propertyIsUserRegister;
    zfbool _ZFP_ZFProperty_propertyIsDynamicRegister;
    ZFObject *_ZFP_ZFProperty_propertyDynamicRegisterUserData;
    ZFObject *_ZFP_ZFProperty_propertyDynamicRegisterUserDataWrapper;
    const ZFClass *_ZFP_ZFProperty_propertyOwnerClass;
    zfstring _ZFP_ZFProperty_name;
    zfstring _ZFP_ZFProperty_typeName;
    zfstring _ZFP_ZFProperty_typeId;
    const ZFMethod *_ZFP_ZFProperty_setterMethod;
    const ZFMethod *_ZFP_ZFProperty_getterMethod;
    const ZFClass *_ZFP_ZFProperty_propertyClassOfRetainProperty;
    _ZFP_ZFPropertyCallbackDealloc _ZFP_ZFProperty_callbackDealloc;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnInit;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnDealloc;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnVerify;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnAttach;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnDetach;
    ZFCoreArrayPOD<_ZFP_PropLifeCycleData> _ZFP_ZFPropertyLifeCycle_OnUpdate;
};

// ============================================================
zfclassFwd ZFFilterForZFProperty;
/** @brief see #ZFPropertyGetAll */
extern ZF_ENV_EXPORT void ZFPropertyGetAllT(ZF_OUT ZFCoreArray<const ZFProperty *> &ret,
                                            ZF_IN_OPT const ZFFilterForZFProperty *propertyFilter = zfnull);
/**
 * @brief get all property currently registered, for debug use only
 */
inline ZFCoreArrayPOD<const ZFProperty *> ZFPropertyGetAll(ZF_IN_OPT const ZFFilterForZFProperty *propertyFilter = zfnull)
{
    ZFCoreArrayPOD<const ZFProperty *> ret;
    ZFPropertyGetAllT(ret, propertyFilter);
    return ret;
}

// ============================================================
extern ZF_ENV_EXPORT ZFProperty *_ZFP_ZFPropertyRegister(ZF_IN zfbool propertyIsUserRegister
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
                                                         );
extern ZF_ENV_EXPORT void _ZFP_ZFPropertyUnregister(ZF_IN const ZFProperty *propertyInfo);

zfclassLikePOD ZF_ENV_EXPORT _ZFP_ZFPropertyRegisterHolder
{
public:
    _ZFP_ZFPropertyRegisterHolder(ZF_IN zfbool propertyIsUserRegister
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
    : propertyInfo(_ZFP_ZFPropertyRegister(propertyIsUserRegister
                                           , propertyIsDynamicRegister
                                           , propertyDynamicRegisterUserData
                                           , propertyOwnerClass
                                           , name
                                           , typeName
                                           , typeIdName
                                           , setterMethod
                                           , getterMethod
                                           , propertyClassOfRetainProperty
                                           , callbackIsValueAccessed
                                           , callbackIsInitValue
                                           , callbackValueSet
                                           , callbackValueGet
                                           , callbackValueReset
                                           , callbackCompare
                                           , callbackGetInfo
                                           , callbackValueStore
                                           , callbackValueRelease
                                           , callbackSerializeFrom
                                           , callbackSerializeTo
                                           , callbackProgressUpdate
                                           , callbackUserRegisterInitValueSetup
                                           , callbackDealloc
                                           ))
    {
    }
    ~_ZFP_ZFPropertyRegisterHolder(void)
    {
        _ZFP_ZFPropertyUnregister(this->propertyInfo);
    }
public:
    ZFProperty *propertyInfo;
};

// ============================================================
template<typename T_PropHT, typename T_PropVT>
inline void _ZFP_propCbDValueSet(ZF_IN const ZFProperty *property,
                                 ZF_IN ZFObject *dstObj,
                                 ZF_IN const void *value)
{
    property->setterMethod()->_ZFP_execute<void, T_PropVT const &>(dstObj, *(const T_PropHT *)value);
}
template<typename T_PropHT, typename T_PropVT>
inline const void *_ZFP_propCbDValueGet(ZF_IN const ZFProperty *property,
                                        ZF_IN ZFObject *ownerObj)
{
    return &(property->getterMethod()->_ZFP_execute<T_PropVT const &>(ownerObj));
}
template<typename T_PropHT>
inline ZFCompareResult _ZFP_propCbDCompare(ZF_IN const ZFProperty *property,
                                           ZF_IN ZFObject *ownerObj,
                                           ZF_IN const void *v0,
                                           ZF_IN const void *v1)
{
    return ZFComparerDefault(*(const T_PropHT *)v0, *(const T_PropHT *)v1);
}
template<typename T_PropHT>
inline void _ZFP_propCbDGetInfo(ZF_IN const ZFProperty *property,
                                ZF_IN ZFObject *ownerObj,
                                ZF_IN const void *value,
                                ZF_IN_OUT zfstring &ret)
{
    ZFCoreElementInfoGetter<T_PropHT>::elementInfoGetter(ret, *(const T_PropHT *)value);
}

extern ZF_ENV_EXPORT void _ZFP_ZFPropertyValueStoreImpl(ZF_IN const ZFProperty *property,
                                                        ZF_IN ZFObject *ownerObj,
                                                        ZF_IN void *valueStored,
                                                        ZF_IN ZFCorePointerBase *valueHolder);
extern ZF_ENV_EXPORT void _ZFP_ZFPropertyValueReleaseImpl(ZF_IN const ZFProperty *property,
                                                          ZF_IN ZFObject *ownerObj,
                                                          ZF_IN void *valueStored);
template<typename T_PropHT>
inline void *_ZFP_propCbDValueStore(ZF_IN const ZFProperty *property,
                                    ZF_IN ZFObject *ownerObj,
                                    ZF_IN_OPT const void *value)
{
    T_PropHT *valueStored = zfnew(T_PropHT, *(const T_PropHT *)value);
    _ZFP_ZFPropertyValueStoreImpl(property, ownerObj, valueStored, zfnew(ZFCorePointerForObject<T_PropHT *>, valueStored));
    return valueStored;
}
template<typename T_PropHT>
inline void _ZFP_propCbDValueRelease(ZF_IN const ZFProperty *property,
                                     ZF_IN ZFObject *ownerObj,
                                     ZF_IN void *value)
{
    _ZFP_ZFPropertyValueReleaseImpl(property, ownerObj, value);
}

template<typename T_PropVT>
zfclassNotPOD _ZFP_ZFTypeId_propCbSerialize
{
public:
    static ZFPropertyCallbackSerializeFrom f(void)
    {
        return zfnull;
    }
    static ZFPropertyCallbackSerializeTo t(void)
    {
        return zfnull;
    }
};

extern ZF_ENV_EXPORT zfbool _ZFP_propCbDSerializeFrom_generic(ZF_IN const ZFProperty *propertyInfo,
                                                              ZF_IN ZFObject *ownerObject,
                                                              ZF_IN const ZFSerializableData &serializableData,
                                                              ZF_OUT_OPT zfstring *outErrorHint = zfnull,
                                                              ZF_OUT_OPT ZFSerializableData *outErrorPos = zfnull);
extern ZF_ENV_EXPORT zfbool _ZFP_propCbDSerializeFrom_impl(ZF_OUT zfautoObject &zfv,
                                                           ZF_IN const ZFProperty *propertyInfo,
                                                           ZF_IN const ZFSerializableData &serializableData,
                                                           ZF_OUT_OPT zfstring *outErrorHint = zfnull,
                                                           ZF_OUT_OPT ZFSerializableData *outErrorPos = zfnull);
extern ZF_ENV_EXPORT void _ZFP_propCbDSerializeFrom_errorOccurred(ZF_IN const ZFSerializableData &serializableData,
                                                                  ZF_OUT_OPT zfstring *outErrorHint = zfnull,
                                                                  ZF_OUT_OPT ZFSerializableData *outErrorPos = zfnull);
template<typename T_PropVT>
zfbool _ZFP_propCbDSerializeFrom(ZF_IN const ZFProperty *propertyInfo,
                                 ZF_IN ZFObject *ownerObject,
                                 ZF_IN const ZFSerializableData &serializableData,
                                 ZF_OUT_OPT zfstring *outErrorHint /* = zfnull */,
                                 ZF_OUT_OPT ZFSerializableData *outErrorPos /* = zfnull */)
{
    zfautoObject zfv;
    if(_ZFP_propCbDSerializeFrom_impl(zfv, propertyInfo, serializableData, outErrorHint, outErrorPos))
    {
        if(ZFTypeId<T_PropVT>::template Value<T_PropVT>::accessAvailable(zfv))
        {
            propertyInfo->setterMethod()->execute<void, T_PropVT const &>(ownerObject,
                    ZFTypeId<T_PropVT>::template Value<T_PropVT const &>::access(zfv)
                );
            ZFTypeId<T_PropVT>::template Value<T_PropVT const &>::accessFinish(zfv);
            return zftrue;
        }
        else
        {
            _ZFP_propCbDSerializeFrom_errorOccurred(serializableData, outErrorHint, outErrorPos);
            return zffalse;
        }
    }
    else
    {
        return zffalse;
    }
}
template<typename T_PropVT>
ZFPropertyCallbackSerializeFrom _ZFP_propCbDSerializeFrom_get(void)
{
    if(_ZFP_ZFTypeId_propCbSerialize<T_PropVT>::f())
    {
        return _ZFP_ZFTypeId_propCbSerialize<T_PropVT>::f();
    }
    else
    {
        return _ZFP_propCbDSerializeFrom<T_PropVT>;
    }
}

extern ZF_ENV_EXPORT zfbool _ZFP_propCbDSerializeTo_generic(ZF_IN const ZFProperty *propertyInfo,
                                                            ZF_IN ZFObject *ownerObject,
                                                            ZF_OUT ZFSerializableData &serializableData,
                                                            ZF_OUT_OPT zfstring *outErrorHint = zfnull);
extern ZF_ENV_EXPORT zfbool _ZFP_propCbDSerializeTo_impl(ZF_IN const ZFProperty *propertyInfo,
                                                         ZF_IN ZFObject *zfv,
                                                         ZF_OUT ZFSerializableData &serializableData,
                                                         ZF_OUT_OPT zfstring *outErrorHint = zfnull);
extern ZF_ENV_EXPORT void _ZFP_propCbDSerializeTo_errorOccurred(ZF_IN const ZFSerializableData &serializableData,
                                                                ZF_OUT_OPT zfstring *outErrorHint = zfnull);
template<typename T_PropVT>
zfbool _ZFP_propCbDSerializeTo(ZF_IN const ZFProperty *propertyInfo,
                               ZF_IN ZFObject *ownerObject,
                               ZF_OUT ZFSerializableData &serializableData,
                               ZF_OUT_OPT zfstring *outErrorHint = zfnull)
{
    zfautoObject zfv;
    if(ZFTypeId<T_PropVT>::ValueStore(zfv, propertyInfo->getterMethod()->execute<T_PropVT const &>(ownerObject)))
    {
        return _ZFP_propCbDSerializeTo_impl(propertyInfo, zfv, serializableData, outErrorHint);
    }
    else
    {
        _ZFP_propCbDSerializeTo_errorOccurred(serializableData, outErrorHint);
        return zffalse;
    }
}
template<typename T_PropVT>
ZFPropertyCallbackSerializeTo _ZFP_propCbDSerializeTo_get(void)
{
    if(_ZFP_ZFTypeId_propCbSerialize<T_PropVT>::t())
    {
        return _ZFP_ZFTypeId_propCbSerialize<T_PropVT>::t();
    }
    else
    {
        return _ZFP_propCbDSerializeTo<T_PropVT>;
    }
}

template<typename T_Type, typename T_TypeFix = void, typename T_ReservedFix = void>
zfclassNotPOD _ZFP_ZFPropertyProgressHolder
{
public:
    static zfbool update(ZF_IN_OUT_OPT void *ret = zfnull,
                         ZF_IN_OPT const void *from = zfnull,
                         ZF_IN_OPT const void *to = zfnull,
                         ZF_IN_OPT zffloat progress = 1)
    {
        return zffalse;
    }
};
template<typename T_PropHT, typename T_PropVT>
inline zfbool _ZFP_propCbDProgressUpdate(ZF_IN const ZFProperty *property,
                                         ZF_IN ZFObject *ownerObj,
                                         ZF_IN_OPT const void *from = zfnull,
                                         ZF_IN_OPT const void *to = zfnull,
                                         ZF_IN_OPT zffloat progress = 1)
{
    if(from == zfnull)
    {
        return _ZFP_ZFPropertyProgressHolder<T_PropVT>::update();
    }
    else
    {
        T_PropHT v = T_PropHT();
        if(_ZFP_ZFPropertyProgressHolder<T_PropVT>::update(&v, from, to, progress))
        {
            property->setterMethod()->_ZFP_execute<void, T_PropVT const &>(ownerObj, v);
            return zftrue;
        }
        else
        {
            return zffalse;
        }
    }
}
template<typename T_PropVT>
inline zfbool _ZFP_PropTypeProgressUpdate(ZF_IN_OUT T_PropVT &zfv,
                                          ZF_IN_OPT const void *from = zfnull,
                                          ZF_IN_OPT const void *to = zfnull,
                                          ZF_IN_OPT zffloat progress = 1)
{
    if(from == zfnull)
    {
        return _ZFP_ZFPropertyProgressHolder<T_PropVT>::update();
    }
    else
    {
        return _ZFP_ZFPropertyProgressHolder<T_PropVT>::update(&zfv, from, to, progress);
    }
}

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProperty_h_

