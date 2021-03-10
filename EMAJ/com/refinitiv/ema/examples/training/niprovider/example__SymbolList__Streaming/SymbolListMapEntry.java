package com.refinitiv.ema.examples.training.niprovider.example__SymbolList__Streaming;

import com.refinitiv.ema.access.MapEntry;

//The class is to represent an entry in the map of Symbol List.
public class SymbolListMapEntry {
	//Action can be UPDATE(1), ADD(2) or DELETE(3)
	private int action; 
	//PROD_PERM  which field id is 1 and type is UINT64 
	private int prodPerm; 
	//PROV_SYMB  which field id is 3422 and type is RMTES_STRING
	private String provSymb;
	private static final int DEFAULT_PROD_PERM=3056;
    int getAction() {
    	return action;
    }
    int getProdPerm() {
    	return prodPerm;
    }
    String getProSymb() {
    	return provSymb;
    }
    static int getDefaultProdPerm() {
    	return DEFAULT_PROD_PERM;
    } 
    //For ADD action
    //Set PROD_PERM and PROV_SYMB according to the input values
    //If the input PROD_PERM is less than zero set to default(3056)
    //Set Action to be ADD
    void performAddAction(int newpp,String newps) {
    	if(newpp<=0)
    		newpp = DEFAULT_PROD_PERM;
    	prodPerm = newpp;
    	provSymb = newps;
    	action = MapEntry.MapAction.ADD;//2
    }
    //For UPDATE action
    //Set PROD_PERM according to the input value
    //If the input PROD_PERM is less than zero set to default(3056)
    //Set Action to be UPDATE
    void performUpdateAction(int newpp) {
    	if(newpp<=0)
    		newpp = DEFAULT_PROD_PERM;
    	prodPerm = newpp;
    	action = MapEntry.MapAction.UPDATE;//1
    }
    //For DELETE action
    //Set Action to be DELETE
    void performDeleteAction() {
    	action = MapEntry.MapAction.DELETE;//3
    }

}
