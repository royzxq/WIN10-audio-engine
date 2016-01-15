#ifndef IWBXAECAPTUREPLATFORMWIN32__H__
#define IWBXAECAPTUREPLATFORMWIN32__H__

#ifdef  USING_GTEST
#define private public
#define protected public
#endif

#include "windows.h"

#include <synchapi.h>
#include <audioclient.h>
#include <phoneaudioclient.h>

#include "ThreadEmulation.h"

#include "IWbxAeCapturePlatform.h"
#include "WseMutex.h"
#include <mmreg.h>
//#include <dsound.h>

//typedef HRESULT (WINAPI *DIRECTSOUNDCAPTURECREATE)(LPCGUID, LPDIRECTSOUNDCAPTURE *, IUnknown *);

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

class CWbxAeAudioCapturePlatformWP80 : public IWbxAeAudioCapturePlatform									
{
public:
	CWbxAeAudioCapturePlatformWP80();

	virtual ~ CWbxAeAudioCapturePlatformWP80();

	// interface IWbxAeAudioCapturePlatform

	virtual bool IsFormatSupport(const WBXWAVEFORMAT &format);
	virtual int GetDefaultFormat(WBXWAVEFORMAT *pFormat);
	virtual int OpenDevice(const WBXWAVEFORMAT *pFormat);
	virtual int CloseDevice();
	virtual int StartCapture();
	virtual int StopCapture();
	virtual int SetDevice(const WBXAEDEVICEID *pDeviceID);
	virtual bool IsDeviceSet(const WBXAEDEVICEID *pDeviceID);
	virtual int SetSink(IWbxAeAudioDataSink *pSink);
	virtual void OnUnifiedTimer();
	virtual bool IsSupportCoreAudio()
	{
		return false;
	};

	//adding SetVoipPhoneCallstatus status
	//virtual int SetOption(const char* OptionName, int OptionNamelen , const char* OptionValue, int OptionValuelen);


public:
	int RecordCapturedData();

	CWseMutex m_PhoneCalllock;
	
private:
	IWbxAeAudioDataSink *m_pDataSink;

	WBXAEDEVICEID	m_deviceID;	

private:
    HRESULT InitCapture();
    HRESULT StartAudioThreads();

	static DWORD WINAPI CaptureThreadProc(LPVOID lpParameter);

    int m_sourceFrameSizeInBytes;

    WAVEFORMATEX* m_pwfx;

    // Devices
    IAudioClient2* m_pDefaultCaptureDevice;

    // Actual render and capture objects
    IAudioCaptureClient* m_pCaptureClient;

//    Changed by XZ, add IActiveAudioInterfaceCompletionHandler
    IActiveAudioInterfaceCompletionHandler * m_pCpltHdl;
//    Changed by XZ, add IActivateAudioInterfaceAsyncOperation
    IActivateAudioInterfaceAsyncOperation * m_pIAudioAsyncOp;

    
    // Misc interfaces
    IAudioClock* m_pClock; 
    ISimpleAudioVolume* m_pVolume;

    // Audio buffer size
    UINT32 m_nMaxFrameCount;
    HANDLE m_hCaptureEvent;

    // Event for stopping audio capture/render
    HANDLE m_hShutdownEvent;
	HANDLE m_CaptureThread;
	LPCWSTR pwstrCaptureId;

	// Has audio started?
    bool started;
	bool device_opened;
	
public:
	unsigned int		m_uTotalDataLen;
	long ReStartCapture();
};

#endif