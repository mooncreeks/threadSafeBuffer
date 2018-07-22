//buffer_proc.h
#ifndef BUFFER_PROC_H
#define BUFFER_PROC_H

#include <map>
#include "mutex.h"

typedef std::map<int,int> EventCountMap;
typedef std::map<int,EventCountMap> RuleEventCountMap;

class BufferProc
{
public:
	BufferProc();
	~BufferProc();
	int init();
	RuleEventCountMap * get();
	int set(int key, int field_key, int field_value);
  void clear();
	
private:
  MutexLock buffer_map_mutex;
	RuleEventCountMap *buffer_map_A_ptr;
	RuleEventCountMap *buffer_map_B_ptr;
	RuleEventCountMap *buffer_map_producer_ptr;
	RuleEventCountMap *buffer_map_consumer_ptr;	
};

extern BufferProc *g_event_count;

#endif  /* BUFFER_PROC_H */

