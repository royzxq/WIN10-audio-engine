﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "Scenario1.xaml.h"

void ::AudioEngineTest::WASAPIAudio::Scenario1::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///Scenario1.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::AudioEngineTest::WASAPIAudio::Scenario1::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
        case 1:
            {
                this->LayoutRoot = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
            }
            break;
        case 2:
            {
                this->Input = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
            }
            break;
        case 3:
            {
                this->Output = safe_cast<::Windows::UI::Xaml::Controls::Grid^>(__target);
            }
            break;
        case 4:
            {
                this->btnPlay = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->btnPlay))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::btnPlay_Click);
            }
            break;
        case 5:
            {
                this->btnPause = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->btnPause))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::btnPause_Click);
            }
            break;
        case 6:
            {
                this->btnPlayPause = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->btnPlayPause))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::btnPlayPause_Click);
            }
            break;
        case 7:
            {
                this->btnStop = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->btnStop))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::btnStop_Click);
            }
            break;
        case 8:
            {
                this->toggleHWOffload = safe_cast<::Windows::UI::Xaml::Controls::ToggleSwitch^>(__target);
            }
            break;
        case 9:
            {
                this->toggleBackgroundAudio = safe_cast<::Windows::UI::Xaml::Controls::ToggleSwitch^>(__target);
            }
            break;
        case 10:
            {
                this->toggleRawAudio = safe_cast<::Windows::UI::Xaml::Controls::ToggleSwitch^>(__target);
            }
            break;
        case 11:
            {
                this->sliderVolume = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->sliderVolume))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&Scenario1::sliderVolume_ValueChanged);
            }
            break;
        case 12:
            {
                this->txtHWBuffer = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            }
            break;
        case 13:
            {
                this->radioTone = safe_cast<::Windows::UI::Xaml::Controls::RadioButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::RadioButton^>(this->radioTone))->Checked += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::radioTone_Checked);
            }
            break;
        case 14:
            {
                this->sliderFrequency = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->sliderFrequency))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&Scenario1::sliderFrequency_ValueChanged);
            }
            break;
        case 15:
            {
                this->radioFile = safe_cast<::Windows::UI::Xaml::Controls::RadioButton^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::RadioButton^>(this->radioFile))->Checked += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::radioFile_Checked);
            }
            break;
        case 16:
            {
                this->btnFilePicker = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
                (safe_cast<::Windows::UI::Xaml::Controls::Button^>(this->btnFilePicker))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::AudioEngineTest::WASAPIAudio::Scenario1::*)
                    (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&Scenario1::btnFilePicker_Click);
            }
            break;
        case 17:
            {
                this->txtContentProps = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            }
            break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::AudioEngineTest::WASAPIAudio::Scenario1::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}

