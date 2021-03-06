#ifndef BRIG_QOBJECT_WRAP_H
#define BRIG_QOBJECT_WRAP_H

#include <node.h>
#include <QObject>
#include <QVector>
#include "brig.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	class QObjectWrap : public ObjectWrap {

		public:
			QObjectWrap();
			QObjectWrap(QObject *object);
			~QObjectWrap();

			static Persistent<Function> constructor;
			static void Initialize(Handle<Object> target);
			static Handle<Value> NewInstance(QObject *object);

			QObject *GetObject() const { return obj; };

		private:

			static Handle<Value> New(const Arguments& args);

			/* Methods */
			static Handle<Value> setParent(const Arguments& args);
			static Handle<Value> toQuickWindow(const Arguments& args);
			static Handle<Value> toQuickItem(const Arguments& args);
			static Handle<Value> getProperty(const Arguments& args);
			static Handle<Value> setProperty(const Arguments& args);
			static Handle<Value> getPropertyNames(const Arguments& args);
			static Handle<Value> getMethods(const Arguments& args);
			static Handle<Value> emitEvent(const Arguments& args);
			static Handle<Value> invokeMethod(const Arguments& args);

			QObject *obj;
			SignalHandler *signalHandler;
	};

}

#endif
