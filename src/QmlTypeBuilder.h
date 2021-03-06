#ifndef BRIG_QMLTYPEBUILDER_H
#define BRIG_QMLTYPEBUILDER_H

#include <node.h>
#include "brig.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	class QmlTypeBuilder : public ObjectWrap {

		public:
			QmlTypeBuilder(const char *typeName);
			~QmlTypeBuilder();

			static Persistent<Function> constructor;
			static void Initialize(Handle<Object> target);

		private:

			static NAN_METHOD(New);

			/* Methods */
			static NAN_METHOD(build);
			static NAN_METHOD(addMethod);
			static NAN_METHOD(addSignal);

			QObject *obj;
			unsigned long counter;
			DynamicQMetaObjectBuilder *metaobject_builder;
			QMap<unsigned long, QObject *> qml_types;
	};

}

#endif
