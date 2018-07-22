//buffer_proc.cpp
#include <string.h>
#include <stdlib.h>
#include "buffer_proc.h"

using namespace SYS;
BufferProc::BufferProc()
{
    buffer_map_A_ptr = NULL;
    buffer_map_B_ptr = NULL;
    
    buffer_map_producer_ptr = NULL;
    buffer_map_consumer_ptr = NULL;	
}

BufferProc::~BufferProc()
{
	if (buffer_map_A_ptr != NULL)
	{
		delete buffer_map_A_ptr;
		buffer_map_A_ptr = NULL;
	}
	if (buffer_map_B_ptr != NULL)
	{
		delete buffer_map_B_ptr;
		buffer_map_B_ptr = NULL;
	}
	buffer_map_producer_ptr = NULL;
	buffer_map_consumer_ptr = NULL;
	
}

int BufferProc::init()
{
    try{
        buffer_map_A_ptr = new RuleEventCountMap();
        buffer_map_B_ptr = new RuleEventCountMap();
        
        if (NULL == buffer_map_A_ptr || NULL == buffer_map_B_ptr)
        {     
            return -1;
        }
    }
    catch(...)
    {
        if (buffer_map_A_ptr)
        {
            delete buffer_map_A_ptr;
            buffer_map_A_ptr = NULL;          
        }
        if (buffer_map_B_ptr)
        {
            delete buffer_map_B_ptr;
            buffer_map_B_ptr = NULL; 
        }
        
        buffer_map_producer_ptr = NULL;
        buffer_map_consumer_ptr = NULL;
        
        return -1;
        
    }
    
    buffer_map_producer_ptr = buffer_map_A_ptr;
    buffer_map_consumer_ptr = buffer_map_B_ptr;	
   
    return 0;
}

RuleEventCountMap * BufferProc::get()
{
    RuleEventCountMap *tmp = NULL;

    MutexLockGuard lock(buffer_map_mutex);
    if (NULL == buffer_map_producer_ptr || NULL == buffer_map_consumer_ptr)
    {   
        return NULL;
    }

    tmp = buffer_map_producer_ptr;
    buffer_map_producer_ptr = buffer_map_consumer_ptr;
    buffer_map_consumer_ptr = tmp;       

    return buffer_map_consumer_ptr;
}

int BufferProc::set(int key, int field_key, int field_value)
{
	EventCountMap map_upgrade_event_count;
	RuleEventCountMap map_rule_upgrade_event_count;
    RuleEventCountMap::iterator mul_iterator;
    EventCountMap::iterator iterator;
	std::pair<RuleEventCountMap::iterator,bool> ret;
    
	MutexLockGuard lock(buffer_map_mutex);

    if (NULL == buffer_map_producer_ptr)
    {         
        return -1;
    }
    
    mul_iterator = buffer_map_producer_ptr->find(key);
    if (mul_iterator != buffer_map_producer_ptr->end())
	{
	    iterator = mul_iterator->second.find(field_key);
		if (iterator != mul_iterator->second.end())
		{
		    mul_iterator->second[field_key] = iterator->second + field_value;
		}
		else
		{
			mul_iterator->second[field_key] = field_value;
		}
	}
	else
	{
	    map_upgrade_event_count[field_key] = field_value;
        ret = buffer_map_producer_ptr->insert(make_pair(key, map_upgrade_event_count));
        if (ret.second == false)
        {  
            return -1;
        }
	}
    
	return 0;	
}

void BufferProc::clear()
{
    MutexLockGuard lock(buffer_map_mutex);
    if (buffer_map_consumer_ptr != NULL)
    {
        buffer_map_consumer_ptr->clear();
    }
}

