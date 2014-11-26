/* @(#)simplepool.h
 */

#ifndef _SIMPLEPOOL_H
#define _SIMPLEPOOL_H 1

#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

template <typename T>
class SimplePool
{
 public:


  template <typename TT>
  struct SimplePoolObject
  {
    unsigned id;
    TT *obj;
    bool inUse;
    SimplePool *pool;

    SimplePoolObject (unsigned id, SimplePool *pool, TT *obj):id(id), pool(pool), obj(obj),inUse(false)
    {
    }

    void free()
    {
      inUse = false;
      /* std::cout << "SE QUIERE LIBERAR ID="<<id<<std::endl; */
      pool->unUseObject();
      /* std::cout << "Se ha liberado ID="<<id<<std::endl; */
    }

    bool getUse()
    {
      return inUse;
    }

    void setUse()
    {
      inUse = true;
    }

    std::string str()
    {
      return "ID: "+std::to_string(id)+": "+std::to_string(inUse);
    }
  };

 SimplePool(unsigned _maxObjects): _maxObjects(_maxObjects),_usedObjects(0)
    {
      createObjects();

      _objDataMutex.lock();
      _maxObjLock=false;
      /* _fullMutex.lock(); */
      _objDataMutex.unlock();
    }

  virtual ~SimplePool()
    {
    }

  SimplePoolObject<T>* getResource()
  {
	_fullMutex.lock();
	std::lock_guard<std::mutex> lock(_objDataMutex);
	bool cond2 = _maxObjLock;
	if (_usedObjects<_maxObjects-1)
	  _fullMutex.unlock();

    /* std::cout << "PASO EL MUTEX: "<<std::to_string(_usedObjects)<<"<"<<_maxObjects<<std::endl; */
    for (auto &o : _objects)
      {
	if (!o.getUse())
	  {
	    o.setUse();
	    ++_usedObjects;
	    if (_usedObjects==_maxObjects)
	      {
		_maxObjLock = true;
		/* std::cout << "BLOKEOOOOOO"<<std::endl; */
	      }

	    /* std::cout << "Se ha cogido el "<<o.id<<std::endl; */
	    return &o;
	  }
      }

    return NULL;
  }

  void unUseObject()
  {
    std::lock_guard<std::mutex> lock(_objDataMutex);

    if (this->_maxObjLock)
      {
	/* std::cout<< "QUITO UN BLOQUEO"<<std::endl; */
	_maxObjLock = false;
	this->_fullMutex.unlock();
      }

    this->_usedObjects--;
  }

  void listResources()
  {
    _objDataMutex.lock();

    for (auto &o : _objects)
      {
	std::cout << &o << "--"<< o.str()<< std::endl;
      }

    _objDataMutex.unlock();
  }

  unsigned usedObjects()
  {
    std::lock_guard<std::mutex> lock (_objDataMutex);
    return _usedObjects;
  }

 protected:
  void createObjects()
  {
    _objDataMutex.lock();

    for (unsigned i=0; i< _maxObjects; ++i)
      {
	_objects.push_back(SimplePoolObject<T>(i, this, new T()));
      }

    _objDataMutex.unlock();
  }

  unsigned _maxObjects;
  std::vector<SimplePoolObject<T> > _objects;
  unsigned _usedObjects;
  bool _maxObjLock;
  std::mutex _objDataMutex;
  std::mutex _fullMutex;
};

#endif /* _SIMPLEPOOL_H */

