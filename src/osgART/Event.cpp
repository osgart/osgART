/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "osgART/Event"


namespace osgART {

Event::Event()
    : mHandled(false)
    , mTick(0)
    , mId(0)
{
    // reset
    mTick = osg::Timer::instance()->tick();
}

Event::Event(const Event& e)
{
    *this = e;
}

Event&
Event::operator = (const Event& e) {
    if (this != &e) {
        mHandled = e.mHandled;
        mTick = e.mTick;
        mId = e.mId;
    }

    return *this;
}

Event::~Event()
{
}


//

bool
EventHandler::process(Event& e)
{
    for (EventProxyArray::iterator it = mHandlers.begin();
         it != mHandlers.end() && !e.getHandled();
         ++it)
    {
        if ((*it)->corresponds(e)) {
            (*it)->call(this,e);
        }
    }

    return e.getHandled();
}

void EventHandler::removeAll()
{
    for (EventProxyArray::iterator it = mHandlers.begin();
         it != mHandlers.end();
         ++it)
    {
        delete (*it);
    }
    mHandlers.clear();
}


EventHandler::~EventHandler()
{
    removeAll();
}

}




/**
 * taken from TacitPixel (tacitpixel.sourceforge.net)
 *
 */


#if 0


/*
 * Copyright (C) 1999-2011 Hartmut Seichter
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef TP_EVENT_H
#define TP_EVENT_H

#include <tp/referenced.h>
#include <tp/rtti.h>
#include <tp/array.h>
#include <tp/timer.h>

#include <tp/globals.h>
#include <tp/types.h>

/** forward declaration */
class tpEventHandler;


/**
  * Base class for event handling
  *
  */
class TP_API tpEvent {
protected:

    tpReferenced* mSender;
    bool mHandled;
    tpTimerTick mTick;

    tpULong mId;

public:

    TP_TYPE_DECLARE

    /** Default c'tor */
    tpEvent();

    /** Copy c'tor */
    tpEvent(const tpEvent& e);

    /** Assignment operator */
    tpEvent& operator = (const tpEvent& e);

    /** D'tor */
    virtual ~tpEvent();

    void setId(tpULong id) { mId = id; }
    tpULong getId() { return mId; }

    inline void
    setHandled(bool handled = true) { mHandled = handled; }

    inline bool
    getHandled() const { return mHandled; }

};


struct TP_API tpEventProxy {
    virtual void call(tpEventHandler*, tpEvent&) = 0;
    virtual bool corresponds(tpEvent& e) const = 0;

    virtual ~tpEventProxy() {}
};

template <typename EventType>
struct tpEventFunctionProxy : tpEventProxy {

    void (*mHandler)(EventType& e);

    tpEventFunctionProxy(void (*f)(EventType& e)) : mHandler(f) {}

    virtual void call(tpEventHandler*, tpEvent& e) {
        mHandler(static_cast<EventType&>(e));
    }

    virtual bool corresponds(tpEvent &e) const { return (e.getType() == EventType::getTypeInfo()); }

};


template <typename EventType,typename EventClass>
struct tpEventFunctorProxy : tpEventProxy {

    typedef void (EventClass::*EventCallback)(EventType&);

    EventCallback mHandler;
    EventClass* mObject;

    inline
    tpEventFunctorProxy(EventCallback method,EventClass* o)
        : mHandler(method)
        , mObject(o)
    {}

    virtual void call(tpEventHandler* h, tpEvent& e) {
        (mObject->*mHandler)(static_cast<EventType&>(e));
    }

    virtual bool corresponds(tpEvent &e) const { return (e.getType() == EventType::getTypeInfo()); }

};


class TP_API tpEventHandler {
protected:

    typedef tpArray<tpEventProxy*> tpEventProxyArray;
    tpEventProxyArray mHandlers;

public:

    template <typename EventType>
    void attach(void (*f)(EventType& e))
    {
        mHandlers.add(new tpEventFunctionProxy<EventType>(f));
    }

    template <typename EventType,typename EventClass>
    void attach(EventClass* o,void (EventClass::*f)(EventType& e))
    {
        mHandlers.add(new tpEventFunctorProxy<EventType,EventClass>(f,o));
    }

    bool process(tpEvent &e);

    void removeAll();

    ~tpEventHandler();
};


#endif



tpEvent::tpEvent()
    : mSender(0)
    , mHandled(false)
    , mTick(0)
    , mId(0)
{
    tpTimer::getCurrentTick(mTick);
}

tpEvent::tpEvent(const tpEvent& e) {
    *this = e;
}

tpEvent&
tpEvent::operator = (const tpEvent& e) {
    if (this != &e) {
        mSender = e.mSender;
        mHandled = e.mHandled;
        mTick = e.mTick;
        mId = e.mId;
    }

    return *this;
}

tpEvent::~tpEvent() {

}



bool
tpEventHandler::process(tpEvent& e)
{
    for (tpEventProxyArray::iterator it = mHandlers.begin();
         it != mHandlers.end() && !e.getHandled();
         ++it)
    {
        if ((*it)->corresponds(e)) {
            (*it)->call(this,e);
        }
    }

    return e.getHandled();
}

void tpEventHandler::removeAll()
{
    for (tpEventProxyArray::iterator it = mHandlers.begin();
         it != mHandlers.end();
         ++it)
    {
        delete (*it);
    }
    mHandlers.clear();
}


tpEventHandler::~tpEventHandler()
{
    removeAll();
}


TP_TYPE_REGISTER_BASE(tpEvent,Event);

#endif
