#include "tracer.h"

Nan::Persistent<v8::Function> tracer::_logger;
LogLevel tracer::log_level;
threadsafe_queue<tracer::log_message> tracer::_log_messages;

std::shared_ptr<uvasync> tracer::_logger_async;

unsigned int tracer::batch_length;

void tracer::Log(std::string module, LogLevel loglevel, std::string message) {
	//TODO: find a way to change this magic number on startup
	if (_log_messages.length() > 10000) {
		_log_messages.clear();
		_log_messages.enqueue(tracer::log_message{ "tracer", LogLevel::WARN, "Log buffer is full, overwriting..."});
	}

	if (loglevel >= tracer::log_level) {
		_log_messages.enqueue(tracer::log_message{ module, loglevel, message });

		if (_logger_async != nullptr) {
			_logger_async->signal();
		}
	}

}

void tracer::Log(std::string module, LogLevel loglevel, std::function<std::string()> message) {
	if (loglevel >= tracer::log_level) {
		tracer::Log(module, loglevel, message());
	}
}

void tracer::Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
	tracer::batch_length = 1000;
	Nan::SetMethod(target, "RegisterLogger", RegisterLogger);
	Nan::SetMethod(target, "Log", Log);
	Nan::SetMethod(target, "Flush", Flush);
	Nan::SetAccessor(target, Nan::New("log_level").ToLocalChecked(), log_level_getter, log_level_setter);
	Nan::SetAccessor(target, Nan::New("batch_length").ToLocalChecked(), batch_length_getter, batch_length_setter);
	_logger_async = std::make_shared<uvasync>(_async_logger_callback);
	node::AtExit(deinit);
}

NAN_SETTER(tracer::log_level_setter) {
	tracer::log_level = (LogLevel)value->IntegerValue();
}
NAN_GETTER(tracer::log_level_getter) {
	info.GetReturnValue().Set((int)tracer::log_level);
}

NAN_SETTER(tracer::batch_length_setter) {
	tracer::batch_length = (unsigned int)value->IntegerValue();
	if (tracer::batch_length < 1) {
		tracer::batch_length = 1;
	}
}
NAN_GETTER(tracer::batch_length_getter) {
	info.GetReturnValue().Set(tracer::batch_length);
}

void tracer::deinit(void*) {
	_logger_async.reset();
}

NAN_METHOD(tracer::Log) {
	tracer::Log(*Nan::Utf8String(info[0]), (LogLevel)info[1]->IntegerValue(), *Nan::Utf8String(info[2]));
}

NAN_METHOD(tracer::Flush) {
	tracer::flush_log_messages();
}


NAN_METHOD(tracer::RegisterLogger)
{
	auto cb = info[0].As<v8::Function>();

	//if we already have a logger, release it
	if (!_logger.IsEmpty()) {
		_logger.Reset();
	}

	if (info.Length() > 0 && info[0]->IsFunction()) {
		_logger.Reset(cb);
	}
	else {
		_log_messages.clear();
	}

	info.GetReturnValue().Set(Nan::Undefined());
}

void tracer::flush_log_messages() {

	log_message lm;
	Nan::Callback *callback = new Nan::Callback(Nan::New(_logger));

	auto max_in_batch = tracer::batch_length;

	while (_log_messages.dequeue(lm) && (max_in_batch > 0) && (lm.level >= tracer::log_level)) {
		max_in_batch--;

		if (!callback->IsEmpty() && !_logger.IsEmpty() && _logger_async != nullptr && _logger_async->isActive()) {
			v8::Local<v8::Value> argv[] = {
				Nan::New(lm.module).ToLocalChecked(),
				Nan::New((int)lm.level),
				Nan::New(lm.message).ToLocalChecked()
			};
			callback->Call(3, argv);
		}
	}

	if (_log_messages.length() > 0) {
		if (_logger_async != nullptr) {
			_logger_async->signal();
		}
	}
}

void tracer::_async_logger_callback(uv_async_t *handle/*, int status UNUSED*/)
{
	Nan::HandleScope scope;
	tracer::flush_log_messages();
}

std::string tracer::join(const std::vector<std::string>& vec, const char* delim)
{
	return tracer::join<std::string>(vec, [](std::string &v) {return v; }, delim);
}

std::string tracer::join(const std::set<std::string>& set, const char* delim)
{
	return tracer::join<std::string>(set, [](std::string &v) {return v; }, delim);
}