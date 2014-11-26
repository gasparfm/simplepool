#include <iostream>
#include "simplepool.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <cstdlib>

unsigned testid = 100;
int usos = 0;
std::mutex usosmutex;

class Test
{
public:
  Test():id (testid++)
  {
    std::cout << "Creo un objeto" << std::endl;
  }

  virtual ~Test()
  {
  }

  void uso()
  {
    usosmutex.lock();
    usos++;
    usosmutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds((std::rand()%15)*80));
  }

private:
  unsigned id;
};

int main ()
{
  SimplePool<Test> p(200);
  int nth = 0;
      //      std::terminate();

      for (unsigned j=0; j<600; ++j)
	{
	  for (unsigned i=0; i<160; ++i)
	    {
	      nth++;
	      std::thread([&](){
		  auto r = p.getResource();
		  if (r==NULL)
		    {
		      std::cout << "ESTO NO TA BIEN"<<std::endl;
		      std::terminate();
		    }
		  r->obj->uso();
		  r->free();
		  //	      p.listResources();
		}).detach();
	    }
	  while (p.usedObjects()>150);
	}
  unsigned luo = p.usedObjects();
  for (unsigned i=0; i<100; ++i)
    {
      std::cout << "." << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
      if (luo==p.usedObjects())
	continue;
      std::cout<<std::endl;
      luo = p.usedObjects();
      p.listResources();
      usosmutex.lock();
      std::cout << "Usos: "<<usos<<" Recursos usados: "<<p.usedObjects()<<" THREADS: "<<nth<<std::endl;
      if (usos==600*160)
	break;
      usosmutex.unlock();
    }

  std::cout <<" YA NO TE DIGO MAS NA"<<std::endl;
  //  std::this_thread::sleep_for(std::chrono::milliseconds(100000));
  std::cout << "FIN" << std::endl;
}
