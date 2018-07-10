#pragma once
#include <vector>
#include <unordered_set>
#include "SymbolListMapEntry.h"
#include <time.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "Ema.h"

void sleep(int millisecs)
{
#if defined WIN32
	::Sleep((DWORD)(millisecs));
#else
	struct timespec sleeptime;
	sleeptime.tv_sec = millisecs / 1000;
	sleeptime.tv_nsec = (millisecs % 1000) * 1000000;
	nanosleep(&sleeptime, 0);
#endif
}

class NiProvider {
public:
	//Initialize the NiProvider's variables
	NiProvider();
	//Uninitialize the NiProvider's variables
	~NiProvider();

	SymbolListMapEntry* generateNewEntry();
	void generateUpdatedEntries();
	
	//The List of SymbolListMapEntry represents the entries in an update message  
	std::vector<SymbolListMapEntry*>* updateMap;
private:
	//The maximum number of entries in the Symbol List
	static const int MAX_ENTRIES = 150;
	//The minimum number of entries in the Symbol List
	static const int MIN_ENTRIES = 20;
	//The maximum chance of the DELETE action is 10 
	//It the random number is 1-10, perform DELETE action 
	static const int deleteMaxChance = 10;
	//The maximum chance of the UPDATE action is 40 
	//It the random number is 11-40, perform UPDATE action 
	static const int updateMaxChance = 40;
	//The maximum chance of the ADD action is 100
	//It the random number is 41-100, perform ADD action 
	static const int addMaxChance = 100;
	//The List of SymbolListMapEntry represents all entries in the Symbol List from both Refresh and Update messages  
	std::vector<SymbolListMapEntry*>* symbolListMap;
	//The Set keeps the PROV_SYMB/Key in the current Symbol List
	//It is used to verify if a new generated PROV_SYMB/Key is duplicated with any one in the Set
	std::unordered_set<std::string>* currentKeys;
};

