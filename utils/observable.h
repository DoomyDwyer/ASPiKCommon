#pragma once
#include <vector>

class Observer;
/**
\class Observable
\ingroup Utilities
\brief
An implementation of the Observable, or 'Subject' participant in the Observer Pattern (GoF p293, 'Behaviorial Patterns').

Intent:
Define a one-to-many dependency between objects so that when one object changes state,
all its dependencies are notified and updated accordingly

\author Steve Dwyer
\remark This object is used in a number of plugins to notify a plugin when an Envelope Detector's detected value has moved above or below the configurable threshold.
\version Revision : 1.0
\date Date : 2022/ 05 / 08
*/
class Observable
{
public:
    Observable(); /* C-TOR */
    virtual ~Observable(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    Observable(const Observable&) = delete;
    Observable& operator=(const Observable&) = delete;

    // Suppress generation of move constructor and move assignment operator
    Observable(const Observable&&) = delete;
    Observable& operator=(const Observable&&) = delete;

    /** Attach an Observer object */
    /**
    \param observer the object to attach as an Observer to the Observable
    */
    void attach(Observer* observer);

    /** Detach an Observer object */
    /**
    \param observer the Observer object to be detached from the Observable
    */
    void detach(Observer* observer);

    /** Notify Observers of a change of state in the Observable */
    void notify(Observable* subject);

private:
    std::vector<Observer*> observers;
};