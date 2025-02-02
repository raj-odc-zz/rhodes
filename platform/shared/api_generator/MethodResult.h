#pragma once

#include "common/RhoStd.h"
#include "common/AutoPointer.h"
#include "logging/RhoLog.h"

namespace rho
{
namespace apiGenerator
{

class CMethodResult
{
    DEFINE_LOGCLASS

public:
    enum ETypes{ eNone = 0, eString, eStringW, eStringArray, eStringHash, eArrayHash, eJSON, eBool, eInt, eDouble, eError, eArgError};
private:
    rho::String m_strRubyCallback, m_strCallbackParam, m_strParamName;
    rho::String m_strJSCallback;
    int m_iTabId;
    rho::Hashtable<rho::String, rho::String> m_hashStrRes;
    rho::Hashtable<rho::String, rho::Hashtable<rho::String, rho::String> > m_hashStrL2Res;
    rho::Vector<rho::Hashtable<rho::String, rho::String> > m_arHashRes;
    rho::String m_strRes;
    rho::String m_strJSONRes;
    rho::StringW m_strResW;
    rho::Vector<rho::String> m_arStrRes;
    int64 m_nRes;
    bool m_bRes;
    double m_dRes;

    rho::String m_strError;
    ETypes m_ResType, m_eRequestedType;

    unsigned long m_oRubyObjectClass;
    rho::String   m_strRubyObjectClassPath;
    bool m_bCollectionMode;

    struct CMethodRubyValue
    {
        unsigned long m_value;
        CMethodRubyValue(unsigned long val);
        ~CMethodRubyValue();

        unsigned long getValue(){return m_value;}
        operator unsigned long() {return m_value;}
    };

    rho::common::CAutoPtr<CMethodRubyValue> m_pRubyCallbackProc;

    void setType(ETypes eType){ m_ResType = eType; convertToType(m_eRequestedType); callCallback(); }

public:

    CMethodResult(bool bCollectionMode=false): m_strParamName("result"), m_iTabId(-1), m_ResType(eNone), m_oRubyObjectClass(0), m_bCollectionMode(bCollectionMode),
        m_nRes(0), m_bRes(false), m_dRes(0), m_eRequestedType(eNone){}

    void setRubyCallback(const rho::String& strCallback){ m_strRubyCallback = strCallback; }
    const rho::String& getRubyCallback() const { return m_strRubyCallback; }
    void setRubyCallbackProc(unsigned long oRubyCallbackProc);
    void setJSCallback(const rho::String& strCallback);
    void setCallInUIThread(rho::boolean bUIThread) {}
    void setCallbackParam(const rho::String& strCallbackParam){ m_strCallbackParam = strCallbackParam; }
    const rho::String& getCallbackParam() const { return m_strCallbackParam; }
    void setParamName(const rho::String& strParam){m_strParamName = strParam;}
    void setRubyObjectClass(unsigned long val){ m_oRubyObjectClass = val; }
    void setRubyObjectClassPath(const rho::String& strPath){ m_strRubyObjectClassPath = strPath; }
    void setJSObjectClassPath(const rho::String& strPath){m_strRubyObjectClassPath = strPath;}
    void setRequestedType( ETypes eRequestedType ){ m_eRequestedType = eRequestedType;}

    void set(const rho::Hashtable<rho::String, rho::String>& res){ m_hashStrRes = res; setType(eStringHash); }
    void set(const rho::Vector<rho::Hashtable<rho::String, rho::String> > res ) { m_arHashRes = res; setType(eArrayHash); }

#ifndef OS_ANDROID
    void set(const rho::StringW& res){ m_strResW = res;  setType(eStringW); }
    void set(rho::StringW::const_pointer res){ m_strResW = res;  setType(eStringW); }
#endif //OS_ANDROID

    void set(const rho::String& res){ m_strRes = res;  setType(eString); }
    void setJSON(const rho::String& res){ m_strJSONRes = res;  setType(eJSON);}
    void set(rho::String::const_pointer res){ m_strRes = res;  setType(eString); }

    void set(const rho::Vector<rho::String>& res){ m_arStrRes = res;  setType(eStringArray); }
    void set(bool res){ m_bRes = res;  setType(eBool); }
    void set(int64 res){ m_nRes = res;  setType(eInt);  }
    void set(int res){ m_nRes = res;  setType(eInt); }
    void set(unsigned long res){ m_nRes = res;  setType(eInt); }
    void set(double res){ m_dRes = res;  setType(eDouble);  }

    void setError(const rho::String& res){ m_strError = res; setType(eError); }
    void setArgError(const rho::String& fmt, ...)
    {
        //TODO: support sprintf
        m_strError = fmt;
        setType(eArgError);
    }

    ETypes getType() const { return m_ResType; }
    bool isError() const { return m_ResType == eError || m_ResType == eArgError; }

    rho::Vector<rho::String>& getStringArray(){ return m_arStrRes; }
    rho::Vector<rho::Hashtable<rho::String, rho::String> >& getHashArray() { return m_arHashRes; }
    rho::Hashtable<rho::String, rho::String>& getStringHash(){ return m_hashStrRes; }
    rho::Hashtable<rho::String, rho::Hashtable<rho::String, rho::String> >& getStringHashL2(){ return m_hashStrL2Res; }
    const rho::String& getString() const { return m_strRes; }
    const rho::StringW& getStringW() const { return m_strResW; }
    rho::String& getString() { return m_strRes; }
    rho::String& getJSON() { return m_strJSONRes; }
    rho::StringW& getStringW() { return m_strResW; }
    bool getBool() const { return m_bRes; }
    int64 getInt() const { return m_nRes; }
    double getDouble() const { return m_dRes; }

    unsigned long getObjectClass() const { return m_oRubyObjectClass; }
    const rho::String& getObjectClassPath() const { return m_strRubyObjectClassPath; }
    const rho::String& getParamName() const { return m_strParamName; }
    const rho::String& getErrorString() const { return m_strError; }

    rho::String toString();
    void convertToType(ETypes eType);

    void setCollectionMode(bool bMode){m_bCollectionMode = bMode;}

    unsigned long toRuby(bool bForCallback = false);
    rho::String toJSON();

    bool hasCallback();
    void callCallback();
    bool isEqualCallback(CMethodResult& oResult);
};

}
}
