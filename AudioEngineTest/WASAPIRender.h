
#include <Windows.h>
#include <wrl\implements.h>
#include <Audioclient.h>
#include <mfapi.h>
#include <mmdeviceapi.h>
#include "MainPage.xaml.h"
#include"DeviseState.h"

using namespace Microsoft::WRL;
using namespace Windows::Media::Devices;
using namespace Windows::Storage::Streams;

#pragma once

namespace AudioEngineTest {
	  namespace WASAPIAudio {
			public enum class ContentType {
				  ContentTypeTone,
				  ContentTypeFile
			};

			struct DEVICEPROPS
			{
				  Platform::Boolean       IsHWOffload;
				  Platform::Boolean       IsTonePlayback;
				  Platform::Boolean       IsBackground;
				  Platform::Boolean       IsRawSupported;
				  Platform::Boolean       IsRawChosen;
				  Platform::Boolean       IsLowLatency;
				  REFERENCE_TIME          hnsBufferDuration;
				  DWORD                   Frequency;
				  IRandomAccessStream^    ContentStream;
			};

			class WASAPIRender: public RuntimeClass< RuntimeClassFlags<ClassicCom>, FtmBase, IActivateAudioInterfaceCompletionHandler>
			{
			public:
				  WASAPIRender();
				  HRESULT SetProperties(DEVICEPROPS props);
				  HRESULT InitAudioDeviceAsync();
				  HRESULT StartPlaybackAsync();
				  HRESULT StopPlaybackAsync();
				  HRESULT PausePlaybackAsync();

				  HRESULT SetVolumeOnSession(UNIT32 volume);
				  DeviceChangedEvent ^ GetDeviceStateEvent() {
						return m_DeviceStateChanged;
				  }

			
				  STDMETHOD(ActivateComplete)(IActivateAudioInterfaceAsyncOperation * operation);
			private:
				  ~WASAPIRender();

				  HRESULT OnStartPlayback(IMFAsyncResult * pResult);
				  HRESULT OnStopPlayback(IMFAsyncResult * pResult);
				  HRESULT OnPausePlayback(IMFAsyncResult * pResult);
				  HRESULT OnSampleReady(IMFAsyncResult * pResult);

				  HRESULT ConfigureDeviceInternal();
				  HRESULT ValidateBufferValue();
				  HRESULT OnAudioSampleRequested(Platform::Boolean IsSilence = false);
				  HRESULT ConfigureSource();
				  UNIT32 GetBufferFramesPerPeriod();

				  HRESULT GetToneSample(UNIT32 FramesAvailable);
				  HRESULT GetMFSample(UNIT32 FramesAvailable);

			private:
				  Platform::String ^ m_DeviceIdString;
				  UNIT32 m_BufferFrames;
				  HANDLE m_SampleReadyEvent;
				  MFWORKITEM_KEY m_SampleReadyKey;
				  CRITICAL_SECTION m_CritSec;

				  WAVEFORMATEX *m_MixFormat;
				  UNIT32	  m_DefaultPeriodInFrames;
				  UNIT32	  m_FundamentalPeriodInFrames;
				  UNIT32	  m_MaxPeriodInFrames;
				  UNIT32	  m_MinPeriodInFrames;

				  IAudioClient3 *m_AudioClient;
				  IAudioRenderClient *m_AudioRenderClient;
				  IMFAsyncResult *m_SampleReadyAsyncResult;

				  DeviceChangedEvent ^ m_DeviceStateChanged;
				  DEVICEPROPS m_DeviceProps;


			};

			
	  }
}