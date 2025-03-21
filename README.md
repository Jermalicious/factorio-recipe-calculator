# factorio-recipe-calculator
This is a project that automatically finds how many assemblers you need of each item in a recipe to hit a specified items/sec
The CSV file is optional, as the program can create it on its own, and it will add to the csv file whenever a new item is needed.

Usage: ./recipeCalc "[item_name]" [items/sec] -flags
                        the quotes are VERY important around item_name

flags:
-s <float>:     crafting speed multiplier to account for different assembler crafting speeds
-v:             verbose. print out the item csv file in readable format
                prints out cumulative assemblers when calculating total
-i <file.csv>:  input csv file (default is items.csv)
-o <file.csv>:  output csv file (default is items.csv)
-h:             help
