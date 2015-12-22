/*
 *  WbxAEDeivceEmu_WP8.h
 *  auidoengine
 *
 *  Created by auido audio on 11-11-2.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef WbxAEDeivceEmu_WP8_H_
#define WbxAEDeivceEmu_WP8_H_
#include <deque>

#include "IWbxAeBase.h"
#include "WseTimer.h"
#include "WseMutex.h"
#include "WbxAEVolumeControl_WP8.h"


class CWBXDeviceProp
{
public:
	std::wstring sName;
	std::wstring sCoreID;
	std::wstring sDevPath;
	ULONG nWaveID;
	//GUID dsGUID;
	ULONG uFlow;
};
typedef std::deque<CWBXDeviceProp> DEVICELIST;



class  CWBXDeviceEnumerator_WP8: public IWBXDeviceEnumerator
,public WSEUTIL::CWseTimerSink
{
	
public:
	CWBXDeviceEnumerator_WP8(IWbxAudioEngine* pAudioEngine);
	virtual ~CWBXDeviceEnumerator_WP8(void);
	
	CWBXDeviceEnumerator_WP8(const CWBXDeviceEnumerator_WP8&);
	CWBXDeviceEnumerator_WP8& operator=(const CWBXDeviceEnumerator_WP8&);
public:

	static void  CreateInstance(CWBXDeviceEnumerator_WP8*& pEnumerator,IWbxAudioEngine* pAudioEngine);
public:
	//Get number of speakers
	virtual int GetNumOfSpeakers();
	//Get number of microphones
	virtual int GetNumOfMicrophones();
	
	//Get the device object by index
	virtual int GetSpeakerByIndex(int index, WBXAEDEVICEID& deviceID);
	virtual int GetMicrophoneByIndex(int index, WBXAEDEVICEID& deviceID);
	
	//Get the wave ID by WBXAEDEVICEID
	virtual int GetWaveIDByDeviceID(const WBXAEDEVICEID& deviceID);
	
	//Get the system default speaker and microphone.
	virtual int GetSysDefaultSpeaker(WBXAEDEVICEID& deviceID, int ntype = WBXAE_DEFAULT_COMMUNICATION);
	virtual int GetSysDefaultMicrophone(WBXAEDEVICEID& deviceID, int ntype = WBXAE_DEFAULT_COMMUNICATION);
	
	//Register and unregister the device change notification.
	virtual int RegisterNotification(IWBXDeviceEnumeratorSink* pSink);
	virtual int UnregisterNotification(IWBXDeviceEnumeratorSink* pSink);
	
	//Create volume control object by the device ID.
	virtual int CreateVolumeControl(const WBXAEDEVICEID& deviceId, IWBXVolumeControl*& pControl, bool bExitRecover = false);
	virtual void GetOSCapability(LPWBXAEOSCAP pOSCap);

	virtual int GetDeviceInfo(const WBXAEDEVICEID& pDeviceID, AEDEVICEINFO* pDeviceInfo) { return 0; }
    
    virtual void AddRef();
    virtual void Release();
	void OnTimer(WSEUTIL::CWseTimer* aId);
	

protected:
	
	typedef std::deque<IWBXDeviceEnumeratorSink*> OBSERVERLIST;
	//typedef std::deque<CWbxAeVolumeControl*> MIXERLIST;
	
	CWseMutex m_aMutex;
	DEVICELIST m_lstSpks;
	DEVICELIST m_lstMics;
	
	//This two lists only can be accessed in the main thread.
	DEVICELIST m_lstSpksOld;
	DEVICELIST m_lstMicsOld;
	
	CWseMutex m_aMutexMix;
	//MIXERLIST m_mixers;
	
	CWseMutex m_aSinkLock;
	OBSERVERLIST m_Observers;
	
	//HMODULE m_hLibDsound;
	//HINSTANCE m_hInstance;
	//HWND m_hWnd;
	//HDEVNOTIFY m_hCaptureNotify;
	//HDEVNOTIFY m_hRenderNotify;
	
	//MAC IOS
	//AudioUnit					rioUnit;
	BOOL						m_unitIsRunning;
	BOOL						m_unitHasBeenCreated;
	float64						m_hwSampleRate;
	uint32                      m_hwInputNumberChannels;
	uint32                      m_hwOutputNumberChannels;
	float32                     m_hwOutputVolume;
	float32                     m_hwInputLatency;
	float32                     m_hwOutputLatency;
	float32                     m_hwIOBufferDuration;
	
    
    WSEUTIL::CWseTimer			m_DetectASReactiveTimer;
    int                         m_nDetectCount;
    BOOL                        m_bFirstDetectInactive;
    BOOL                        m_bHasBeenInterrupted;
    HANDLE						m_event_WP8;
    BOOL                        m_bHasbeenRouterChanged;
    //WP8_RouteType            m_router_type;
	unsigned int m_nRefCount;
    CWseMutex m_RefMutex;
};
#endif
