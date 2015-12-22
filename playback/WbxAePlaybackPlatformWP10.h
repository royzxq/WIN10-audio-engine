#ifndef IWBXAEPLAYBACKPLATFORMWIN32__H__
#define IWBXAEPLAYBACKPLATFORMWIN32__H__

#include "IWbxAePlaybackPlatform.h"
#include <mmreg.h>
//#include <dsound.h>

#include "windows.h"

#include <synchapi.h>
#include <audioclient.h>
#include <phoneaudioclient.h>
#include "ThreadEmulation.h"
//typedef HRESULT (WINAPI *DIRECTSOUNDCREATE)(LPCGUID, LPDIRECTSOUND *, IUnknown *);

// Added by XZ as the completion handler to callback
class CActivateAudioInterfaceCompletionHandler : public IActivateAudioInterfaceCompletionHandler {
public:
    
    CActivateAudioInterfaceCompletionHandler(IAudioClient * audioclient):m_pIAudioClient(audioclient){}
    
    HRESULT ActivateCompleted(IActivateAudioInterfaceAsyncOperation *activateOperation){
        HRESULT hr = E_FAIL;
        activateOperation -> GetActivateResult(&hr,(IUnknown**)&m_pIAudioClient);
        return hr;
    }
private:
    IAudioClient * m_pIAudioClient;
    
}


class CWbxAeAudioPlaybackPlatformWP80	: public IWbxAeAudioPlayPlatform
{
public:
	CWbxAeAudioPlaybackPlatformWP80();
	virtual ~ CWbxAeAudioPlaybackPlatformWP80();

	virtual bool IsFormatSupport(const WBXWAVEFORMAT &format);
	virtual int GetDefaultFormat(WBXWAVEFORMAT *pFormat);
	virtual int OpenDevice(const WBXWAVEFORMAT *pFormat);
	virtual int CloseDevice();
	virtual int StartPlayback();
	virtual int StopPlayback();
	virtual int SetDevice(const WBXAEDEVICEID *pDeviceID);
	virtual bool IsDeviceSet(const WBXAEDEVICEID *pDeviceID);
	virtual int SetSink(IWbxAeAudioPlaybackDataSink *pSink);

	virtual void AddCaptureTimer(IUnifiedTimer* m_pUnifiedTimer);
	virtual void RmvCaptureTimer(IUnifiedTimer* m_pUnifiedTimer);

public:
	// ccu::IRunnable
	//virtual void run();

private:
	IWbxAeAudioPlaybackDataSink *m_pDataSink;

	WBXAEDEVICEID	m_deviceID;	


	HRESULT InitRender();
    HRESULT StartAudioThreads();

	static DWORD WINAPI PlaybackThreadProc(LPVOID lpParameter);

    int m_sourceFrameSizeInBytes;

    WAVEFORMATEX* m_pwfx;

    // Devices
    IAudioClient2* m_pDefaultRenderDevice;
    
//    Issue about the type, if using desktop version, the type should be Platform::String, no idea whether convertible
	LPCWSTR pwstrRendererId;

//    Changed by XZ, add IActiveAudioInterfaceCompletionHandler
    IActiveAudioInterfaceCompletionHandler * m_pCpltHdl;
//    Changed by XZ, add IActivateAudioInterfaceAsyncOperation
    IActivateAudioInterfaceAsyncOperation * m_pIAudioAsyncOp;
    
    // Actual render and capture objects
    IAudioRenderClient* m_pRenderClient;

    // Misc interfaces
    IAudioClock* m_pClock; 
    ISimpleAudioVolume* m_pVolume;

    // Audio buffer size
    UINT32 m_nMaxFrameCount;
    HANDLE m_hPlaybackEvent;

    // Event for stopping audio capture/render
    HANDLE m_hShutdownEvent;
	HANDLE m_PlayerThread;
	HANDLE m_hCallBackEvent;
	UINT32 m_nBuferSize;


	// Has audio started?
    bool started;
	bool device_opened;


#if 0
	LPDIRECTSOUND       m_pDSPlayback;
	LPDIRECTSOUNDBUFFER m_pDSBPlayback;
	LPDIRECTSOUNDNOTIFY        m_pDSNotify;
	GUID                       m_guidPlaybackDevice;
	WAVEFORMATEX               m_wfxInput;
	DSBPOSITIONNOTIFY          m_aPosNotify[ DSPLAYBACKBUFFERSIZE];  
	HANDLE                     m_hNotificationEvent;


	HANDLE m_hPlaybackThread;
	DWORD  m_dwPlaybackThreadID;
	HANDLE                     m_hTimerEvent;
	DWORD  m_dwPlaybackThreadIDCheck;
	CRITICAL_SECTION m_cs;
	DWORD	m_dwNotifySize;
	DWORD	m_dwPlaybackBufferSize;
	DWORD		m_dwNextWriteOffset;
	static DWORD WINAPI PlaybackThread(LPVOID lpParameter);
	
	void StopPlaybackThread();
	HRESULT FillData(int nNumbers = 1);
	BOOL  NeedFillData();
	volatile BOOL  m_bDone;

	HINSTANCE				m_hDSOUND;
	DIRECTSOUNDCREATE	m_DirectSoundCreate;

	DWORD m_dwTotalSystemWritePos;
	DWORD m_dwTotalNextWriteOffset;
	DWORD m_dwPreviousSystemWritePos;
	BOOL m_bRDC;	// is remote desktop control

	unsigned long m_gsMultiMediaTimer;
	UINT m_uResolution;

	int m_nSystemBufSize;
	int m_nStableTimers;

	LARGE_INTEGER     _perfFreq;
	double            m_maxTime;

	DWORD          m_PreCaptureTime;
	DWORD          m_PrePlayTime;
	double		   m_CaptureConsumedTime;
	double		   m_PlaybackConsumedTime;

	int  m_waitTimePlay;
	int  m_waitTimeCapture;

	int m_figFirstPlay;
	int m_waitTimePlay_temp;
#endif
	IUnifiedTimer* m_pTimerForCapt;
	IUnifiedTimer* m_pTimerForPlay;

};

#endif