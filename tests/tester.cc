#include "../tracer.h"

using namespace node_addon_tracer;

NAN_METHOD(LogTrace) {
	tracer::Log("tester", LogLevel::TRACE, [&info]() {return *Nan::Utf8String(info[0]); });
}

NAN_METHOD(LogDebug) {
	tracer::Log("tester", LogLevel::DEBUG, [&info]() {return  *Nan::Utf8String(info[0]); });
}

NAN_METHOD(LogInfo) {
	tracer::Log("tester", LogLevel::INFO, [&info]() {return  *Nan::Utf8String(info[0]); });
}

NAN_METHOD(LogWarn) {
	tracer::Log("tester", LogLevel::WARN, [&info]() {return  *Nan::Utf8String(info[0]); });
}

NAN_METHOD(LogError) {
	tracer::Log("tester", LogLevel::ERROR, [&info]() {return  *Nan::Utf8String(info[0]); });
}

NAN_METHOD(LogFatal) {
	tracer::Log("tester", LogLevel::FATAL, [&info]() {return  *Nan::Utf8String(info[0]); });
}

void init(v8::Handle<v8::Object> target) {
	//assert(false);
	tracer::Init(target);
	
	Nan::SetMethod(target, "LogTrace", LogTrace);
	Nan::SetMethod(target, "LogDebug", LogDebug);
	Nan::SetMethod(target, "LogInfo", LogInfo);
	Nan::SetMethod(target, "LogWarn", LogWarn);
	Nan::SetMethod(target, "LogError", LogError);
	Nan::SetMethod(target, "LogFatal", LogFatal);
}

NODE_MODULE(hello, init);