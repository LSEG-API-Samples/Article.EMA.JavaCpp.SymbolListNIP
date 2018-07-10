#pragma once
#include <string>
#include <Ema.h>

//The class is to represent an entry in the map of Symbol List.
class SymbolListMapEntry {
public:
	inline int getAction() {
		return action;
	}
	inline int getProdPerm() {
		return prodPerm;
	}
	inline std::string getProSymb() {
		return provSymb;
	}
	inline static int getDefaultProdPerm() {
		return DEFAULT_PROD_PERM;
	}
	void performAddAction(int newpp, std::string newps);
	void performUpdateAction(int newpp);
	void performDeleteAction();

private:
	//Action can be UPDATE(1), ADD(2) or DELETE(3)
	 int action;
	//PROD_PERM  which field id is 1 and type is UINT64 
	 int prodPerm;
	//PROV_SYMB  which field id is 3422 and type is RMTES_STRING
	std::string provSymb;
	static const int DEFAULT_PROD_PERM = 3056;
};