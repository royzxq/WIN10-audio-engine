//#include "pch.h"

//#include "IWbxAeBase.h"
#include "WbxAeCapturePlatformWP80.h"
#include "WbxAeDefine.h"
#include "mmsystem.h"
#include "WbxAeTrace.h"
//#include "CommonFunc.h"

using namespace Windows::Phone::Media::Devices;
using namespace Windows::Foundation;
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#ifndef  TIMEOUT_NUMBERS
#define  TIMEOUT_NUMBERS	(2)
#endif


void FillPcmFormat_i(WAVEFORMATEX& format, WORD wChannels, int nSampleRate, WORD wBits)
{
    format.wFormatTag        = WAVE_FORMAT_PCM;
    format.nChannels         = wChannels;
    format.nSamplesPerSec    = nSampleRate;
    format.wBitsPerSample    = wBits;
    format.nBlockAlign       = format.nChannels * (format.wBitsPerSample / 8);
    format.nAvgBytesPerSec   = format.nSamplesPerSec * format.nBlockAlign;
    format.cbSize            = 0;
}

DWORD CWbxAeAudioCapturePlatformWP80::CaptureThreadProc(LPVOID lpParameter)
{
	CWbxAeAudioCapturePlatformWP80 *pCapture = (CWbxAeAudioCapturePlatformWP80*)lpParameter;
	HRESULT hr = 0;
//	HRESULT hr = pCapture->m_pDefaultCaptureDevice->Start();
    BYTE *pLocalBuffer = new BYTE[MAX_RAW_BUFFER_SIZE];
    HANDLE eventHandles[] = {
                             pCapture->m_hShutdownEvent,			// WAIT_OBJECT0
                             pCapture->m_hCaptureEvent			// WAIT_OBJECT0 + 1
                            };
	long nTotalSize = 0;
	long nFrameNum = 0;

	int tem_num = 0;

	if (SUCCEEDED(hr) && pLocalBuffer)
    {
        unsigned int uAccumulatedBytes = 0;
        while (SUCCEEDED(hr))
        {	
			
            DWORD waitResult = WaitForMultipleObjectsEx(SIZEOF_ARRAY(eventHandles), eventHandles, FALSE, INFINITE, FALSE);
			if (WAIT_OBJECT_0 == waitResult)
            {
                //AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::CaptureThreadProc m_hShutdownEvent Reviced");
				ResetEvent(pCapture->m_hShutdownEvent);
                break;
            }
            else if (WAIT_OBJECT_0 + 1 == waitResult)
            {
                BYTE* pbData = nullptr;
                UINT32 nFrames = 0;
                DWORD dwFlags = 0;
                if (SUCCEEDED(hr))
                {
                    hr = pCapture->m_pCaptureClient->GetBuffer(&pbData, &nFrames, &dwFlags, nullptr, nullptr);
                    unsigned int incomingBufferSize = nFrames * pCapture->m_sourceFrameSizeInBytes;

					//Trace("---------- CWbxAeAudioCapturePlatformWP80::run: dwFlags=%d, time=%d, incomingBufferSize=%d, uAccumulatedBytes=%d ---------------------\n", 
					//	dwFlags, ccu::getTickCount(), incomingBufferSize, uAccumulatedBytes);
					if(/*(dwFlags & AUDCLNT_BUFFERFLAGS_SILENT) != AUDCLNT_BUFFERFLAGS_SILENT*/dwFlags==0)
					{
						if (pCapture->m_pDataSink)
                        {
							nTotalSize+= incomingBufferSize;
							nFrameNum++;
							pCapture->m_pDataSink->OnData(pbData, incomingBufferSize);
                        }
					}
                }

                if ((SUCCEEDED(hr))&&( pCapture->m_pCaptureClient !=NULL))
                {
                    hr = pCapture->m_pCaptureClient->ReleaseBuffer(nFrames);
                }
            }
            else
            {
                // Unknown return value
                DbgRaiseAssertionFailure();
            }
        }
    }

    delete[] pLocalBuffer;
	return (WBXAE_SUCCESS);
}

HRESULT CWbxAeAudioCapturePlatformWP80::StartAudioThreads()
{
    m_hShutdownEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
    if (!m_hShutdownEvent)
    {
		AE_ERROR_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StartCaptureThreads failed GetLastError is "<< HRESULT_FROM_WIN32(GetLastError()));
        return HRESULT_FROM_WIN32(GetLastError());
    }

	m_CaptureThread = ThreadEmulation::CreateThread(NULL, 0, CaptureThreadProc, this, 0, nullptr);
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StartCaptureThreads succeed");
    return S_OK;
}

CWbxAeAudioCapturePlatformWP80::CWbxAeAudioCapturePlatformWP80() :
    m_pClock(NULL),
    m_pVolume(NULL),
    m_nMaxFrameCount(0),
    m_pwfx(NULL),
    m_pDefaultCaptureDevice(NULL),
    m_pCaptureClient(NULL),
    m_sourceFrameSizeInBytes(0),
    m_hCaptureEvent(NULL),
    m_hShutdownEvent(NULL),
//  added by XZ
    m_pCpltHdl(m_pDefaultCaptureDevice),
//  added by XZ
    m_pIAudioAsyncOp(NULL),

    started(false)
{
	device_opened = false;
	m_CaptureThread = NULL;
	
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80 create succeed");
}

CWbxAeAudioCapturePlatformWP80::~ CWbxAeAudioCapturePlatformWP80()
{
	device_opened = false;

	if (m_CaptureThread)
    {
		ThreadEmulation::TerminateThread(m_CaptureThread);
        if (m_hShutdownEvent)
		{
			SetEvent(m_hShutdownEvent);
			while (1)
			{
				DWORD waitResult = WaitForSingleObjectEx(m_CaptureThread,10, FALSE);
				if (waitResult == WAIT_OBJECT_0)
				{
					AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::m_CaptureThread Exit Success");
					break;
				}
				else{
					ThreadEmulation::Sleep(10);
				}
			}
		}
        m_CaptureThread = NULL;
    }
    

    if (m_pDefaultCaptureDevice)
    {
        m_pDefaultCaptureDevice->Stop();
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

    if (m_pCaptureClient)
    {
        m_pCaptureClient->Release();
        m_pCaptureClient = NULL;
    }

    if (m_pDefaultCaptureDevice)
    {
        m_pDefaultCaptureDevice->Release();
        m_pDefaultCaptureDevice = NULL;
    }

    if (m_pwfx)
    {
        CoTaskMemFree((LPVOID)m_pwfx);
        m_pwfx = NULL;
    }

    if (m_hCaptureEvent)
    {
        CloseHandle(m_hCaptureEvent);
        m_hCaptureEvent = NULL;
    }

    if (m_hShutdownEvent)
    {
        CloseHandle(m_hShutdownEvent);
        m_hShutdownEvent = NULL;
    }

	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80 delete succeed");
}



HRESULT CWbxAeAudioCapturePlatformWP80::InitCapture()
{
    HRESULT hr = E_FAIL;

	if (m_pDefaultCaptureDevice != NULL)
    {
        hr = m_pDefaultCaptureDevice->Start();
		//LOGMSG("m_pDefaultCaptureDevice->Start() is %d",hr);
    }
    
    if (pwstrCaptureId)
    {
        CoTaskMemFree((LPVOID)pwstrCaptureId);
    }
    return hr;
}


bool CWbxAeAudioCapturePlatformWP80::IsFormatSupport(const WBXWAVEFORMAT &format)
{
	bool bRet = false;

	return (bRet);
}

int CWbxAeAudioCapturePlatformWP80::GetDefaultFormat(WBXWAVEFORMAT *pFormat)
{
	pFormat->nSamplesPerSec = 48000;
	pFormat->wFormatTag = WBX_WAVE_FORMAT_PCM;
	pFormat->dwSize = sizeof(WBXWAVEFORMAT);
	pFormat->nChannels = 1;
	pFormat->wBitsPerSample = 16;
	pFormat->nBlockAlign = pFormat->nChannels * pFormat->wBitsPerSample/8;
	pFormat->nAvgBytesPerSec = pFormat->nBlockAlign * pFormat->nSamplesPerSec;
	return (WBXAE_SUCCESS);
}

int CWbxAeAudioCapturePlatformWP80::OpenDevice(const WBXWAVEFORMAT *pFormat)
{
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::OpenDevice");
	HRESULT hr = E_FAIL;
	if(device_opened == true){
		AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::OpenDevice again!");
		return (WBXAE_SUCCESS);
	}

	if( pwstrCaptureId != NULL){
		pwstrCaptureId = NULL;
	}

    pwstrCaptureId = Windows::Media::Devices::GetDefaultAudioCaptureId(AudioDeviceRole::Communications/*Communications*//*Default*/);

    if (NULL == pwstrCaptureId)
    {
        hr = E_FAIL;
    }

	if(m_pDefaultCaptureDevice != NULL){
		m_pDefaultCaptureDevice->Release();
	}

//     Changed by XZ, the API has changed
//    hr = ActivateAudioInterface(pwstrRendererId, __uuidof(IAudioClient2), (void**)&m_pDefaultRenderDevice);
    hr = ActivateAudioInterfaceAsync(pwstrRendererId,__uuidof(IAudioClient2), NULL/*NULL FOR IAudioClient*/, m_pCpltHdl,(IActivateAudioInterfaceAsyncOperation**)&m_pIAudioAsyncOp);
//    hr = ActivateAudioInterface(pwstrCaptureId, __uuidof(IAudioClient2), (void**)&m_pDefaultCaptureDevice);

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultCaptureDevice->GetMixFormat(&m_pwfx);
    }

    // Set the category through SetClientProperties
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications /*AudioCategory_Communications,AudioCategory_Other*/;
        hr = m_pDefaultCaptureDevice->SetClientProperties(&properties);
    }
	int dw;
    if (SUCCEEDED(hr))
    {
        //0x88140000 has AUDCLNT_STREAMFLAGS_EVENTCALLBACK in it already
        WAVEFORMATEX temp;
        //FillPcmFormat_i(temp, 1, 48000, 16); 
		FillPcmFormat_i(temp, 1, 48000, 16); 
        *m_pwfx = temp;
		m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
        m_sourceFrameSizeInBytes = (m_pwfx->wBitsPerSample / 8) * m_pwfx->nChannels;
		
		/*AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE*/
		/*AUDCLNT_STREAMFLAGS_EVENTCALLBACK*/
        hr = m_pDefaultCaptureDevice->Initialize(AUDCLNT_SHAREMODE_SHARED, 0x00140000, 40 * 10000, 0, m_pwfx, NULL);
		dw = (int)hr;
	}

    if (SUCCEEDED(hr))
    {
        m_hCaptureEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_INITIAL_SET, EVENT_ALL_ACCESS);
        if (NULL == m_hCaptureEvent)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultCaptureDevice->SetEventHandle(m_hCaptureEvent);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultCaptureDevice->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient);
    }

	if(FAILED(hr)){
		AE_ERROR_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::OpenDevice Failed!");
		return (WBXAE_CE_ERROR_AUDIO_CAPTURE_PLAYBACK_DEVICE);
	}
	device_opened = true;
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::OpenDevice Succeed!");
	return (WBXAE_SUCCESS);
}

int  CWbxAeAudioCapturePlatformWP80::CloseDevice()
{
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

    if (m_pCaptureClient)
    {
        m_pCaptureClient->Release();
        m_pCaptureClient = NULL;
    }

    if (m_pDefaultCaptureDevice)
    {
        m_pDefaultCaptureDevice->Release();
        m_pDefaultCaptureDevice = NULL;
    }

    if (m_pwfx)
    {
        CoTaskMemFree((LPVOID)m_pwfx);
        m_pwfx = NULL;
    }

    if (m_hCaptureEvent)
    {
        CloseHandle(m_hCaptureEvent);
        m_hCaptureEvent = NULL;
    }

    if (m_hShutdownEvent)
    {
        CloseHandle(m_hShutdownEvent);
        m_hShutdownEvent = NULL;
    }

	device_opened = false;
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::CloseDevice Succeed!");
	return (WBXAE_SUCCESS);
}

int CWbxAeAudioCapturePlatformWP80::StartCapture()
{
	// Make sure only one API call is in progress at a time

	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StartCapture");

    if (started){
		AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StartCapture again");
        return (WBXAE_SUCCESS);
	}

    HRESULT hr = InitCapture();

    if (SUCCEEDED(hr))
    {
        hr = StartAudioThreads();
    }

    if (FAILED(hr))
    {
		AE_ERROR_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StartCapture Failed hr is " << hr);
        StopCapture();
		return (WBXAE_CE_ERROR_AUDIO_CAPTURE_PLAYBACK_START);
    }

    started = true;

	return (WBXAE_SUCCESS);

}

int CWbxAeAudioCapturePlatformWP80::StopCapture()
{
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StopCapture");

	started = false;

	// Shutdown the threads
    if (m_hShutdownEvent)
    {
        SetEvent(m_hShutdownEvent);
		while (1)
		{
			DWORD waitResult = WaitForSingleObjectEx(m_CaptureThread,10, FALSE);
			if (waitResult == WAIT_OBJECT_0)
			{
				AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::m_CaptureThread Exit Success");
				break;
			}
			else{
				ThreadEmulation::Sleep(10);
			}
		}
    }

	if (m_pDefaultCaptureDevice)
    {
		HRESULT hr = m_pDefaultCaptureDevice->Stop();

		if(hr != S_OK){
			AE_ERROR_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::m_pDefaultCaptureDevice Stop Failed, hr = "<< hr <<" GetLastError is "<< HRESULT_FROM_WIN32(GetLastError()));
			return (WBXAE_CE_ERROR_AUDIO_CAPTURE_PLAYBACK_STOPERROR);
		}
    }

	if (m_CaptureThread)
    {
		int hr = ThreadEmulation::TerminateThread(m_CaptureThread);
        m_CaptureThread = NULL;
    }
	AE_INFO_TRACE_THIS(0, "CWbxAeAudioCapturePlatformWP80::StopCapture, end");
	return (WBXAE_SUCCESS);
}

int CWbxAeAudioCapturePlatformWP80::RecordCapturedData() 
{
	if (m_pDataSink)
	{
//		m_pDataSink->OnData((unsigned char *)pbCaptureData, dwCaptureLength);
	}



	return 0;
}


int CWbxAeAudioCapturePlatformWP80::SetDevice(const WBXAEDEVICEID *pDeviceID)
{

	return (WBXAE_SUCCESS);
}

bool CWbxAeAudioCapturePlatformWP80::IsDeviceSet(const WBXAEDEVICEID *pDeviceID)
{

	return false;
}

int CWbxAeAudioCapturePlatformWP80::SetSink(IWbxAeAudioDataSink *pSink)
{
//	CM_INFO_TRACE_THIS("CWbxAeAudioCapturePlatformWP80::SetSink = " << pSink);
	m_pDataSink = pSink;
	return (WBXAE_SUCCESS);
}

void CWbxAeAudioCapturePlatformWP80::OnUnifiedTimer()
{
	RecordCapturedData();
}

long CWbxAeAudioCapturePlatformWP80::ReStartCapture()
{

	return (WBXAE_SUCCESS);
}