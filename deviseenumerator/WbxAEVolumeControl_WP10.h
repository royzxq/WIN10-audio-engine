#ifndef _WbxAEVolumeControl_WP8_H_
#define _WbxAEVolumeControl_WP8_H_

#include "IWbxAeBase.h"
#include "WseTimer.h"
#include "WseMutex.h"

class CWBXDeviceEnumerator_WP8;
class CWBXVolumeControl_wp8 :public IWBXVolumeControl
{
public:
	CWBXVolumeControl_wp8(const WBXAEDEVICEID& deviceId,CWBXDeviceEnumerator_WP8* pRoot);
	
	~CWBXVolumeControl_wp8();
	
    //Get/Set Volume 0 ~ 65535, if return volume > 65535 is error
	virtual int SetVolume(unsigned int  volume, WbxAEVolumeCtrlType nType = WBXAE_VOL_APPLICATION) {return 0;};
	virtual unsigned int  GetVolume(WbxAEVolumeCtrlType nType = WBXAE_VOL_APPLICATION) {return 0;};
    
	//Delete created volume control object, this object doesn't apply reference count.
	virtual void Destroy(){};
    
	virtual int Mute(WbxAEVolumeCtrlType nType = WBXAE_VOL_DIGITAL);
	virtual int UnMute(WbxAEVolumeCtrlType nType = WBXAE_VOL_DIGITAL);
	virtual bool IsMute(WbxAEVolumeCtrlType nType = WBXAE_VOL_DIGITAL);
	virtual void GetLineInfo(WBXAEMIXERINFO& lineInfo){}; //XP only, see WBXAEMIXERINFO
    
	//Get the binded device ID
	virtual void GetDeviceID(WBXAEDEVICEID& devId){};
    
	//Register Unregister the volume change notification.
	virtual int RegisterNotification(IWBXVolumeControlSink* pSink){return 0;};
	virtual int UnregisterNotification(IWBXVolumeControlSink* pSink) {return 0;};
public:
	CWBXVolumeControl_wp8* m_self;
	IWBXVolumeControlSink* m_Sink;
	WBXAEDEVICEID m_wbxDeviceID;
	bool m_bInputVolumeChangeListenerCallback;
	bool m_bOutputVolumeChangeListenerCallback;
	bool m_bMutedChangeListenerCallback;
	bool m_bMuted;

	
private:
	bool          m_isCallbackInstalled;
    WbxAEDeviceType  m_iType;
    CWBXDeviceEnumerator_WP8* m_pRoot;
};

#endif
