//*********************************************************************************************************************
//
//! Copyright (c) 2019  Mantra Softech (India) Pvt Ltd. All rights reserved.
//!
//! \file	Communication_API.h
//!
//! Mantra Biometric Service (MBS) Software Development Kit (SDK) APIs and Declarations.
//
//*********************************************************************************************************************

#ifndef __COMMON_CORE_H__
#define __COMMON_CORE_H__

#include "..\Header Files\MSP0045CommLib.h"
#include "..\Logger\NativeLogger.h"
#include "..\Header Files\ipl.h"
#include "..\Header Files\iris_engine_v3.h"
#include "..\Header Files\iris_image_record.h"
#include "..\Header Files\stdafx.h"
#include "..\Header Files\Marvis_Auth.h"
#include "..\Header Files\Version.h"
#include "..\Header Files\Jpeg2KEndodeDecode.h"
#include "PositionDetect.h"
#include "..\Header Files\mobile_spoof.h"

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

using namespace NativeLogging;
using namespace std;

#define ISO_HEADER_LENGTH           46
#define FINGER_DATA_BLOCK_LENGTH    14

static const int VENDORID_LENGTH = 5;
static const int PRODUCTID_LENGTH = 5;
static const int DEVICE_LENGTH = 10;

void AutoDetection();
void WaitForDeviceDetectionThread();
#endif // __COMMON_CORE_H__
