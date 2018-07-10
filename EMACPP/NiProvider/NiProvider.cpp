#include "NiProvider.h"
#include <iostream>
#include <cstring>


using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;
using namespace std;

NiProvider::NiProvider() {
	symbolListMap = new std::vector<SymbolListMapEntry*>();
	updateMap = new std::vector<SymbolListMapEntry*>();
	currentKeys = new std::unordered_set<std::string>();
}

NiProvider::~NiProvider() {
	for (vector<SymbolListMapEntry*>::iterator it = symbolListMap->begin(); it != symbolListMap->end(); it++)
	{
		delete *it;
	}
	delete symbolListMap;
	delete updateMap;
	delete currentKeys;
}

//Generate a new SymbolListMapEntry which represents an entry.
SymbolListMapEntry* NiProvider::generateNewEntry()
{
	//Initialize a new SymbolListMapEntry
	SymbolListMapEntry* newEntry = new SymbolListMapEntry();
	//The variable to keep PROV_SYMB/Key
	std::string tmpKey;
	srand(time(NULL));
	//PROV_SYMB pattern is [A-Z][A-Z][A-Z].[A-Z]
	//Generate a PROV_SYMB/Key till it is not the same as any current PROV_SYMB/Key 
	do {
		tmpKey.clear();
		tmpKey += (char)(rand() % 26 + 65);
		tmpKey += (char)(rand() % 26 + 65);
		tmpKey += (char)(rand() % 26 + 65);
		tmpKey += '.';
		tmpKey += (char)(rand() % 26 + 65);
	} while (currentKeys->count(tmpKey));
	//Set the SymbolListMapEntry with ADD Action 
	newEntry->performAddAction(SymbolListMapEntry::getDefaultProdPerm(), tmpKey);
	//Add the SymbolListMapEntry in the Symbol List
	symbolListMap->push_back(newEntry);
	//Add the PROV_SYMB/Key of the SymbolListMapEntry in the PROV_SYMB/Key Set
	currentKeys->insert(newEntry->getProSymb());
	return newEntry;
}

//Generate the List of new entries which will be added to the next update message
void NiProvider::generateUpdatedEntries()
{
	//Create the PROV_SYMB/Key Set of the update message
	std::unordered_set<std::string>* updateEntries = new std::unordered_set<std::string>();
	//Generate a new ProdPerm which is 1000-5000 at random  
	int newProdPerm = rand() % 4000 + 1000;
	//The list contains maximum 5 entries, for each one do:
	for (int i = 0; i < 5; i++)
	{
		//Declare SymbolListMapEntry to keep a new entry
		SymbolListMapEntry* updateEntry = 0;
		//Select an Action at random
		int action = rand() % 100;
		//If the number of entries in the Symbol List is less than the minimum(20)
		//or the random Action is ADD
		if (symbolListMap->size() < MIN_ENTRIES || action > updateMaxChance)
		{
			//If the number of entries in the Symbol List is less than maximum(150)
			if (symbolListMap->size() < MAX_ENTRIES) {
				//Perform ADD action by generating a new entry with ADD action
				updateEntry = generateNewEntry();
			}
			//If the number of entries in the Symbol List equal or is more than the maximum(150),
			//try to delete an existing entry in the Symbol List
			else
			{
				//Select the deleted entry in the Symbol List at random
				int index = rand() % symbolListMap->size();
				updateEntry = (*symbolListMap)[index];
				//If the PROV_SYMB/Key of the selected entry is duplicated with 
				//any PROV_SYMB/Key in the update Set, skip to create the next entry
				if (updateEntries->count(updateEntry->getProSymb()))
				{
					continue;
				}
				//Otherwise, perform DELETE action on the selected entry
				updateEntry->performDeleteAction();
				//Remove the deleted entry from the Symbol List
				symbolListMap->erase(symbolListMap->begin() + index);
				//Remove the PROV_SYMB/Key of the deleted entry from the PROV_SYMB/Key Set
				currentKeys->erase(updateEntry->getProSymb());
			}
		}
		//If the random action is UPDATE
		else if (action > deleteMaxChance)
		{
			//Select the updated entry in the Symbol List at random
			int index = rand() % symbolListMap->size();
			updateEntry = (*symbolListMap)[index];
			//If the PROV_SYMB/Key of the selected entry is duplicated with 
			//any PROV_SYMB/Key in the update Set, skip to create the next entry
			if (updateEntries->count(updateEntry->getProSymb()))
			{
				continue;
			}
			//Otherwise, perform UPDATE action on the selected entry  
			updateEntry->performUpdateAction(newProdPerm);
		}
		//If the random action is DELETE
		else
		{
			//If the number of entries in the Symbol List equal or less than minimum(20)
			if (symbolListMap->size() <= MIN_ENTRIES) {
				//Perform ADD action by generating a new entry with ADD action
				updateEntry = generateNewEntry();
			}
			else
			{   //If the number of entries in the Symbol List is more than   the minimum(20),
				//try to delete an existing entry in the Symbol List
				//Select the deleted entry in the Symbol List at random
				int index = rand() % symbolListMap->size();
				updateEntry = (*symbolListMap)[index];
				//If the PROV_SYMB/Key of the selected entry is duplicated with 
				//any PROV_SYMB/Key in the update Set, skip to create the next entry
				if (updateEntries->count(updateEntry->getProSymb()))
				{
					continue;
				}
				//Otherwise, perform DELETE action on the selected entry
				updateEntry->performDeleteAction();
				//Remove the deleted entry from the Symbol List
				symbolListMap->erase(symbolListMap->begin() + index);
				//Remove the PROV_SYMB/Key of the deleted entry from the PROV_SYMB/Key Set
				currentKeys->erase(updateEntry->getProSymb());
			}
		}
		//Add the PROV_SYMB/Key in the Set of the new update message.
		updateEntries->insert(updateEntry->getProSymb());
		//Add the entry in the List of the new update message.
		updateMap->push_back(updateEntry);
	}
}

int main(int argc, char* argv[])
{
	try
	{
		//Use the default NiProvider's configuration in EmaConfig.xml found in the working directory
		//change "adhserver:14003" to your ADH server IP/name and its port
		//change "user" to NI Provider's user which logs in to ADH
		OmmProvider provider(OmmNiProviderConfig().host("192.168.27.53:14003").username("user"));


		cout<< "Non-Interactive Provider starts." << endl;
		//The handle which identifies item stream on which to send the Refresh message and Update messages of a Symbol List.
		long itemHandle = 5;
		//Create an object of NiProvider which provides the methods to create/update and manage Symbol List.
		NiProvider* niprov = new NiProvider();
		//Create a map which is encoded content of Symbol List. The map contains entries.
		Map map;
		//Create a field list which is encoded content of an entries.
		FieldList fieldList;
		//Encode a map contains 25 entries
		for (int i = 0; i < 25; i++)
		{
			//generate SymbolListMapEntry instance consisting of action(ADD), PROD_PERM field and PROV_SYMB field
			SymbolListMapEntry* newEntry = niprov->generateNewEntry();
			string prodSymb = newEntry->getProSymb();
			//set PROD_PERM field
			fieldList.addUInt(1, newEntry->getProdPerm());
			//set PROV_SYMB field
			char *cstr = new char[prodSymb.length() + 1];
			strcpy(cstr, prodSymb.c_str());
			fieldList.addRmtes(3422, EmaBuffer(cstr, sizeof(cstr)));
			fieldList.complete();

			//set PROV_SYMB as a Key, set Action and fields to an entry. Then, add the entry to the map.
			map.addKeyAscii(EmaString(cstr), static_cast<MapEntry::MapAction>(newEntry->getAction()), fieldList);
			delete[] cstr;
			fieldList.clear();
		}
		map.complete();

		cout<<"Non-Interactive Provider is publishing a Symbol List Refresh message."<<endl;
		//Publish a single complete Refresh message of the Symbol List named 0#NEWSBL to the service named NI_PUB. 
		//The payload of the Refresh message is the map created previously.
		provider.submit(RefreshMsg().domainType(MMT_SYMBOL_LIST).serviceName("NI_PUB").name("0#NEWSBL").state(OmmState::StreamState::OpenEnum, OmmState::DataState::OkEnum, OmmState::StatusCode::NoneEnum, "UnSolicited Refresh Completed").payload(map).complete(true), itemHandle);
		sleep(1000);
		map.clear();
		cout<< "Non-Interactive Provider starts publishing Symbol List Update messages." << endl;
		//Send one Update message every 3 seconds till 20 Update messages
		for (int i = 0; i < 20; i++)
		{
			//Generate the List of new entries
			niprov->generateUpdatedEntries();
			
			for (vector<SymbolListMapEntry*>::iterator it = niprov->updateMap->begin(); it != niprov->updateMap->end();it++)
			{
				SymbolListMapEntry* anEntry = *it;
				//An entry which action is ADD or UPDATE contains PROD_PERM and PROV_SYMB field. 
				string prodSymb = anEntry->getProSymb();
				char *cstr = new char[prodSymb.length() + 1];
				strcpy(cstr, prodSymb.c_str());

				if (anEntry->getAction() != MapEntry::MapAction::DeleteEnum)
				{
					//set PROD_PERM field
					fieldList.addUInt(1, anEntry->getProdPerm());
					//set PROV_SYMB field
					fieldList.addRmtes(3422, EmaBuffer(cstr, sizeof(cstr)));
				}
				fieldList.complete();
				//set PROV_SYMB as a Key, set Action and fields to an entry. Then, add the entry to the map.
				map.addKeyAscii(EmaString(cstr), static_cast<MapEntry::MapAction>(anEntry->getAction()), fieldList);
				delete[] cstr;
				fieldList.clear();
			}
			map.complete();
			//Publish an Update message of the Symbol List named 0#NEWSBL to the service named NI_PUB. 
			//The payload of the Update message is the map created previously.
			provider.submit(UpdateMsg().serviceName("NI_PUB").name("0#NEWSBL").domainType(MMT_SYMBOL_LIST).payload(map), itemHandle);
			map.clear();

			//Clear the list of entries which have been added to the latest update message
			for (int i = 0; i < niprov->updateMap->size(); i++)
			{
				if ((*niprov->updateMap)[i]->getAction() == (int)MapEntry::DeleteEnum)
					delete (*niprov->updateMap)[i];
			}
			niprov->updateMap->clear();

			sleep(3000);
		}
		cout << "Non-Interactive Provider published all Symbol List messages. It exits." << endl;
	}
	catch (const OmmException& excp)
	{
		cout << excp << endl;
	}
	return 0;
}

