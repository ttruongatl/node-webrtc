#include "src/dictionaries/node_webrtc/some_error.h"

#include <webrtc/api/rtc_error.h>

namespace node_webrtc {

CONVERTER_IMPL(webrtc::RTCError*, SomeError, error) {
  return Converter<const webrtc::RTCError*, SomeError>::Convert(error);
}

CONVERTER_IMPL(const webrtc::RTCError*, SomeError, error) {
  if (!error) {
    return Validation<SomeError>::Invalid("No error? Please file a bug on https://github.com/js-platform/node-webrtc");
  }
  auto type = MakeRight<ErrorFactory::DOMExceptionName>(ErrorFactory::ErrorName::kError);
  switch (error->type()) {
    case webrtc::RTCErrorType::NONE:
      return Validation<SomeError>::Invalid("No error? Please file a bug on https://github.com/js-platform/node-webrtc");
    case webrtc::RTCErrorType::UNSUPPORTED_PARAMETER:
    case webrtc::RTCErrorType::INVALID_PARAMETER:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kInvalidAccessError);
      break;
    case webrtc::RTCErrorType::INVALID_RANGE:
      type = MakeRight<ErrorFactory::DOMExceptionName>(ErrorFactory::ErrorName::kRangeError);
      break;
    case webrtc::RTCErrorType::SYNTAX_ERROR:
      type = MakeRight<ErrorFactory::DOMExceptionName>(ErrorFactory::ErrorName::kSyntaxError);
      break;
    case webrtc::RTCErrorType::INVALID_STATE:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kInvalidStateError);
      break;
    case webrtc::RTCErrorType::INVALID_MODIFICATION:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kInvalidModificationError);
      break;
    case webrtc::RTCErrorType::NETWORK_ERROR:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kNetworkError);
      break;
    // NOTE(mroberts): SetLocalDescription in the wrong state can throw this.
    case webrtc::RTCErrorType::INTERNAL_ERROR:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kInvalidStateError);
      break;
    case webrtc::RTCErrorType::UNSUPPORTED_OPERATION:
    case webrtc::RTCErrorType::RESOURCE_EXHAUSTED:
      type = MakeLeft<ErrorFactory::ErrorName>(ErrorFactory::DOMExceptionName::kOperationError);
      break;
  }
  return Pure(SomeError(error->message(), type));
}

TO_JS_IMPL(SomeError, someError) {
  Nan::EscapableHandleScope scope;
  auto message = someError.message();
  return Pure(scope.Escape(someError.name().FromEither<v8::Local<v8::Value>>([message](auto name) {
    switch (name) {
      case ErrorFactory::DOMExceptionName::kInvalidAccessError:
        return ErrorFactory::CreateInvalidAccessError(message);
      case ErrorFactory::DOMExceptionName::kInvalidModificationError:
        return ErrorFactory::CreateInvalidModificationError(message);
      case ErrorFactory::DOMExceptionName::kInvalidStateError:
        return ErrorFactory::CreateInvalidStateError(message);
      case ErrorFactory::DOMExceptionName::kNetworkError:
        return ErrorFactory::CreateNetworkError(message);
      case ErrorFactory::DOMExceptionName::kOperationError:
        return ErrorFactory::CreateOperationError(message);
    }
  }, [message](auto name) {
    switch (name) {
      case ErrorFactory::ErrorName::kError:
        return ErrorFactory::CreateError(message);
      case ErrorFactory::ErrorName::kRangeError:
        return ErrorFactory::CreateRangeError(message);
      case ErrorFactory::ErrorName::kSyntaxError:
        return ErrorFactory::CreateSyntaxError(message);
    }
  })));
}

}  // namespace node_webrtc
