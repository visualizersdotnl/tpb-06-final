#pragma once


#include <vector>

class Event
{
public:
	virtual void Fire(void * pSender) = 0;
};


class Events
{
private:
	std::vector<Event*> events; // only references!

public:
	Events();
	virtual ~Events();

	void Add(Event* pEvent);

	void Fire(void * pSender);
};