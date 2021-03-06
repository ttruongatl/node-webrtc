/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#pragma once

#include <memory>

#include <nan.h>
#include <webrtc/api/rtp_sender_interface.h>
#include <webrtc/api/scoped_refptr.h>
#include <v8.h>

#include "src/converters/v8.h"
#include "src/node/async_object_wrap.h"
#include "src/node/wrap.h"

namespace webrtc { class RtpSenderInterface; }

namespace node_webrtc {

class PeerConnectionFactory;

class RTCRtpSender: public AsyncObjectWrap {
 public:
  ~RTCRtpSender() override;

  static void Init(v8::Handle<v8::Object> exports);

  rtc::scoped_refptr<webrtc::RtpSenderInterface> sender() { return _sender; }

  static ::node_webrtc::Wrap <
  RTCRtpSender*,
  rtc::scoped_refptr<webrtc::RtpSenderInterface>,
  std::shared_ptr<PeerConnectionFactory>
  > * wrap();

  static Nan::Persistent<v8::FunctionTemplate>& tpl();

 private:
  RTCRtpSender(
      std::shared_ptr<PeerConnectionFactory>&& factory,
      rtc::scoped_refptr<webrtc::RtpSenderInterface>&& sender);

  static RTCRtpSender* Create(
      std::shared_ptr<PeerConnectionFactory>,
      rtc::scoped_refptr<webrtc::RtpSenderInterface>);

  static Nan::Persistent<v8::Function>& constructor();

  static NAN_METHOD(New);

  static NAN_GETTER(GetTrack);
  static NAN_GETTER(GetTransport);
  static NAN_GETTER(GetRtcpTransport);

  static NAN_METHOD(GetCapabilities);

  static NAN_METHOD(GetParameters);
  static NAN_METHOD(SetParameters);
  static NAN_METHOD(GetStats);
  static NAN_METHOD(ReplaceTrack);

  const std::shared_ptr<PeerConnectionFactory> _factory;
  const rtc::scoped_refptr<webrtc::RtpSenderInterface> _sender;
};

DECLARE_TO_AND_FROM_JS(RTCRtpSender*)

}  // namespace node_webrtc
