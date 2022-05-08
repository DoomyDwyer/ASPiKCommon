#include "observer.h"

Observable::Observable() = default;

Observable::~Observable()
{
    for (Observer* observer : observers)
     {
         detach(observer);
     }
}

void Observable::attach(Observer* observer)
{
    observers.push_back(observer);
}

void Observable::detach(Observer* observer)
{
    remove(observers.begin(), observers.end(), observer);
}

void Observable::notify(Observable* subject)
{
    for (Observer* observer : observers)
     {
         observer->update(subject);
     }
}
