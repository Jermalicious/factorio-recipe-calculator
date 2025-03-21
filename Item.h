#pragma once
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class Item
{
private:
	string name, isBasicStr;
	float craftTime;
	int craftNum;
	bool isBasic;

	vector<string> ingredientNames;
	vector<int> ingredientNums;

public:
	Item(std::string name, float craftTime, int craftNum, bool isBasic);
	Item();

	string getName() 	 { return name; }
	float  getCraftTime()	 { return craftTime; }
	int    getCraftNum() 	 { return craftNum; }
	bool   isBasicResource() { return isBasic; }
	string getIsBasicStr()	{ return isBasicStr; }
	vector<string> getIngredientNames() { return ingredientNames; }
	vector<int>    getIngredientNums()  { return ingredientNums; }
	
	string getPrintableRecipe();
	string toString();
	string toCSV();

	void addIngredient(string ingredientName, int ingredientNum);

};
