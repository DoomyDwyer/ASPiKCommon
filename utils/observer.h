#pragma once
#include "observable.h"

/**
\class Observer
\ingroup Utilities
\brief
The interface defining the Observer participant in the Observer Pattern (GoF p293, 'Behaviorial Patterns').

Intent:
Define a one-to-many dependency between objects so that when one object changes state,
all its dependencies are notified and updated accordingly

\author Steve Dwyer
\remark This object is used in a number of plugins to receive notifications when an Envelope Detector's detected value has moved above or below the configurable threshold.
\version Revision : 1.0
\date Date : 2022/ 05 / 08
*/
class Observer
{
public:
    Observer(); /* C-TOR */
    virtual ~Observer(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    Observer(const Observer&) = delete;
    Observer& operator=(const Observer&) = delete;

    // Suppress generation of move constructor and move assignment operator
    Observer(const Observer&&) = delete;
    Observer& operator=(const Observer&&) = delete;

    /** This method is invoked when an Observable object, to which this Observer is attached, publishes a state change update */
    virtual void update(Observable* observable);
};