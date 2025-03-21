#include "Item.h"
#include <sstream>

using namespace std;

Item::Item(string name, float craftTime, int craftNum, bool isBasic) {
	this->name = name;
	this->craftTime = craftTime;
	this->craftNum = craftNum;
	this->isBasic = isBasic;

	if(isBasic) isBasicStr = "true";
	else isBasicStr = "false";
}

Item::Item() {

}

void Item::addIngredient(string ingredientName, int ingredientNum) {
	ingredientNames.push_back(ingredientName);
	ingredientNums.push_back(ingredientNum);
}

string Item::getPrintableRecipe() {
	ostringstream sout;
	
	sout << "{";
	for(int i=0; i < ingredientNames.size(); i++) {
		sout << ingredientNames.at(i)  << "," << ingredientNums.at(i) << ";";
	}
	sout << "}";

	string printableRecipe = sout.str();
	return printableRecipe;
}

string Item::toString() {
	ostringstream sout;

	sout << setw(16) << left << 	getName()	<< " "
	     << setw(4)  	 << 	getCraftTime() 	<< " " 
	     << setw(2)  	 << 	getCraftNum() 	<< " "
	     << setw(6)		 <<	getIsBasicStr()	<< " "
	     << getPrintableRecipe() << endl;
	
	string itemStr = sout.str();
	return itemStr;
}

string Item::toCSV() {
	ostringstream sout;

	sout << name << "," << craftTime << "," << craftNum << "," << isBasicStr << "," << getPrintableRecipe();
	
	string itemCSV = sout.str();
	return itemCSV;
}
