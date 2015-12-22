                                                                                                                                                                                                                                                                                                                                        /*
 *  WbxAEDeivceEmu_WP8.cpp
 *  auidoengine
 *
 *  Created by auido audio on 11-11-2.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "WbxAEDeviceEmu_winp8.h"
#include "WbxAeTrace.h"
#define WP8ENU_INFO_TRACE(str) WBXAE_INFO_TRACE("[WP8_DeviceEnumerator]:"<<str)
#define WP8ENU_WARNING_TRACE(str) WBXAE_WARNING_TRACE("[WP8_DeviceEnumerator]:"<<str)
#define WP8ENU_ERROR_TRACE(str) WBXAE_ERROR_TRACE("[WP8_DeviceEnumerator]:"<<str)


#define WP8ENU_INFO_TRACE_THIS(str) WBXAE_INFO_TRACE_THIS("[WP8_DeviceEnumerator]:"<<str)
#define WP8ENU_WARNING_TRACE_THIS(str) WBXAE_WARNING_TRACE_THIS("[WP8_DeviceEnumerator]:"<<str)
#define WP8ENU_ERROR_TRACE_THIS(str) WBXAE_ERROR_TRACE_THIS("[WP8_DeviceEnumerator]:"<<str)

//
void CWBXDeviceEnumerator_WP8::CreateInstance(CWBXDeviceEnumerator_WP8*& pEnumerator,IWbxAudioEngine* pAudioEngine)
{
	pEnumerator = new CWBXDeviceEnumerator_WP8(pAudioEngine);
	WP8ENU_INFO_TRACE("CWBXDeviceEnumerator_WP8::CreateInstance()");
}

CWBXDeviceEnumerator_WP8::CWBXDeviceEnumerator_WP8(IWbxAudioEngine* pAudioEngine):m_nRefCount(0)
{
	SetAudioEngineIns(pAudioEngine);
}

CWBXDeviceEnumerator_WP8::~CWBXDeviceEnumerator_WP8(void)
{

}
void CWBXDeviceEnumerator_WP8::AddRef()
{
    CWseMutexGuardT<CWseMutex> theGuard(m_RefMutex);
    m_nRefCount++;
    WP8ENU_INFO_TRACE(0,"CWBXDeviceEnumerator_WP8::AddRef(),ref:"<<m_nRefCount);

}
void CWBXDeviceEnumerator_WP8::Release()
{
    {
		CWseMutexGuardT<CWseMutex> theGuard(m_RefMutex);
        m_nRefCount --;
	}
    if(0 == m_nRefCount)
    {
        WP8ENU_INFO_TRACE(0,"CWBXDeviceEnumerator_WP8::Release,delete this");
        delete this;
    }
    WP8ENU_INFO_TRACE("CWBXDeviceEnumerator_WP8::Release()");
}
//#pragma mark -Audio Session Property Listener




int CWBXDeviceEnumerator_WP8::GetNumOfSpeakers()
{

	
	return 1;
}

int CWBXDeviceEnumerator_WP8::GetNumOfMicrophones()
{

	
	return 1;
}



int CWBXDeviceEnumerator_WP8::GetSpeakerByIndex(int index, WBXAEDEVICEID& deviceID)
{
	
	return 0;
}

int CWBXDeviceEnumerator_WP8::GetMicrophoneByIndex(int index, WBXAEDEVICEID& deviceID)
{
	
	return 0;
}



int CWBXDeviceEnumerator_WP8::GetWaveIDByDeviceID(const WBXAEDEVICEID& deviceID)
{
	int nRet = -1;
	
	return nRet;
}


int CWBXDeviceEnumerator_WP8::GetSysDefaultSpeaker(WBXAEDEVICEID& deviceID, int ntype)
{
	return 0;
}

int CWBXDeviceEnumerator_WP8::GetSysDefaultMicrophone(WBXAEDEVICEID& deviceID, int ntype)
{
	return 0;
}


int CWBXDeviceEnumerator_WP8::RegisterNotification(IWBXDeviceEnumeratorSink* pSink)
{
	
	return 0;
}

int CWBXDeviceEnumerator_WP8::UnregisterNotification(IWBXDeviceEnumeratorSink* pSink)
{
	return 0;
}


int CWBXDeviceEnumerator_WP8::CreateVolumeControl(const WBXAEDEVICEID& deviceId, IWBXVolumeControl*& pControl, bool bExitRecover)
{
	CWBXVolumeControl_wp8* pTmpVolCon = new CWBXVolumeControl_wp8(deviceId,this);
	if (pTmpVolCon == NULL){
		WP8ENU_ERROR_TRACE_THIS("CWBXDeviceEnumerator_WP8::CreateVolumeControl error!");
		return -1;
	}
	pControl = pTmpVolCon;
	return 0;
 }


void CWBXDeviceEnumerator_WP8::GetOSCapability(LPWBXAEOSCAP pOSCap)
{

}


void CWBXDeviceEnumerator_WP8::OnTimer(WSEUTIL::CWseTimer* aId)
{
	
}


