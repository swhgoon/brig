#include <unistd.h>
#include <node.h>
#include <QtGui>
#include <QObject>
#include "QuickItem.h"

namespace Brig {

	using namespace v8;
	using namespace node;

	Persistent<Function> QuickItem::constructor;

	QuickItem::QuickItem() : ObjectWrap()
	{
		obj = NULL;
		signal = NULL;
	}

	QuickItem::~QuickItem()
	{
printf("RELEASE QuickItem\n");
		delete signal;
		delete obj;
	}

	void QuickItem::Initialize(Handle<Object> target)
	{
		NanScope();

		Local<String> name = NanNew("QuickItem");

		/* Constructor template */
		Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(QuickItem::New);
		tpl->InstanceTemplate()->SetInternalFieldCount(1);  
		tpl->SetClassName(name);

		/* Prototype */
		NODE_SET_PROTOTYPE_METHOD(tpl, "create", QuickItem::create);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getPropertyNames", QuickItem::getPropertyNames);
		NODE_SET_PROTOTYPE_METHOD(tpl, "getProperty", QuickItem::getProperty);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setProperty", QuickItem::setProperty);
		NODE_SET_PROTOTYPE_METHOD(tpl, "setParent", QuickItem::setParent);
		NODE_SET_PROTOTYPE_METHOD(tpl, "invokeMethod", QuickItem::invokeMethod);
		NODE_SET_PROTOTYPE_METHOD(tpl, "emitEvent", QuickItem::emitEvent);
		NODE_SET_PROTOTYPE_METHOD(tpl, "on", QuickItem::on);

		NanAssignPersistent(constructor, tpl->GetFunction());

		target->Set(name, NanNew(constructor));
	}

	// Prototype Constructor
	NAN_METHOD(QuickItem::New) {
		NanScope();

		QuickItem *obj_wrap = new QuickItem();
		obj_wrap->Wrap(args.This());

		NanReturnThis();
	}

	// Method
	NAN_METHOD(QuickItem::create) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		QmlComponent *component = ObjectWrap::Unwrap<QmlComponent>(args[0]->ToObject());

		// Create QuickItem with component
		obj_wrap->obj = static_cast<QQuickItem *>(component->GetObject()->create());
		obj_wrap->signal = new SignalHandler(qobject_cast<QObject *>(obj_wrap->GetObject()));
//		obj_wrap->signal->setObject(qobject_cast<QObject *>(obj_wrap->GetObject()));

		NanReturnUndefined();
	}

	NAN_METHOD(QuickItem::getPropertyNames) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		Handle<Array> keys = NanNew<Array>();

		// Getting property names
		static const QMetaObject *meta = obj_wrap->GetObject()->metaObject();
		for (int i = 0; i < meta->propertyCount(); i++) {
			keys->Set(i, NanNew(QString(meta->property(i).name()).toUtf8().constData()));
		}

		NanReturnValue(keys);
	}

	NAN_METHOD(QuickItem::getProperty) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		if (!args[0]->IsString())
			NanThrowTypeError("First argument must be a string");

		String::Utf8Value name(args[0]->ToString());

		// Get property
		QVariant v = obj_wrap->GetObject()->property(*name);

		// Convert Qvariant to V8 data type
		if (v.isNull())
			NanReturnNull();

		NanReturnValue(Utils::QVariantToV8(v.userType(), v));
	}

	NAN_METHOD(QuickItem::setProperty) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		if (!args[0]->IsString())
			NanThrowTypeError("First argument must be a string");

		String::Utf8Value name(args[0]->ToString());
		Handle<Value> value(args[1]);
		QVariant v;

		// Check data type
		if (value->IsTrue() || value->IsFalse() || value->IsBoolean() ) {
			v.setValue(QVariant(value->ToBoolean()->Value()));
		} else if (value->IsNumber()) {
			v.setValue(QVariant(value->NumberValue()));
		} else if (value->IsInt32()) {
			v.setValue(QVariant(value->ToInt32()->Value()));
		} else if (value->IsString()) {
			String::Utf8Value _v(value->ToString());
			v.setValue(QVariant(static_cast<char *>(*_v)));
		}

		// Set property
		obj_wrap->GetObject()->setProperty(*name, v);

		NanReturnUndefined();
	}

	NAN_METHOD(QuickItem::setParent) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		QuickItem *item = ObjectWrap::Unwrap<QuickItem>(args[0]->ToObject());

		if (!item->GetObject())
			NanReturnUndefined();

		QQuickItem *parentObj = NULL;
		if (item->GetObject()->isWindowType()) {
			// It's a window, so we need to get inside content item
			parentObj = qobject_cast<QQuickWindow *>(item->GetObject())->contentItem();
		} else {
			parentObj = item->GetObject();
		}

		obj_wrap->GetObject()->setParentItem(parentObj);

		NanReturnUndefined();
	}

	NAN_METHOD(QuickItem::invokeMethod) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());
		QObject *qobj = qobject_cast<QObject *>(obj_wrap->GetObject());

		if (!args[0]->IsString())
			NanThrowTypeError("First argument must be a string");

		// Method name
		String::Utf8Value methodSig(args[0]->ToString());

		QVariant returnedValue;
		int argsLen = args.Length() - 1;

		// It supports only 10 arguments with limitation of Qt
		QMetaObject::invokeMethod(qobj, *methodSig,
			Qt::AutoConnection,
			Q_RETURN_ARG(QVariant, returnedValue),
			(argsLen > 0) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[1])) : QGenericArgument(),
			(argsLen > 1) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[2])) : QGenericArgument(),
			(argsLen > 2) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[3])) : QGenericArgument(),
			(argsLen > 3) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[4])) : QGenericArgument(),
			(argsLen > 4) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[5])) : QGenericArgument(),
			(argsLen > 5) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[6])) : QGenericArgument(),
			(argsLen > 6) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[7])) : QGenericArgument(),
			(argsLen > 7) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[8])) : QGenericArgument(),
			(argsLen > 8) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[8])) : QGenericArgument(),
			(argsLen > 9) ? Q_ARG(QVariant, Utils::V8ToQVariant(args[9])) : QGenericArgument());

		// Convert Qvariant to V8 data type
		if (returnedValue.isNull())
			NanReturnNull();

		switch(returnedValue.userType()) {
		case QMetaType::Bool:
			NanReturnValue(NanNew<Boolean>(returnedValue.toBool()));
		case QMetaType::Int:
			NanReturnValue(NanNew<Number>(returnedValue.toInt()));
		case QMetaType::UInt:
			NanReturnValue(NanNew<Number>(returnedValue.toUInt()));
		case QMetaType::Float:
			NanReturnValue(NanNew<Number>(returnedValue.toFloat()));
		case QMetaType::Double:
			NanReturnValue(NanNew<Number>(returnedValue.toDouble()));
		case QMetaType::LongLong:
			NanReturnValue(NanNew<Number>(returnedValue.toLongLong()));
		case QMetaType::ULongLong:
			NanReturnValue(NanNew<Number>(returnedValue.toULongLong()));
		case QMetaType::QString:
			NanReturnValue(NanNew(returnedValue.toString().toUtf8().constData()));
		case QMetaType::QColor:
			NanReturnValue(NanNew(returnedValue.value<QColor>().name(QColor::HexArgb).toUtf8().constData()));
		}

		NanReturnUndefined();
	}

	NAN_METHOD(QuickItem::emitEvent) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());
		QObject *qobj = qobject_cast<QObject *>(obj_wrap->GetObject());

		if (!args[0]->IsString())
			NanThrowTypeError("First argument must be a string");

		// Method name
		String::Utf8Value methodSig(args[0]->ToString());

		QVariant returnedValue;
		int argsLen = args.Length() - 1;

		static const QMetaObject *meta = qobj->metaObject();
//		int methodIndex = meta->indexOfMethod(*methodSig);
//		QMetaMethod method = meta->method(methodIndex);
//
		// Getting currect context
		QQmlContext *thisContext = QQmlEngine::contextForObject(qobj);

		// Preparing parameter and ensure QJSValue have individual memory
//		QJSValue val[argsLen];
		QMap<int, QJSValue> vals;
		QList<QGenericArgument> parameters;
		QList<Utils::ParamData> dataList;

		// Getting signature
		for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
			QMetaMethod method = meta->method(i);
			const char *methodName = method.name().data();
			if (strcmp(*methodSig, methodName) != 0)
				continue;

			if (!method.isValid())
				break;

			// Getting parameter types
			for (int j = 0; j < method.parameterCount(); j++) {
				int type = method.parameterType(j);

				if (j >= args.Length())
					break;

				Handle<Value> value = args[j + 1];

				// Type is "var" in QML, which is different from QVariant
				if (type == qMetaTypeId<QJSValue>()) {
					vals.insert(j, Utils::V8ToQJSValue(thisContext->engine(), value));
					//val[j] = Utils::V8ToQJSValue(thisContext->engine(), value);

					// Undefined
					//parameters << QGenericArgument("QJSValue", static_cast<const void *>(&val[j]));
					parameters << QGenericArgument("QJSValue", static_cast<const void *>(&vals[j]));
					continue;
				}

				// Making arguments
				Utils::ParamData *data = Utils::MakeParameter(type, value);
				if (data == NULL) {
					// Unknown type, set Undefined to this parameter
					parameters << QGenericArgument();
					continue;
				}

				dataList << data;
				parameters << QGenericArgument(QMetaType::typeName(type), data->ptr);
			}

			// Invoke
			method.invoke(qobj,
				Qt::QueuedConnection,
				(argsLen > 0) ? parameters[0] : QGenericArgument(),
				(argsLen > 1) ? parameters[1] : QGenericArgument(),
				(argsLen > 2) ? parameters[2] : QGenericArgument(),
				(argsLen > 3) ? parameters[3] : QGenericArgument(),
				(argsLen > 4) ? parameters[4] : QGenericArgument(),
				(argsLen > 5) ? parameters[5] : QGenericArgument(),
				(argsLen > 6) ? parameters[6] : QGenericArgument(),
				(argsLen > 7) ? parameters[7] : QGenericArgument(),
				(argsLen > 8) ? parameters[8] : QGenericArgument(),
				(argsLen > 9) ? parameters[9] : QGenericArgument());

			// Release
			vals.clear();
			dataList.clear();
			parameters.clear();

			//NanReturnValue(NanNew<Boolean>(True));
			NanReturnValue(NanTrue());
		}

		NanReturnValue(NanFalse());
	}

	NAN_METHOD(QuickItem::on) {
		NanScope();

		QuickItem *obj_wrap = ObjectWrap::Unwrap<QuickItem>(args.This());

		// Signal name
		String::Utf8Value url(args[0]->ToString());
		int id = obj_wrap->signal->addCallback(*url, args[1]);

		NanReturnThis();
	}
}
