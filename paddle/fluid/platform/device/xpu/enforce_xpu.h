/* Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include "paddle/fluid/platform/device/xpu/xpu_header.h"
#include "paddle/fluid/platform/enforce.h"
#include "xpu/bkcl.h"

namespace paddle {
namespace platform {

// Note: XPU runtime api return int, not XPUError_t
inline const char* xpuGetErrorString(int stat) {
  switch (stat) {
    case XPU_SUCCESS:
      return "Success";
    case XPUERR_INVALID_DEVICE:
      return "Invalid XPU device";
    case XPUERR_UNINIT:
      return "XPU runtime not properly inited";
    case XPUERR_NOMEM:
      return "Device memory not enough";
    case XPUERR_NOCPUMEM:
      return "CPU memory not enough";
    case XPUERR_INVALID_PARAM:
      return "Invalid parameter";
    case XPUERR_NOXPUFUNC:
      return "Cannot get XPU Func";
    case XPUERR_LDSO:
      return "Error loading dynamic library";
    case XPUERR_LDSYM:
      return "Error loading func from dynamic library";
    case XPUERR_SIMULATOR:
      return "Error from XPU Simulator";
    case XPUERR_NOSUPPORT:
      return "Operation not supported";
    case XPUERR_ABNORMAL:
      return "Device abnormal due to previous error";
    case XPUERR_KEXCEPTION:
      return "Exception in kernel execution";
    case XPUERR_TIMEOUT:
      return "Kernel execution timed out";
    case XPUERR_BUSY:
      return "Resource busy";
    case XPUERR_USEAFCLOSE:
      return "Use a stream after closed";
    case XPUERR_UCECC:
      return "Uncorrectable ECC";
    case XPUERR_OVERHEAT:
      return "Overheat";
    case XPUERR_UNEXPECT:
      return "Execution error, reach unexpected control flow";
    case XPUERR_DEVRESET:
      return "Device is being reset, try again later";
    case XPUERR_HWEXCEPTION:
      return "Hardware module exception";
    case XPUERR_HBM_INIT:
      return "Error init HBM";
    case XPUERR_DEVINIT:
      return "Error init device";
    case XPUERR_PEERRESET:
      return "Device is being reset, try again later";
    case XPUERR_MAXDEV:
      return "Device count exceed limit";
    case XPUERR_NOIOC:
      return "Unknown IOCTL command";
    case XPUERR_DMATIMEOUT:
      return "DMA timed out, a reboot maybe needed";
    case XPUERR_DMAABORT:
      return "DMA aborted due to error, possibly wrong address or hardware "
             "state";
    case XPUERR_MCUUNINIT:
      return "Firmware not initialized";
    case XPUERR_OLDFW:
      return "Firmware version too old (<15), please update.";
    case XPUERR_PCIE:
      return "Error in PCIE";
    case XPUERR_FAULT:
      return "Error copy between kernel and user space";
    case XPUERR_INTERRUPTED:
      return "Execution interrupted by user";
    default:
      return "unkonwn error";
  }
}

inline const char* bkclGetErrorString(BKCLResult_t stat) {
  switch (stat) {
    case BKCL_SUCCESS:
      return "BKCL_SUCCESS";
    case BKCL_INVALID_ARGUMENT:
      return "BKCL_INVALID_ARGUMENT";
    case BKCL_RUNTIME_ERROR:
      return "BKCL_RUNTIME_ERROR";
    case BKCL_SYSTEM_ERROR:
      return "BKCL_SYSTEM_ERROR";
    case BKCL_INTERNAL_ERROR:
      return "BKCL_INTERNAL_ERROR";
    default:
      return "Unknown BKCL status";
  }
}

inline std::string build_xpu_error_msg(int stat) {
  std::string msg("XPU Error <" + std::to_string(stat) + ">, ");
  return msg + xpuGetErrorString(stat) + " ";
}

inline std::string build_xpu_error_msg(BKCLResult_t stat) {
  std::string msg("BKCL Error, ");
  return msg + bkclGetErrorString(stat) + " ";
}

namespace details {

template <typename T>
struct ExternalApiType {};

#define DEFINE_EXTERNAL_API_TYPE(type, success_value) \
  template <>                                         \
  struct ExternalApiType<type> {                      \
    using Type = type;                                \
    static constexpr Type kSuccess = success_value;   \
  }

DEFINE_EXTERNAL_API_TYPE(int, XPU_SUCCESS);
DEFINE_EXTERNAL_API_TYPE(BKCLResult_t, BKCL_SUCCESS);

#undef DEFINE_EXTERNAL_API_TYPE

}  // namespace details

#define PADDLE_ENFORCE_XPU_SUCCESS(COND)                      \
  do {                                                        \
    auto __cond__ = (COND);                                   \
    using __XPU_STATUS_TYPE__ = decltype(__cond__);           \
    constexpr auto __success_type__ =                         \
        ::paddle::platform::details::ExternalApiType<         \
            __XPU_STATUS_TYPE__>::kSuccess;                   \
    if (UNLIKELY(__cond__ != __success_type__)) {             \
      auto __summary__ = paddle::platform::errors::External(  \
          ::paddle::platform::build_xpu_error_msg(__cond__)); \
      __THROW_ERROR_INTERNAL__(__summary__);                  \
    }                                                         \
  } while (0)

}  // namespace platform
}  // namespace paddle
