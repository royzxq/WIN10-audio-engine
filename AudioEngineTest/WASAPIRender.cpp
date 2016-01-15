#include "pch.h"
#include "WASAPIRender.h"


using namespace Windows::System::Threading;
using namespace AudioEngineTest::WASAPIAudio;

WASAPIRender::WASAPIRender() :
	  m_BufferFrames(0),
	  m_DeviceStateChanged(nullptr),
	  m_AudioClient(nullptr),
	  m_AudioRenderClient(nullptr),
	  m_SampleReadyAsyncResult(nullptr)
{
	  m_SampleReadyEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	  if (nullptr == m_SampleReadyEvent)
	  {		
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	  }
	  if (!InitializeCriticalSectionEx( &m_CritSec, 0, 0))
	  {
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	  }
	  m_DeviceStateChanged = ref new DeviceChangedEvent();
	  if (nullptr == m_DeviceStateChanged)
	  {
			ThrowIfFailed(E_OUTOFMEMORY);
	  }
}

WASAPIRender::~WASAPIRender() {
	  SAFE_RELEASE(m_AudioClient);
	  SAFE_RELEASE(m_AudioRenderClient);
	  SAFE_RELEASE(m_SampleReadyAsyncResult);
	  
	  if (INVALID_HANDLE_VALUE != m_SampleReadyEvent)
	  {
			CloseHandle(m_SampleReadyEvent);
			m_SampleReadyEvent = INVALID_HANDLE_VALUE;
	  }
	  DeleteCriticalSection(&m_CritSec);
	  m_DeviceStateChanged = nullptr;
}

HRESULT WASAPIRender::InitAudioDeviceAsync() {
	  IActivateAudioInterfaceAsyncOperation * asyncOp;
	  HRESULT hr = S_OK;

	  m_DeviceIdString = MediaDevice::GetDefaultAudioRenderId(Windows::Media::Devices::AudioDeviceRole::Default);

	  hr = ActivateAudioInterfaceAsync(m_DeviceIdString->Data(), __uuidof(IAudioClient3), nullptr, this, &asyncOp);

	  if (FAILED(hr))
	  {
			m_DeviceStateChanged->SetState(DeviceState::DeviceStateInError, hr, true);

	  }
	  SAFE_RELEASE(asyncOp);
	  return hr;
}

HRESULT WASAPIRender::ActivateComplete(IActivateAudioInterfaceAsyncOperation * operation) {

}