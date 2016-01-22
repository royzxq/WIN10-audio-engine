#pragma once
#include "pch.h"
#include "Scenario1.g.h"
#include "MainPage.xaml.h"
#include "WASAPIRender.h"

namespace AudioEngineTest {
	  namespace WASAPIAudio {
			[Windows::Foundation::Metadata::WebHostHidden]
			public ref class Scenario1 sealed {
			public:
				  Scenario1();
			protected:
				  virtual void OnNavigateTo(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs ^e) override;
				  virtual void OnNavigateFrom(Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs ^e) override;
			
			private:
				  ~Scenario1();

				  void btnPlay_Click(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void btnPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void btnStop_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void btnPlayPause_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void btnFilePicker_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void radioTone_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void radioFile_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs ^e);
				  void sliderFrequency_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^e);
				  void sliderVolume_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs ^e);
				  
				  void ShowStatusMessage(Platform::String ^ str, NotifyType messageType);
				  void UpdateContentProps(Platform::String^ str);
				  void UpdateContentUI(Platform::Boolean disableAll);
				  void UpdateMediaControlUI(DeviceState deviceState);

				  void OnDeviceStateChange(Object ^ sender, DeviceStateChangedEventArgs ^ e);
				  void OnPickFileAsync();
				  void OnsetVolume(UINT32 volume);

				  void InitDevice();
				  void StartDevice();
				  void StopDevice();
				  void PauseDevice();
				  void PlayPauseToggleDevice();

				  void MediaButtonPressed(Windows::Media::SystemMediaTransportControls^ sender, Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs^ e);

			private:
				  MainPage ^ rootPage;
				  Windows::UI::Core::CoreDispatcher ^ m_CoreDispatcher;
				  Windows::Media::SystemMediaTransportControls ^ m_SystemMediaControls;
				  Windows::Foundation::EventRegistrationToken m_deviceStateChangeToken;
				  Windows::Foundation::EventRegistrationToken m_SystemMediaControlsButtonToken;

				  Platform::Boolean m_IsMFLoaded;
				  IRandomAccessStream^ m_ContentStream;
				  ContentType m_ContentType;
				  DeviceChangedEvent ^ m_StateChangeEvent;
				  ComPtr<WASAPIRender> m_spRender;
				  Platform::Boolean m_deviceSupportsRawMode;


			};
	  }
}