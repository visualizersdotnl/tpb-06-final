#include "stdafx.h"
#include "events.h"


Events::Events()
{
}

Events::~Events()
{
}

void Events::Add(Event *pEvent)
{
	events.push_back(pEvent);
}


void Events::Fire(void * pSender)
{
	for (int i=0; i<(int)events.size(); ++i)
		events[i]->Fire(pSender);	
}