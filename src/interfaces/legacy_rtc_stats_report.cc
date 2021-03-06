/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/interfaces/legacy_rtc_stats_report.h"

#include <cstdint>
#include <utility>

#include <v8.h>

namespace node_webrtc {

Nan::Persistent<v8::Function>& LegacyStatsReport::constructor() {
  static Nan::Persistent<v8::Function> constructor;
  return constructor;
}

NAN_METHOD(LegacyStatsReport::New) {
  if (info.Length() != 2 || !info[0]->IsExternal() || !info[1]->IsExternal()) {
    return Nan::ThrowTypeError("You cannot construct an LegacyStatsReport");
  }

  auto timestamp = static_cast<double*>(v8::Local<v8::External>::Cast(info[0])->Value());
  auto stats = static_cast<std::map<std::string, std::string>*>(v8::Local<v8::External>::Cast(info[1])->Value());

  auto obj = new LegacyStatsReport(*timestamp, *stats);
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(LegacyStatsReport::names) {
  Nan::HandleScope scope;

  auto self = Nan::ObjectWrap::Unwrap<LegacyStatsReport>(info.This());
  v8::Local<v8::Array> names = Nan::New<v8::Array>(self->_stats.size());

  uint32_t i = 0;
  for (const auto& pair : self->_stats) {
    auto name = pair.first;
    names->Set(i++, Nan::New<v8::String>(name).ToLocalChecked());
  }

  info.GetReturnValue().Set(names);
}

NAN_METHOD(LegacyStatsReport::stat) {
  Nan::HandleScope scope;

  auto self = Nan::ObjectWrap::Unwrap<LegacyStatsReport>(info.This());
  auto requested = std::string(*v8::String::Utf8Value(info[0]->ToString()));

  v8::Local<v8::Value> found = Nan::Undefined();
  for (const auto& pair : self->_stats) {
    auto name = pair.first;
    if (requested == name) {
      found = Nan::New<v8::String>(pair.second).ToLocalChecked();
    }
  }

  info.GetReturnValue().Set(found);
}

NAN_GETTER(LegacyStatsReport::GetTimestamp) {
  (void) property;

  auto self = Nan::ObjectWrap::Unwrap<LegacyStatsReport>(info.Holder());

  info.GetReturnValue().Set(Nan::New<v8::Number>(self->_timestamp));
}

NAN_GETTER(LegacyStatsReport::GetType) {
  (void) property;

  auto self = Nan::ObjectWrap::Unwrap<LegacyStatsReport>(info.Holder());
  auto type = self->_stats.find("type")->second;

  info.GetReturnValue().Set(Nan::New<v8::String>(type).ToLocalChecked());
}

LegacyStatsReport* LegacyStatsReport::Create(double timestamp, const std::map<std::string, std::string>& stats) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> cargv[2];
  cargv[0] = Nan::New<v8::External>(static_cast<void*>(&timestamp));
  cargv[1] = Nan::New<v8::External>(const_cast<void*>(static_cast<const void*>(&stats)));
  auto report = Nan::NewInstance(Nan::New(LegacyStatsReport::constructor()), 2, cargv).ToLocalChecked();
  return Nan::ObjectWrap::Unwrap<LegacyStatsReport>(report);
}

void LegacyStatsReport::Init(v8::Handle<v8::Object> exports) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate> (New);
  tpl->SetClassName(Nan::New("LegacyStatsReport").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "names", names);
  Nan::SetPrototypeMethod(tpl, "stat", stat);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("timestamp").ToLocalChecked(), GetTimestamp, nullptr);
  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("type").ToLocalChecked(), GetType, nullptr);

  constructor().Reset(tpl->GetFunction());
  exports->Set(Nan::New("LegacyStatsReport").ToLocalChecked(), tpl->GetFunction());
}

}  // namespace node_webrtc
