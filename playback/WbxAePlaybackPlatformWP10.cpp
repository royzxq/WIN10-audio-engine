
#include "WbxAePlaybackPlatformWP80.h"
#include "WbxAeDefine.h"
#include "WbxAeTrace.h"
//#include "CommonFunc.h"
using namespace Windows::Phone::Media::Devices;
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#define  TIMEOUT_NUMBERS	(2)
#define  RDC_FRAMES	(60)


void FillPcmFormat_o(WAVEFORMATEX& format, WORD wChannels, int nSampleRate, WORD wBits)
{
    format.wFormatTag        = WAVE_FORMAT_PCM;
    format.nChannels         = wChannels;
    format.nSamplesPerSec    = nSampleRate;
    format.wBitsPerSample    = wBits;
    format.nBlockAlign       = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec   = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize            = 0;
}

DWORD CWbxAeAudioPlaybackPlatformWP80::PlaybackThreadProc(LPVOID lpParameter)
{
	CWbxAeAudioPlaybackPlatformWP80 *pPlayback = (CWbxAeAudioPlaybackPlatformWP80*)lpParameter;
	static ULONGLONG ltimelast = GetTickCount64();
	
    while (pPlayback->m_pRenderClient)
    {
		HANDLE eventHandles[] = {
									pPlayback->m_hShutdownEvent,			// WAIT_OBJECT0
									pPlayback->m_hCallBackEvent			// WAIT_OBJECT0 + 1
								};

        DWORD waitResult = WaitForMultipleObjectsEx(SIZEOF_ARRAY(eventHandles), eventHandles, FALSE, INFINITE, FALSE);
	//	if(waitResult == WAIT_OBJECT_0)
	//		break;

		// get the data from datasink

		HRESULT hr = E_FAIL;
		unsigned int padding = 0;
		if(WAIT_OBJECT_0 == waitResult){
			ResetEvent(pPlayback->m_hShutdownEvent);
			break;
		}
		else if(WAIT_OBJECT_0 + 1 == waitResult){
			hr = pPlayback->m_pDefaultRenderDevice->GetCurrentPadding(&padding);
			if(SUCCEEDED(hr))
			{
				BYTE* pRenderBuffer = NULL;
				unsigned int framesToWrite = pPlayback->m_nBuferSize - padding;
				if (framesToWrite)
				{
					hr = pPlayback->m_pRenderClient->GetBuffer(framesToWrite, &pRenderBuffer);
				
					if (SUCCEEDED(hr))
					{
						unsigned int bytesToBeWritten = framesToWrite * pPlayback->m_sourceFrameSizeInBytes;
						int nLen = 0;
						if(pPlayback->m_pDataSink)
						{
							nLen = pPlayback->m_pDataSink->OnNeedData(pRenderBuffer, bytesToBeWritten);//MAX_RAW_BUFFER_SIZE);
						}
						// Release the buffer
						pPlayback->m_pRenderClient->ReleaseBuffer(framesToWrite, 0);
					}
				}
			}
			ResetEvent(pPlayback->m_hCallBackEvent);
		}
		//ULONGLONG Ltemp = GetTickCount64();
		//ULONGLONG ltime = Ltemp-ltimelast;
		//ltimelast = Ltemp;
		
    }
	return (WBXAE_SUCCESS);
}

HRESULT CWbxAeAudioPlaybackPlatformWP80::StartAudioThreads()
{
	m_PlayerThread = ThreadEmulation::CreateThread(NULL, 0, PlaybackThreadProc, this, 0, nullptr);
	if(m_PlayerThread != NULL){
		AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::StartPlaybackThreads succeed");
	}
    return S_OK;
}



CWbxAeAudioPlaybackPlatformWP80::CWbxAeAudioPlaybackPlatformWP80(): m_pDefaultRenderDevice(NULL),
    m_pRenderClient(NULL),
    m_pClock(NULL),
    m_pVolume(NULL),
    m_nMaxFrameCount(0),
    m_pwfx(NULL),
    m_sourceFrameSizeInBytes(0),
    m_hCallBackEvent(NULL),
	m_hShutdownEvent(NULL),
//  added by XZ
    m_pCpltHdl(m_pDefaultRenderDevice),
//  added by XZ
    m_pIAudioAsyncOp(NULL),
    started(false)
{
	device_opened = false;
	m_pDataSink = NULL;
	pwstrRendererId = NULL;
	m_PlayerThread = NULL;
	m_nBuferSize = 0;  
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80 created Succeed");
}

CWbxAeAudioPlaybackPlatformWP80::~ CWbxAeAudioPlaybackPlatformWP80()
{
	
	if (m_PlayerThread)
    {
		ThreadEmulation::TerminateThread(m_PlayerThread);
        if (m_hShutdownEvent)
		{
			SetEvent(m_hShutdownEvent);
			while (1)
			{
				DWORD waitResult = WaitForSingleObjectEx(m_PlayerThread,10, FALSE);
				if (waitResult == WAIT_OBJECT_0)
				{
					AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::m_PlayerThread Exit Success");
					break;
				}
				else{
					ThreadEmulation::Sleep(10);
				}
			}
		}
        m_PlayerThread = NULL;
    }

	

	if (m_hCallBackEvent)
    {
        CloseHandle(m_hCallBackEvent);
        m_hCallBackEvent = NULL;
    }

	if (m_hShutdownEvent)
    {
        CloseHandle(m_hShutdownEvent);
        m_hShutdownEvent = NULL;
    }

	if (m_pVolume)
    {
        m_pVolume->Release();
        m_pVolume = NULL;
    }
    if (m_pClock)
    {
        m_pClock->Release();
        m_pClock = NULL;
    }

    if (m_pRenderClient)
    {
        m_pRenderClient->Release();
        m_pRenderClient = NULL;
    }

    if (m_pDefaultRenderDevice)
    {
        m_pDefaultRenderDevice->Release();
        m_pDefaultRenderDevice = NULL;
    }

    if (m_pwfx)
    {
        CoTaskMemFree((LPVOID)m_pwfx);
        m_pwfx = NULL;
    }
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80 delete Succeed");
}

HRESULT CWbxAeAudioPlaybackPlatformWP80::InitRender()
{
	HRESULT hr = E_FAIL;

    if (m_pDefaultRenderDevice != NULL)
    {
        hr = m_pDefaultRenderDevice->Start();
    }

    if (pwstrRendererId)
    {
        CoTaskMemFree((LPVOID)pwstrRendererId);
    }

    return hr;
}

// Questioned by XZ, to check normal windows how they detect?
bool CWbxAeAudioPlaybackPlatformWP80::IsFormatSupport(const WBXWAVEFORMAT &format)
{
	bool bRet = false;


	return (bRet);
}

int CWbxAeAudioPlaybackPlatformWP80::GetDefaultFormat(WBXWAVEFORMAT *pFormat)
{

	HRESULT hr = E_FAIL;

//    Changed by XZ
//    pwstrRendererId = GetDefaultAudioRenderId(AudioDeviceRole::Communications/*Communications*//*Default*/);
    pwstrRendererId = Windows::Media::Devices::GetDefaultAudioRenderId(AudioDeviceRole::Communications/*Communications*//*Default*/);
    
    if (m_pDefaultRenderDevice != NULL){
        m_pDefaultRenderDevice->Release();
    }
//     Changed by XZ, the API has changed
//    hr = ActivateAudioInterface(pwstrRendererId, __uuidof(IAudioClient2), (void**)&m_pDefaultRenderDevice);
    hr = ActivateAudioInterfaceAsync(pwstrRendererId,__uuidof(IAudioClient2), NULL/*NULL FOR IAudioClient*/, m_pCpltHdl,(IActivateAudioInterfaceAsyncOperation**)&m_pIAudioAsyncOp);
    
    
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications /*AudioCategory_Communications,AudioCategory_Other*/;
        hr = m_pDefaultRenderDevice->SetClientProperties(&properties);
    }


	if(m_pDefaultRenderDevice != NULL){
		hr = m_pDefaultRenderDevice->GetMixFormat(&m_pwfx);
		pFormat->wFormatTag = WBX_WAVE_FORMAT_PCM;
		pFormat->nSamplesPerSec = m_pwfx->nSamplesPerSec;
		pFormat->dwSize = sizeof(WBXWAVEFORMAT);
		pFormat->nChannels =  m_pwfx->nChannels;
		pFormat->wBitsPerSample =  16; ///format will reset in opendevice into short if default is float
		pFormat->nBlockAlign = pFormat->nChannels * pFormat->wBitsPerSample/8;
		pFormat->nAvgBytesPerSec = pFormat->nBlockAlign * pFormat->nSamplesPerSec;
	}
	else{
		pFormat->wFormatTag = WBX_WAVE_FORMAT_PCM;
		pFormat->nSamplesPerSec = 48000;
		pFormat->dwSize = sizeof(WBXWAVEFORMAT);
		pFormat->nChannels = 2;
		pFormat->wBitsPerSample =  32;
		pFormat->nBlockAlign = pFormat->nChannels * pFormat->wBitsPerSample/8;
		pFormat->nAvgBytesPerSec = pFormat->nBlockAlign * pFormat->nSamplesPerSec;
	}
	return (WBXAE_SUCCESS);
}

int CWbxAeAudioPlaybackPlatformWP80::OpenDevice(const WBXWAVEFORMAT *pFormat)
{
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::OpenDevice");
	HRESULT hr = E_FAIL;
	
	if(device_opened == true){
		AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::OpenDevice again");
		return (WBXAE_SUCCESS);
	}

	if(pwstrRendererId != NULL){
		pwstrRendererId = NULL;
	}
//Changed by XZ, API Changed
    pwstrRendererId = Windows::Media::Devices::GetDefaultAudioRenderId(AudioDeviceRole::Communications/*Communications*//*Default*/);

    if (NULL == pwstrRendererId)
    {
        hr = E_FAIL;
    }

	if(m_pDefaultRenderDevice != NULL){
		m_pDefaultRenderDevice->Release();
	}
//    Changed by XZ, use the new API
//    hr = ActivateAudioInterface(pwstrRendererId, __uuidof(IAudioClient2), (void**)&m_pDefaultRenderDevice);
    hr = ActivateAudioInterface(pwstrRendererId, __uuidof(IAudioClient2), NULL, m_pCpltHdl, (IActivateAudioInterfaceAsyncOperation**)&m_pIAudioAsyncOp);
    
    
        // Set the category through SetClientProperties
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications /*AudioCategory_Communications,AudioCategory_Other*/;
        hr = m_pDefaultRenderDevice->SetClientProperties(&properties);
    }

//    WAVEFORMATEX* pwfx = nullptr;
	WAVEFORMATEXTENSIBLE* pexent = NULL;
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetMixFormat(&m_pwfx);
		switch (m_pwfx->wFormatTag)
		{

			case WAVE_FORMAT_EXTENSIBLE:
			{
				// naked scope for case-local variable
				PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(m_pwfx);

				LPOLESTR strID = NULL;
				StringFromCLSID(pEx->SubFormat, &strID);
				std::wstring guidName;
				if(strID != NULL)
				{
					guidName = strID;
					CoTaskMemFree(strID);
				}
				guidName.clear();

				if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
					pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
					pEx->Samples.wValidBitsPerSample = 16;
					m_pwfx->wBitsPerSample = 16;
					m_pwfx->nBlockAlign = m_pwfx->nChannels * m_pwfx->wBitsPerSample / 8;
					m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
				}
				else
				{
					return E_FAIL;
				}
			}
			break;
		}

    }

    WAVEFORMATEX format = {};
    if (SUCCEEDED(hr))
    {
//        FillPcmFormat_o(format, m_pwfx->nChannels, m_pwfx->nSamplesPerSec, m_pwfx->wBitsPerSample); 
		//FillPcmFormat(format, 1, 16000, 16);
//        *m_pwfx = format;

        m_sourceFrameSizeInBytes = (m_pwfx->wBitsPerSample / 8) * m_pwfx->nChannels;

		/*AUDCLNT_E_ALREADY_INITIALIZED*/
        hr = m_pDefaultRenderDevice->Initialize(AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            20 * 10000,  // Seconds in hns
            0, // periodicity
            m_pwfx, 
            NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);
    }
    
    // Check for other supported GetService interfaces as well
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(IAudioClock), (void**)&m_pClock);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(ISimpleAudioVolume), (void**)&m_pVolume);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetBufferSize(&m_nMaxFrameCount);
    }
	hr = m_pDefaultRenderDevice->GetBufferSize(&m_nBuferSize);
	if(m_hCallBackEvent == NULL){
		m_hCallBackEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
		hr = m_pDefaultRenderDevice->SetEventHandle(m_hCallBackEvent);
	}

	if(FAILED(hr)){
		AE_ERROR_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::OpenDevice Failed");
		return (WBXAE_PE_ERROR_BASE);
	}

	device_opened = true;

	return (WBXAE_SUCCESS);
}

int CWbxAeAudioPlaybackPlatformWP80::CloseDevice()
{
	device_opened = false;

	if (m_hCallBackEvent)
    {
        CloseHandle(m_hCallBackEvent);
        m_hCallBackEvent = NULL;
    }
	
	if (m_hShutdownEvent)
    {
        CloseHandle(m_hShutdownEvent);
        m_hShutdownEvent = NULL;
    }

	if (m_pVolume)
    {
        m_pVolume->Release();
        m_pVolume = NULL;
    }
    if (m_pClock)
    {
        m_pClock->Release();
        m_pClock = NULL;
    }

    if (m_pRenderClient)
    {
        m_pRenderClient->Release();
        m_pRenderClient = NULL;
    }

    if (m_pDefaultRenderDevice)
    {
        m_pDefaultRenderDevice->Release();
        m_pDefaultRenderDevice = NULL;
    }

    if (m_pwfx)
    {
        CoTaskMemFree((LPVOID)m_pwfx);
        m_pwfx = NULL;
    }

    AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::CloseDevice Succeed");
	return (WBXAE_SUCCESS);
}

int CWbxAeAudioPlaybackPlatformWP80::StartPlayback()
{
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::StartPlayback");
	if (started){
		AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::StartPlayback Again");
		return (WBXAE_SUCCESS);
	}
	
	m_hShutdownEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
    if (!m_hShutdownEvent)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = InitRender();

    if (SUCCEEDED(hr))
    {
        hr = StartAudioThreads();
    }

    if (FAILED(hr))
    {
        StopPlayback();
		AE_ERROR_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::StartPlayback Failed");
		return (WBXAE_CE_ERROR_AUDIO_CAPTURE_PLAYBACK_START);
    }

    started = true;

	return (WBXAE_SUCCESS);

}

int CWbxAeAudioPlaybackPlatformWP80::StopPlayback()
{
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::StopPlayback");

	if (m_hShutdownEvent)
    {
        SetEvent(m_hShutdownEvent);
		while (1)
		{
			DWORD waitResult = WaitForSingleObjectEx(m_PlayerThread,10, FALSE);
			if (waitResult == WAIT_OBJECT_0)
			{
				AE_INFO_TRACE_THIS(0, "CWbxAeAudioPlaybackPlatformWP80::m_PlayerThread Exit Success");
				break;
			}
			else{
				ThreadEmulation::Sleep(10);
			}
		}
    }

    if (m_pDefaultRenderDevice)
    {
        m_pDefaultRenderDevice->Stop();
    }

    if (m_PlayerThread)
    {
		ThreadEmulation::TerminateThread(m_PlayerThread);
        //CloseHandle(m_PlayerThread);
        m_PlayerThread = NULL;
    }

    started = false;

	return (WBXAE_SUCCESS);
}

//FILE *fpplay = 0;



int CWbxAeAudioPlaybackPlatformWP80::SetDevice(const WBXAEDEVICEID *pDeviceID)
{

	return (WBXAE_SUCCESS);
}

bool CWbxAeAudioPlaybackPlatformWP80::IsDeviceSet(const WBXAEDEVICEID *pDeviceID)
{

	return false;
}

int CWbxAeAudioPlaybackPlatformWP80::SetSink(IWbxAeAudioPlaybackDataSink *pSink)
{
//	CM_INFO_TRACE_THIS("CWbxAeAudioPlaybackPlatformWP80::SetSink = " << pSink);
	m_pDataSink = pSink;
	return (WBXAE_SUCCESS);

}


void CWbxAeAudioPlaybackPlatformWP80::AddCaptureTimer(IUnifiedTimer* m_pUnifiedTimer)
{
//	CM_INFO_TRACE_THIS("CWbxAeAudioPlaybackPlatformWP80::AddCaptureTimer     m_pUnifiedTimer = " << m_pUnifiedTimer);
	m_pTimerForCapt = m_pUnifiedTimer;
}
void CWbxAeAudioPlaybackPlatformWP80::RmvCaptureTimer(IUnifiedTimer* m_pUnifiedTimer)
{
	if (m_pTimerForCapt == m_pUnifiedTimer)
	{
//		CM_INFO_TRACE_THIS("CWbxAeAudioPlaybackPlatformWP80::RmvCaptureTimer     m_pTimerForCapt = " << m_pTimerForCapt);
		m_pTimerForCapt = NULL;
	}
}



