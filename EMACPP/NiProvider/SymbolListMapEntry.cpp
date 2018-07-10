#include "SymbolListMapEntry.h"

using namespace thomsonreuters::ema::access;

//For ADD action
//Set PROD_PERM and PROV_SYMB according to the input values
//If the input PROD_PERM is less than zero set to default(3056)
//Set Action to be ADD
void SymbolListMapEntry::performAddAction(int newpp, std::string newps) {
	if (newpp <= 0)
		newpp = DEFAULT_PROD_PERM;
	prodPerm = newpp;
	provSymb = newps;
	action = MapEntry::MapAction::AddEnum;//2
}
//For UPDATE action
//Set PROD_PERM according to the input value
//If the input PROD_PERM is less than zero set to default(3056)
//Set Action to be UPDATE
void SymbolListMapEntry::performUpdateAction(int newpp) {
	if (newpp <= 0)
		newpp = DEFAULT_PROD_PERM;
	prodPerm = newpp;
	action = MapEntry::MapAction::UpdateEnum;//1
}
//For DELETE action
//Set Action to be DELETE
void SymbolListMapEntry::performDeleteAction() {
	action = MapEntry::MapAction::DeleteEnum;//3
}