#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include "Item.h"

using namespace std;

float craftingSpeedMultiplier = 1;

vector<Item> items;

vector<string> basicItemNames; // to calculate total raw resources
vector<float> basicItems_secs;  // ^

vector<string> output_assembler_names;
vector<float> output_assembler_nums;
vector<int> output_tab_levels;

Item promptForItem(string item_name) {
	string name, isBasicStr, ingredientName;
	float craftTime;
	int craftNum, ingredientCraftNum;
	bool isBasic;
	Item new_item;

	cout << endl;
	cout << "is " << item_name << " a basic resource? [y/n]: ";
	while(cin >> isBasicStr) {
		if(isBasicStr == "y" || isBasicStr == "n") {
			isBasic = (isBasicStr == "y");
			break;
		}
	}
	if(isBasic) { //the basic item does not need extra information
		cout << endl;
		return Item(item_name, 0, 0, true);
	}

	cout << "crafting Time: ";
	cin >> craftTime;

	cout << "number craft: ";
	cin >> craftNum;

	
	new_item = Item(item_name, craftTime, craftNum, isBasic); //this is for a non-basic item

	cout << endl;	
	cout << "input ingredient. Type done to finish:" << endl;
	cout << endl;
	cout << "ingredient item name: ";
	cin.clear();
	while( getline(cin >> ws, ingredientName) ) {
		if(ingredientName == "done") break;

		cout << "ingredient number needed: ";
		cin >> ingredientCraftNum;

		new_item.addIngredient(ingredientName, ingredientCraftNum);

		cout << endl;
		cout << "ingredient item name (or done): ";
		cin.clear();
	}

	cout << left << setw(16) << "Name" << "Time" << " " << "Num" << " " << "Basic" << "  " << "{ingredient,num;}" << endl;
	cout << "------------------------------------------------" << endl;
	cout << new_item.toString() << endl;

	return new_item;
}

void recursive_assembler_calculator(string item_name, float items_sec, int level = 0) { //ingredient_items_sec = item_sec * (item_recipe)
	vector<string> ingredientNames;
	vector<int> ingredientNums;
	vector<float> ingredient_items_secs;
	Item item;

	//add an item, because none exist
	if(0 == items.size()) {
		cout << item_name << " does not exist, please add " << item_name << endl;
		item = promptForItem(item_name);
		items.push_back(item);
	}

	//lookup the item to get recipe
	for(int i=0; i < items.size(); i++) {
		if(items.at(i).getName() == item_name) {
			item = items.at(i);
			break;
		}
		else if((items.size() - 1) == i) {
			//add the item, because it doesn't exist
			cout << item_name << " does not exist, please add " << item_name << endl;
			item = promptForItem(item_name);
			items.push_back(item);
		}
	}
	ingredientNames = item.getIngredientNames();
	ingredientNums = item.getIngredientNums();

	if( item.isBasicResource() ) {
		bool foundBasicItem = false;
		for(int i=0; i < basicItemNames.size(); i++) {
			if( item_name == basicItemNames.at(i) ) {
				basicItems_secs.at(i) += items_sec;
				foundBasicItem = true;
			}
		}
		if(!foundBasicItem) {
			basicItemNames.push_back(item_name);
			basicItems_secs.push_back(items_sec);
		}
		
		return; //we're at the end of the chain if this is a basic resource.
	}

	//turn item_sec into assembler_nums
	output_assembler_names.push_back(item_name);
	output_assembler_nums.push_back( items_sec / ( item.getCraftNum() / (item.getCraftTime() / craftingSpeedMultiplier) ) );
	output_tab_levels.push_back(level);

	//use recipe and item_sec to find ingredient_item_sec
	for(int i=0; i < ingredientNames.size(); i++) {
		ingredient_items_secs.push_back( items_sec * ingredientNums.at(i) / item.getCraftNum() );
	}

	//loop through ingredient items recalling this function, unless it is a basic resource, then add to total basic resoures
	for(int i=0; i < ingredientNames.size(); i++) {
		recursive_assembler_calculator( ingredientNames.at(i), ingredient_items_secs.at(i), level + 1 );
	}
}

int readFromCSV(string inputFile) { // returns 1 if an error, 0 if not
	// open inputFile, check if it opens, if not try to create inputFile
	ifstream fin(inputFile);
	if(!fin.is_open()) {
		cerr << "failed to open " << inputFile << ". Attempting to create " << inputFile << endl;

		ofstream fout(inputFile);
		if(!fout.is_open()) {
			cerr << "failed to create " << inputFile << ". Check permissions" << endl;
			fout.close();
			return 1;
		}
		cerr << inputFile << " successfully created, reattepting open" << endl;
		fin.open(inputFile);
		if(!fin.is_open()) {
			cerr << inputFile << " failed to open" << endl;
			return 1;
		}
		cerr << inputFile << " successfully opened" << endl;
	}
	
	// read in from csv file
	string line, name, craftTimeStr, craftNumStr, isBasicStr, ingredientName, ingredientNumStr;
	bool isBasic;

	getline(fin, line); // trash the headers
	while (getline(fin, line)) {
		istringstream sin(line);

		getline(sin, name,		',');
		getline(sin, craftTimeStr,	',');
		getline(sin, craftNumStr,	',');
		getline(sin, isBasicStr,	',');
		
		if(isBasicStr == "true" || isBasicStr == "false") { isBasic = isBasicStr == "true"; }
		else { cerr << name << "has bad input for isBasic in " << inputFile << endl; }

		items.push_back(Item( name, stof(craftTimeStr), stoi(craftNumStr), isBasic ));
		
		getline(sin, line, '{'); // trash the bracket
		while(true) { // go until the end bracket
			getline(sin, ingredientName, ',');

			if("}" == ingredientName) break; //if there is not a ingredient, and instead a bracket. we're done

			getline(sin, ingredientNumStr, ';');

			items.at(items.size() - 1).addIngredient(ingredientName, stoi(ingredientNumStr));
		}
	}
	fin.close();
	return 0;
}

int writeToCSV(string outputFile) {
// print new outputFile file //
	ofstream fout(outputFile);
	if(!fout.is_open()) {
		cerr << "failed to open " << outputFile << ". Check permissions" << endl;
		fout.close();
		return 1;
	}
	fout << "name,craftingTime,craftNum,isBasicResource,{ingredient,num;...}" << endl; //header
	for(int i=0; i < items.size(); i++) {
		fout << items.at(i).toCSV() << endl;
	}
	fout.close();
	return 0;
}

int main(int argc, char** argv) {

// parse paramteters and flags //
	string goal_item_name, inputFile = "items.csv", outputFile = "items.csv";
	float goal_items_sec;
	bool verbose = false;

	if(2 == argc && *"-h" == *argv[1]) {
		cout << "Usage: ./recipeCalc \"[item_name]\" [items/sec] -flags" << endl;
		cout << "			the quotes are VERY important around item_name" << endl;
		cout << "-s <float>:	crafting speed multiplier" << endl;
		cout << "-v:		verbose. print out the item csv file in readable format" << endl;
		cout << "		        prints out cumulative assemblers when calculating total" << endl;
		cout << "-i <file.csv>:	input csv file (default is items.csv)" << endl;
		cout << "-o <file.csv>:	output csv file (default is items.csv)" << endl;
		cout << "-h:	help" << endl;
		cout << endl;
		return 0;
	}

	if(argc < 3) {
		cerr << "Usage: ./recipeCalc \"[item_name]\" [items/sec] -flags" << endl;
		cerr << "			the quotes are VERY important around item_name" << endl;
		return 1;
	}
	
	//parse -flags
	for(int i=3; i < argc; i++) {
		char flag;
		if(argv[i][0] == '-') { flag = argv[i][1]; }

		switch(flag) {
			case 'i':
				inputFile = argv[i+1];
				if("items.csv" == outputFile) { outputFile = argv[i+1]; }
				i++; //to skip the filename next iteration;
				break;
			case 'o':
				outputFile = argv[i+1];
				i++; //to skip the filename next iteration;
				break;
			case 'v':
				verbose = true;
				break;
			case 's':
				craftingSpeedMultiplier = stof(argv[i+1]);
				i++;
				break;
			case 'h':
				cout << "Usage: ./recipeCalc \"[item_name]\" [items/sec] -flags" << endl;
				cout << "			the quotes are VERY important around item_name" << endl;
				cout << "-s <float>:	crafting speed multiplier" << endl;
				cout << "-v:		verbose. print out the item csv file in readable format" << endl;
				cout << "		        prints out cumulative assemblers when calculating total" << endl;
				cout << "-i <file.csv>:	input csv file (default is items.csv)" << endl;
				cout << "-o <file.csv>:	output csv file (default is items.csv)" << endl;
				cout << "-h:	help" << endl;
				cout << endl;
				return 0;
		}
	}

	goal_item_name = argv[1];	
	goal_items_sec = stoi(argv[2]);

	// grab info from the csv, input file can be changed with -i flag. exits on error
	if( readFromCSV(inputFile) ) { return 1; }

	// print the items that have been read in
	if(verbose) {
		cout << endl;
		cout << "items in " << inputFile << ":" << endl;
		cout << left << setw(16) << "Name" << "Time" << " " << "Num" << " " << "Basic" << "  " << "{ingredient,num;}" << endl;
		cout << "------------------------------------------------" << endl;
		for(int i=0; i < items.size(); i++) {
			cout << items.at(i).toString();
		}
		cout << endl; //give space
	}

	cout << "name:     " << goal_item_name << endl;
	cout << "items/sec: " << goal_items_sec << endl;
	cout << endl;

// calculate the number of assemblers recursively //
	recursive_assembler_calculator(goal_item_name, goal_items_sec);

	int total_rounded_assemblers = 0;
	// print the assembler tree
	for(int i=0; i < output_assembler_names.size(); i++) {
		for(int l=0; l < output_tab_levels.at(i); l++) {
			cout << '\t';
		}
		cout << output_assembler_nums.at(i) << " " << output_assembler_names.at(i) << endl;
		total_rounded_assemblers += ( ceil(output_assembler_nums.at(i)) );
		if(verbose) { cout << total_rounded_assemblers << endl; }
	}

	cout << endl;
	cout << "total assemblers (rounded up for each item): " << total_rounded_assemblers << endl;

	cout << "total basic resources" << endl;
	cout << left << setw(15) << "Name" << "Items/sec" << endl;
	cout << "------------------------" << endl;

	for(int i=0; i < basicItemNames.size(); i++) {
		cout << left << setw(15) << basicItemNames.at(i) << ": " << basicItems_secs.at(i) << endl;
	}


	// write info to the csv, output file can be changed with -o flag. exits on error
	if( writeToCSV(outputFile) ) { return 1; }


	cout << endl;
}
