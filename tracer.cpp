#include "tracer.h"

Nan::Persistent<v8::Function> tracer::_logger;
LogLevel tracer::log_level;
threadsafe_queue<tracer::log_message> tracer::_log_messages;

std::shared_ptr<uvasync> tracer::_logger_async;

unsigned int tracer::batch_length;
unsigned int tracer::buffer_length;


template<typename T>
static T get_env(std::string&& key) {
	throw std::runtime_error(std::string("not implemented"));
}

template<>
static std::string get_env<std::string>(std::string&& key) {
	auto env_value = getenv(key.c_str());
	if (env_value == NULL) {
		return "";
	}
	return std::string(env_value);
}


template<>
static int get_env<int>(std::string&& key) {
	auto env_value = getenv(key.c_str());
	
	if (env_value == NULL) {
		return 0;
	}

	return std::stoi(std::string(env_value));
}



void tracer::Log(std::string&& module, LogLevel loglevel, std::string&& message) {
	if (loglevel >= tracer::log_level) {
		if (_log_messages.length() > tracer::buffer_length) {
			_log_messages.clear();
			_log_messages.enqueue(tracer::log_message{ "tracer", LogLevel::WARN, "Log buffer is full, overwriting..." });
		}

		_log_messages.enqueue(tracer::log_message{ module, loglevel, message });

		if (_logger_async != nullptr) {
			_logger_async->signal();
		}
	}

}

void tracer::Log(std::string&& module, LogLevel loglevel, std::function<std::string()> message) {
	if (loglevel >= tracer::log_level) {
		tracer::Log(std::move(module), loglevel, std::move(message()));
	}
}

void tracer::Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target) {
	tracer::batch_length = get_env<int>("TRACER_BATCH_LENGTH");
	if (tracer::batch_length  < 1) {
		tracer::batch_length = 1000;
	}

	tracer::buffer_length = get_env<int>("TRACER_BUFFER_LENGTH");
	if (tracer::buffer_length < 1) {
		tracer::buffer_length = 1000;
	}

	auto log_level_string = get_env<std::string>("TRACER_LOG_LEVEL");

	tracer::log_level = LogLevel::WARN;
	

	if (log_level_string == "TRACE" || log_level_string == "0") {
		tracer::log_level = LogLevel::TRACE;
	} else if (log_level_string == "DEBUG" || log_level_string == "1") {
		tracer::log_level = LogLevel::DEBUG;
	} else 	if (log_level_string == "INFO" || log_level_string == "2"){
		tracer::log_level = LogLevel::INFO;
	} else if (log_level_string == "WARN" || log_level_string == "3") {
		tracer::log_level = LogLevel::WARN;
	} else if (log_level_string == "ERROR" || log_level_string == "4"){
		tracer::log_level = LogLevel::ERROR;
	} else if (log_level_string == "FATAL" || log_level_string == "5") {
		tracer::log_level = LogLevel::FATAL;
	}

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
	tracer::Log(std::string(*Nan::Utf8String(info[0])), (LogLevel)info[1]->IntegerValue(),std::string( *Nan::Utf8String(info[2])));
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