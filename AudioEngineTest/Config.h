//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once 
#include "pch.h"

namespace AudioEngineTest
{
	  typedef unsigned int UNIT32;
	  value struct Scenario;

	  partial ref class MainPage
	  {
	  internal:
			static property Platform::String^ FEATURE_NAME
			{
				  Platform::String^ get()
				  {
						return ref new Platform::String(L"Windows Audio Session API (WASAPI) Sample");
				  }
			}

			static property Platform::Array<Scenario>^ scenarios
			{
				  Platform::Array<Scenario>^ get()
				  {
						return scenariosInner;
				  }
			}

	  private:
			static Platform::Array<Scenario>^ scenariosInner;
	  };

	  public value struct Scenario
	  {
			Platform::String^ Title;
			Platform::String^ ClassName;
	  };
}
