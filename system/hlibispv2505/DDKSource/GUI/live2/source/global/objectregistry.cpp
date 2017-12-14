#include "objectregistry.hpp"
#include "algorithms.hpp"

//
// Public Functions
//

VisionLive::ObjectRegistry::ObjectRegistry()
{
}

VisionLive::ObjectRegistry::~ObjectRegistry()
{
}

void VisionLive::ObjectRegistry::registerChildrenObjects(QObject *object, const QString &path, const QString &objectNamePrefix)
{
	QMap<QString, std::pair<QObject *, QString> > tmp = Algorithms::getChildrenObjects(object, path, objectNamePrefix);
	for(QMap<QString, std::pair<QObject *, QString> >::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		registerObject(it.key(), (*it).first, (*it).second);
	}
}

void VisionLive::ObjectRegistry::registerObject(const QString &objectName, QObject *object, const QString &path)
{
	_mutex.lock();

	emit logMessage("Registering object - " + path + objectName, tr("ObjectRegistry::registerObject()"));
	if(_objects.find(objectName) == _objects.end())
	{
		_objects.insert(objectName, std::make_pair(object, path));
		emit objectRegistered(objectName, path);
	}
	else
	{
		emit logWarning("Couldn't register object - " + path + objectName, tr("ObjectRegistry::registerObject()"));
	}

	_mutex.unlock();
}

void VisionLive::ObjectRegistry::deregisterObject(const QString &name)
{
	_mutex.lock();

	QMap<QString, std::pair<QObject *, QString> >::iterator it = _objects.find(name);
	if(it != _objects.end())
	{		
		QMap<QString, std::pair<QObject *, QString> > tmp = Algorithms::getChildrenObjects((*it).first, QString(), name);
		QMap<QString, std::pair<QObject *, QString> >::iterator it2;
		for(it2 = tmp.begin(); it2 != tmp.end(); it2++)
		{
			it = _objects.find(it2.key());
			if(it != _objects.end())
			{
				emit logMessage("Deregistering object - " + (*it2).second + it2.key(), tr("ObjectRegistry::registerObject()"));
				_objects.erase(it);
			}
			else
			{
				emit logWarning("Couldn't deregister object - " + (*it2).second + it2.key(), tr("ObjectRegistry::deregisterObject()"));
			}
		}

		emit objectDeregistered(name);
	}
	else
	{
		emit logWarning("Couldn't deregister object - " + name, tr("ObjectRegistry::deregisterObject()"));
	}

	_mutex.unlock();
}

const QString VisionLive::ObjectRegistry::getObjectName(QObject *object) const 
{
	for(QMap<QString, std::pair<QObject *, QString> >::const_iterator it = _objects.begin(); it != _objects.end(); it++)
	{
		if((*it).first == object) return it.key();
	}

	return QString();
}