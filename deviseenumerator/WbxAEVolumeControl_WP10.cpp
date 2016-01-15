#include "WbxAEVolumeControl_WP8.h"
#include "WbxAEDeviceEmu_winp8.h"
#include "WbxAeTrace.h"
#define WP8VOL_INFO_TRACE(str) WBXAE_INFO_TRACE( "[WP8_VolumeControl]:"<<str)
#define WP8VOL_WARNING_TRACE(str) WBXAE_WARNING_TRACE( "[WP8_VolumeControl]:"<<str)
#define WP8VOL_ERROR_TRACE(str) WBXAE_ERROR_TRACE( "[WP8_VolumeControl]:"<<str)

#define WP8VOL_DEBUG_TRACE( str)   WBXAE_DEBUG_TRACE("[WP8_VolumeControl]:"<<str)
#define WP8VOL_DETAIL_TRACE( str)  WBXAE_DETAIL_TRACE("[WP8_VolumeControl]:"<<str)

#define WP8VOL_INFO_TRACE_THIS(str)     WBXAE_INFO_TRACE_THIS("[WP8_VolumeControl]:"<<str)
#define WP8VOL_WARNING_TRACE_THIS( str) WBXAE_WARNING_TRACE_THIS("[WP8_VolumeControl]:"<<str)
#define WP8VOL_ERROR_TRACE_THIS( str)   WBXAE_ERROR_TRACE_THIS("[WP8_VolumeControl]:"<<str)
#define WP8VOL_DEBUG_TRACE_THIS( str)   WBXAE_DEBUG_TRACE_THIS("[WP8_VolumeControl]:"<<str)
#define WP8VOL_DETAIL_TRACE_THIS( str)  WBXAE_DETAIL_TRACE_THIS("[WP8_VolumeControl]:"<<str)


CWBXVolumeControl_wp8::CWBXVolumeControl_wp8(const WBXAEDEVICEID& deviceId,CWBXDeviceEnumerator_WP8* pRoot):m_pRoot(pRoot)
{
	m_iType = (WbxAEDeviceType)deviceId.flow;
	WP8VOL_DEBUG_TRACE_THIS("CWBXVolumeControlMac_ios::CWBXVolumeControlMac_ios(), set audio device type:" << m_iType);
}

CWBXVolumeControl_wp8::~CWBXVolumeControl_wp8()
{

}

int CWBXVolumeControl_wp8::Mute(WbxAEVolumeCtrlType nType)
{
	if (WBXAE_VOL_DIGITAL == nType)
	{
		if (m_iType == WBXAE_DEVICE_CAPTURE)
		{
			m_pRoot->GetAudioEngineIns()->SetCaptureEngineMuteStatus(true);
		}
		else if (m_iType == WBXAE_DEVICE_RENDER)
		{
			m_pRoot->GetAudioEngineIns()->SetPlaybackEngineMuteStatus(true);
		}
	}
	WP8VOL_INFO_TRACE_THIS("CWBXVolumeControlMac_ios::Mute, m_iType(0:Capture, 1: Playback/Render):" << m_iType);
	return 0;
}

int CWBXVolumeControl_wp8::UnMute(WbxAEVolumeCtrlType nType)
{
    if (WBXAE_VOL_DIGITAL == nType)
	{
		if (m_iType == WBXAE_DEVICE_CAPTURE)
		{
			m_pRoot->GetAudioEngineIns()->SetCaptureEngineMuteStatus(false);
		}
		else if (m_iType == WBXAE_DEVICE_RENDER)
		{
			m_pRoot->GetAudioEngineIns()->SetPlaybackEngineMuteStatus(false);
		}
	}
	WP8VOL_INFO_TRACE_THIS("CWBXVolumeControlMac_ios::UnMute, m_iType(0:Capture, 1: Playback/Render):" << m_iType);
	return 0;
}

bool CWBXVolumeControl_wp8::IsMute(WbxAEVolumeCtrlType nType)
{
	bool bMute = false;
	if (WBXAE_VOL_DIGITAL == nType)
	{
		if (m_iType == WBXAE_DEVICE_CAPTURE)
		{
			m_pRoot->GetAudioEngineIns()->GetCaptureEngineMuteStatus(bMute);
		}
		else if (m_iType == WBXAE_DEVICE_RENDER)
		{
			m_pRoot->GetAudioEngineIns()->GetPlaybackEngineMuteStatus(bMute);
		}
	}
	WP8VOL_INFO_TRACE_THIS("CWBXVolumeControlMac_ios::IsMute, m_iType(0:Capture, 1: Playback/Render):" << m_iType<<", mute:"<<bMute);
	return bMute;
}