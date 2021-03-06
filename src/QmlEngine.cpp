#include <unistd.h>
#include <node.h>
#include <QtGui>
#include <QObject>
#include "QmlEngine.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	Persistent<Function> QmlEngineWrap::constructor;

	QmlEngineWrap::QmlEngineWrap() : ObjectWrap()
	{
		obj = new QQmlEngine();
		obj->setOutputWarningsToStandardError(false);
		signal = new SignalHandler(obj);
	}

	QmlEngineWrap::~QmlEngineWrap()
	{
printf("RELEASE ENGINE\n");
		delete signal;
		delete obj;
	}

	void QmlEngineWrap::Initialize(Handle<Object> target)
	{
		NanScope();

		Local<String> name = NanNew("QmlEngine");

		/* Constructor template */
		Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(QmlEngineWrap::New);
		tpl->InstanceTemplate()->SetInternalFieldCount(1);  
		tpl->SetClassName(name);

		/* Prototype */
		NODE_SET_PROTOTYPE_METHOD(tpl, "on", QmlEngineWrap::on);
//		NODE_SET_PROTOTYPE_METHOD(tpl, "rootContext", QmlEngineWrap::rootContext);

		//constructor = Persistent<Function>::New(tpl->GetFunction());
		NanAssignPersistent(constructor, tpl->GetFunction());

		target->Set(name, NanNew(constructor));
	}

	NAN_METHOD(QmlEngineWrap::New) {
		NanScope();

		QmlEngineWrap *obj_wrap = new QmlEngineWrap();
		obj_wrap->Wrap(args.This());

		NanReturnThis();
	}

	NAN_METHOD(QmlEngineWrap::on) {
		NanScope();

		QmlEngineWrap *obj_wrap = ObjectWrap::Unwrap<QmlEngineWrap>(args.This());

		// Signal name
		String::Utf8Value url(args[0]->ToString());

		int id = obj_wrap->signal->addCallback(*url, args[1]);

		NanReturnThis();
	}
/*
	Handle<Value> QmlEngineWrap::rootContext(const Arguments& args)
	{
		HandleScope scope;

		QmlEngineWrap *obj_wrap = ObjectWrap::Unwrap<QmlEngineWrap>(args.This());

		QQmlEngine *engine = obj_wrap->GetObject();
		QQmlContext *context = engine->rootContext();

		Handle<Value> instance = QmlContextWrap::NewInstance(context);

		return scope.Close(instance);
	}
*/
}
