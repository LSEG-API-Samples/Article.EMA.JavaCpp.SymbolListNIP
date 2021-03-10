package com.refinitiv.ema.examples.training.niprovider.example__SymbolList__Streaming;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import com.refinitiv.ema.examples.training.niprovider.example__SymbolList__Streaming.SymbolListMapEntry;
import com.refinitiv.ema.access.EmaFactory;
import com.refinitiv.ema.access.FieldList;
import com.refinitiv.ema.access.Map;
import com.refinitiv.ema.access.MapEntry;
import com.refinitiv.ema.access.OmmException;
import com.refinitiv.ema.access.OmmNiProviderConfig;
import com.refinitiv.ema.access.OmmProvider;
import com.refinitiv.ema.access.OmmState;
import com.refinitiv.ema.rdm.EmaRdm;

public class NiProvider {
	//The maximum number of entries in the Symbol List
	int MAX_ENTRIES=150;
	//The minimum number of entries in the Symbol List
	int MIN_ENTRIES=20;
	//The maximum chance of the DELETE action is 10 
	//It the random number is 1-10, perform DELETE action 
	int deleteMaxChance = 10;
	//The maximum chance of the UPDATE action is 40 
	//It the random number is 11-40, perform UPDATE action 
	int updateMaxChance = 40; 
	//The maximum chance of the ADD action is 100
	//It the random number is 41-100, perform ADD action 
	int addMaxChance = 100; 
	//The List of SymbolListMapEntry represents all entries in the Symbol List from both Refresh and Update messages  
	List<SymbolListMapEntry> symbolListMap;
	//The List of SymbolListMapEntry represents the entries in an update message  
	List<SymbolListMapEntry> updateMap;
	//The Set keeps the PROV_SYMB/Key in the current Symbol List
	//It is used to verify if a new generated PROV_SYMB/Key is duplicated with any one in the Set
	HashSet<String> currentKeys;
	//The Random class is used to generate PROD_PERM and PROV_SYMB/Key at random
	//It is used to choose an Action, an entry which will be updated or deleted at random
	Random randomInt;
	//Initialize the NiProvider's variables
	public NiProvider() {
		symbolListMap = new ArrayList<SymbolListMapEntry>(MAX_ENTRIES);
		updateMap = new ArrayList<SymbolListMapEntry>(5);
		currentKeys = new HashSet<String>(MAX_ENTRIES);
		randomInt = new Random();
	}
	//Generate a new SymbolListMapEntry which represents an entry.
	public SymbolListMapEntry generateNewEntry()
	{
		//Initialize a new SymbolListMapEntry
		SymbolListMapEntry newEntry = new SymbolListMapEntry();
		//The variable to keep PROV_SYMB/Key
		String tmpKey;
		//PROV_SYMB pattern is [A-Z][A-Z][A-Z].[A-Z]
		//Generate a PROV_SYMB/Key till it is not the same as any current PROV_SYMB/Key 
		do {
			tmpKey = String.valueOf(new char[] { (char)(randomInt.nextInt(26) + 65),
	                (char)(randomInt.nextInt(26) + 65), (char)(randomInt.nextInt(26) + 65), '.',
	                (char)(randomInt.nextInt(26) + 65) });
		} while(currentKeys.contains(tmpKey));
		//Set the SymbolListMapEntry with ADD Action 
	    newEntry.performAddAction(SymbolListMapEntry.getDefaultProdPerm(),tmpKey); 
	    //Add the SymbolListMapEntry in the Symbol List
	    symbolListMap.add(newEntry);
	    //Add the PROV_SYMB/Key of the SymbolListMapEntry in the PROV_SYMB/Key Set
		currentKeys.add(newEntry.getProSymb());
	    return newEntry;
	}
	//Generate the List of new entries which will be added to the next update message
	public void generateUpdatedEntries()
	{
		//Clear the list of entries which have been added to the latest update message
		updateMap.clear();
		//Create the PROV_SYMB/Key Set of the update message
	    HashSet<String> updateEntries = new HashSet<String>();
	    //Generate a new ProdPerm which is 1000-5000 at random  
	    int newProdPerm = randomInt.nextInt(4000) + 1000;
	    //The list contains maximum 5 entries, for each one do:
	    for (int i = 0; i < 5; i++) 
	    {
	    	//Declare SymbolListMapEntry to keep a new entry
	    	SymbolListMapEntry updateEntry = null;
	    	//Select an Action at random
	        int action = randomInt.nextInt(100);
	        //If the number of entries in the Symbol List is less than the minimum(20)
	        //or the random Action is ADD
	        if (symbolListMap.size() < MIN_ENTRIES || action > updateMaxChance) 
	        {
	        	//If the number of entries in the Symbol List is less than maximum(150)
	        	if(symbolListMap.size() < MAX_ENTRIES) {
	        		//Perform ADD action by generating a new entry with ADD action
	        		updateEntry = generateNewEntry();
	            }
	        	//If the number of entries in the Symbol List equal or is more than the maximum(150),
	        	//try to delete an existing entry in the Symbol List
	        	else 
	            {   
	        		//Select the deleted entry in the Symbol List at random
	            	int index = randomInt.nextInt(symbolListMap.size());
	            	updateEntry = symbolListMap.get(index);
	            	//If the PROV_SYMB/Key of the selected entry is duplicated with 
	            	//any PROV_SYMB/Key in the update Set, skip to create the next entry
	            	if (updateEntries.contains(updateEntry.getProSymb()))
	            	{
	            		continue;
	            	}
	            	//Otherwise, perform DELETE action on the selected entry
	            	updateEntry.performDeleteAction();
	            	//Remove the deleted entry from the Symbol List
	            	symbolListMap.remove(updateEntry);
	            	//Remove the PROV_SYMB/Key of the deleted entry from the PROV_SYMB/Key Set
	            	currentKeys.remove(updateEntry.getProSymb());
	            }
	        }
	        //If the random action is UPDATE
	        else if (action > deleteMaxChance) 
	        {
	        	//Select the updated entry in the Symbol List at random
	            int index = randomInt.nextInt(symbolListMap.size());
	            updateEntry = symbolListMap.get(index);
	            //If the PROV_SYMB/Key of the selected entry is duplicated with 
            	//any PROV_SYMB/Key in the update Set, skip to create the next entry
	            if (updateEntries.contains(updateEntry.getProSymb()))
		        {
	            	continue;
		        }
	            //Otherwise, perform UPDATE action on the selected entry  
	            updateEntry.performUpdateAction(newProdPerm);
	        }
	        //If the random action is DELETE
	        else
	        {
	        	//If the number of entries in the Symbol List equal or less than minimum(20)
	            if (symbolListMap.size() <= MIN_ENTRIES) {
	            	//Perform ADD action by generating a new entry with ADD action
	            	updateEntry = generateNewEntry();
	            } 
	            else 
	            {   //If the number of entries in the Symbol List is more than the minimum(20),
	    	        //try to delete an existing entry in the Symbol List
	            	//Select the deleted entry in the Symbol List at random
		            int index = randomInt.nextInt(symbolListMap.size());
		            updateEntry = symbolListMap.get(index);
		            //If the PROV_SYMB/Key of the selected entry is duplicated with 
		            //any PROV_SYMB/Key in the update Set, skip to create the next entry
		            if (updateEntries.contains(updateEntry.getProSymb()))
		            {
		            	continue;
		            }
		            //Otherwise, perform DELETE action on the selected entry
		            updateEntry.performDeleteAction();
		            //Remove the deleted entry from the Symbol List
		            symbolListMap.remove(updateEntry);
		            //Remove the PROV_SYMB/Key of the deleted entry from the PROV_SYMB/Key Set
		            currentKeys.remove(updateEntry.getProSymb());
	            }
	        }
	        //Add the PROV_SYMB/Key in the Set of the new update message.
	        updateEntries.add(updateEntry.getProSymb());
	        //Add the entry in the List of the new update message.
	        updateMap.add(updateEntry);
	    }
	}
	public static void main(String[] args)
	{
		OmmProvider provider = null;
		
		try
		{
			//Use the default NiProvider's configuration in EmaConfig.xml found in the working directory
			OmmNiProviderConfig config = EmaFactory.createOmmNiProviderConfig();
			//change "adhserver:14003" to your ADH server IP/name and its port
			//change "user" to NI Provider's user which logs in to ADH
			provider = EmaFactory.createOmmProvider(config.host("adhserver:14003").username("user"));
			
			System.out.println("Non-Interactive Provider starts.");
			//The handle which identifies item stream on which to send the Refresh message and Update messages of a Symbol List.
			long itemHandle = 5;
			//Create an object of NiProvider which provides the methods to create/update and manage Symbol List.
			NiProvider niprov = new NiProvider();
			
			//Create a map which is encoded content of Symbol List. The map contains entries.
			Map map = EmaFactory.createMap();
			//Create a field list which is encoded content of an entries.
			FieldList fieldList = EmaFactory.createFieldList();
			//Encode a map contains 25 entries
			for( int i = 0; i < 25; i++ )
			{
				//generate SymbolListMapEntry instance consisting of action(ADD), PROD_PERM field and PROV_SYMB field
				SymbolListMapEntry newEntry = niprov.generateNewEntry();
				//set PROD_PERM field
				fieldList.add(EmaFactory.createFieldEntry().uintValue(1, newEntry.getProdPerm()));
				//set PROV_SYMB field
				fieldList.add(EmaFactory.createFieldEntry().rmtes(3422, ByteBuffer.wrap(newEntry.getProSymb().getBytes())));
				//set PROV_SYMB as a Key, set Action and fields to an entry. Then, add the entry to the map.
				map.add(EmaFactory.createMapEntry().keyAscii(newEntry.getProSymb(), newEntry.getAction(), fieldList));
				fieldList.clear();
			}
			System.out.println("Non-Interactive Provider is publishing a Symbol List Refresh message.");
			//Publish a single complete Refresh message of the Symbol List named 0#NEWSBL to the service named NI_PUB. 
			//The payload of the Refresh message is the map created previously.
			provider.submit( EmaFactory.createRefreshMsg().domainType(EmaRdm.MMT_SYMBOL_LIST).serviceName("NI_PUB").name("0#NEWSBL")
					.state(OmmState.StreamState.OPEN, OmmState.DataState.OK, OmmState.StatusCode.NONE, "UnSolicited Refresh Completed")
					.payload(map).complete(true), itemHandle);
			try {
				Thread.sleep(1000);
			}catch(Exception e) {
				
			}
			map.clear();
			System.out.println("Non-Interactive Provider starts publishing Symbol List Update messages.");
			//Send one Update message every 3 seconds till 20 Update messages
			for( int i = 0; i < 20; i++ ) {
				//Generate the List of new entries
				niprov.generateUpdatedEntries();
				Iterator<SymbolListMapEntry> it = niprov.updateMap.iterator();
				//for each entry in the List
				while(it.hasNext()) 
				{
					SymbolListMapEntry anEntry = it.next();
					//An entry which action is ADD or UPDATE contains PROD_PERM and PROV_SYMB field. 
					if (anEntry.getAction()!=MapEntry.MapAction.DELETE) {
						//set PROD_PERM field
						fieldList.add(EmaFactory.createFieldEntry().uintValue(1, anEntry.getProdPerm()));
						//set PROV_SYMB field
						fieldList.add(EmaFactory.createFieldEntry().rmtes(3422, ByteBuffer.wrap(anEntry.getProSymb().getBytes())));
					}
					//set PROV_SYMB as a Key, set Action and fields to an entry. Then, add the entry to the map.
					map.add(EmaFactory.createMapEntry().keyAscii(anEntry.getProSymb(),anEntry.getAction(),fieldList));
					fieldList.clear();
				}
				//Publish an Update message of the Symbol List named 0#NEWSBL to the service named NI_PUB. 
				//The payload of the Update message is the map created previously.
				provider.submit( EmaFactory.createUpdateMsg().serviceName("NI_PUB").name("0#NEWSBL").domainType(EmaRdm.MMT_SYMBOL_LIST).payload( map ), itemHandle );
				map.clear();
				Thread.sleep(3000);
			}
			System.out.println("Non-Interactive Provider published all Symbol List messages. It exits.");
			
		} 
		catch (InterruptedException | OmmException excp)
		{
			System.out.println(excp.getMessage());
		}
		finally 
		{
			if (provider != null) provider.uninitialize();
		}
	}
}