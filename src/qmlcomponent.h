#ifndef BRIG_QMLCOMPONENT_WRAP_H
#define BRIG_QMLCOMPONENT_WRAP_H

#include <node.h>
#include <QQmlComponent>
#include "brig.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	class QmlComponentWrap : public ObjectWrap {

		public:
			QmlComponentWrap(QmlEngineWrap *);
			QmlComponentWrap(QmlEngineWrap *, Local<Value>);
			~QmlComponentWrap();

			static Persistent<Function> constructor;
			static void Initialize(Handle<Object> target);

			QQmlComponent *GetObject() const { return obj; };
			Handle<Value> GetPrototypeObject() const { return prototype_object; };

		private:

			static Handle<Value> New(const Arguments& args);

			/* Methods */
			static Handle<Value> create(const Arguments& args);
			static Handle<Value> setData(const Arguments& args);
			static Handle<Value> isReady(const Arguments& args);
			static Handle<Value> toObject(const Arguments& args);

			Handle<Value> prototype_object;
			QQmlComponent *obj;
	};

}

#endif
