#ifndef OBJECTREGISTRY_H
#define OBJECTREGISTRY_H

#include <qobject.h>
#include <qmap.h>
#include <qmutex.h>

namespace VisionLive
{

class ObjectRegistry: public QObject
{
	Q_OBJECT

public:
	ObjectRegistry(); // Class constructor
	~ObjectRegistry(); // Class destructor

	void registerChildrenObjects(QObject *object, const QString &path = QString(), const QString &objectNamePrefix = QString()); // Register child objects of specyfied object (recursively)
	void registerObject(const QString &objectName, QObject *object, const QString &path); // Registers object
	void deregisterObject(const QString &name); // Deregisters object

	unsigned int size() const { return _objects.size(); } // Returns number of registered objects
	QMap<QString, std::pair<QObject *, QString> >::const_iterator find(const QString &name) const { return _objects.find(name); } // Returns const_iterator to object of name name
	QMap<QString, std::pair<QObject *, QString> >::const_iterator begin() const { return _objects.begin(); } // Returns const_iterator to begin of registered objects map
	QMap<QString, std::pair<QObject *, QString> >::const_iterator end() const { return _objects.end(); } // Returns const_iterator to end of registered objects map
	QObject *getObject(unsigned int i) const { return (*(_objects.begin()+i)).first; } // Returns poiter to object of index i
	const QString &getObjectName(unsigned int i) const { return (_objects.begin()+i).key(); } // Returns name of the object of index i
	const QString getObjectName(QObject *object) const; // Returns name of the object object
	const QString &getObjectPath(unsigned int i) const { return (*(_objects.begin()+i)).second; } // Returns path of object of index i

private:
	QMutex _mutex; // Synchronizes usage of ObjectRegistry by mutiple threads
	QMap<QString, std::pair<QObject *, QString> > _objects; // Map containing all registered objects

signals:
	void logError(const QString &error, const QString &src); // Requests TabLogview to log error
	void logWarning(const QString &warning, const QString &src); // Requests TabLogview to log warning
	void logMessage(const QString &message, const QString &src); // Requests TabLogview to log message
	void logAction(const QString &action); // Requests TabLogview to log action

	void objectRegistered(const QString &name, const QString &path); // Indicates object being registered
	void objectDeregistered(const QString &name); // Indicates object being deregistered

};

} // namespace VisionLive

#endif // OBJECTREGISTRY_H
