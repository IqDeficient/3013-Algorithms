/*****************************************************************************
 *
 *  Author:           Trey Taylor
 *  Email:            Tbtaylor0816@my.msutexas.edu
 *  Label:            P02
 *  Title:            Linear Search - Using Json and Getch
 *  Course:           3013-Adv. Algorithms
 *  Semester:         Spring 2024
 *
 *  Description:
 *        This program uses a json dictionary to search for words.
 *        The program also uses getch in order to get the characters
 *          the user is typing.
 *        When typing up to 10 words will show up consisting in some
 *          way of the type characters.
 *        It is done using a linear search on the json file.
 *        It will show suggested words after you
 *          type a character into the terminal.
 *        It matches to any location within the word: from,  middle, end.
 *
 *  Files(Required):
 *       dictionary.json
 *       main.cpp
 *       json.hpp
 *       range.hpp
 *       console.hpp
 *
 *  Credit for majority of source code to Terry Griffin
 *  Link to Github: https://github.com/rugbyprof
 *****************************************************************************/
//---------------------------------------------------------------------------//
/**
 * This program uses a "getch" function which means "get character".
 * However, getch does not print the character to the terminal, it
 * lets you decide what to do based on what character you are pressing.
 *
 * You can test which characters are being pressed using their ascii values.
 *
 * An ascii table here should be helpful
 *      http://www.asciitable.com/
 *
 * Some integer values for some keys:
 *      LowerCase Letters   = 97(a) -> 122(z)
 *      UpperCase Letters   = 65(A) -> 90(Z)
 *      Enter Key           = 10
 *      Space Bar           = 32
 *
 *      Arrow Keys          = Have same values as some letters
 *                            so we can't distinguish between
 *                            the two (in this context).
 *
 * Code below is a basic example of using a "getch" function along with
 * searching an array of words for partial matches.
 *
 * https://repl.it/@rugbyprof/getchexample#main.cpp

 rang only depends on C++ standard library, unistd.h system header on unix and
 windows.h & io.h system headers on windows based systems. In other words, you
 don't need any 3rd party dependencies.
 */
//---------------------------------------------------------------------------//
//#include "./headers/animals.hpp"
#include "./headers/console.hpp"
#include "./headers/json.hpp"
#include "./headers/rang.hpp"
#include "./headers/timer.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

using namespace std;
using namespace rang;
using json = nlohmann::json;

consoleSize console_size; // used to store the size of console (width=cols and
                          // height=rows)
//---------------------------------------------------------------------------//
/**
 * partialMatch
 *
 * Description:
 *      Finds partial matches in an array of strings and returns them. It
 *      doesn't matter where in the string the match is.
 * Params:
 *      vector<string>  array       - array to search
 *      string          substring   - substring to search for in each word
 *
 * Returns:
 *      vector<string> - holding all the matches to substring
 */
vector<string> partialMatch(vector<string> array, string substring) {
  vector<string> matches; // to hold any matches
  size_t found;           // size_t is an integer position of
                          // found item. -1 if its not found.

  if (substring.size() == 0) {
    return matches;
  }

  for (int i = 0; i < array.size(); i++) { // loop through array
    found = array[i].find(substring);      // check for substr match
    if (found != string::npos) {           // if found >= 0 (its found then)
      matches.push_back(array[i]);         // add to matches
    }
  }

  return matches;
}
//---------------------------------------------------------------------------//
/**
* Prints a long background row of gray, with text centered.
* Not a good function since colors are hard coded, but using
* rang, I don't want to invest the time to figuere out how to
* interact with rang's enumerated class types and ostream.
* I guess you could add a switch statement to change background
* color:
* switch(color){
  "black" : cout<<bgB::black;
            break;
  "red" : cout<<bgB::red;
          break;
  ...
  ...
  ...
}
But still not a good solution.
*/
void titleBar(string title, int length = console_size.width) {
  string padding = string((length / 2) - (title.size() / 2), ' ');
  title = padding + title + padding;
  cout << bgB::black << fg::gray << title << fg::reset << bg::reset << endl;
}
//---------------------------------------------------------------------------//
/**
 * horizontalBar
 *
 * Description:
 *      Prints a horizontal bar of a given length
 * Params:
 *      int length - length of the bar
 * Returns:
 *      void
 */
void horizontalBar(int length = console_size.width) {
  string line = string(length, '_');
  cout << fg::gray << line << fg::reset << bg::reset << endl;
}
//---------------------------------------------------------------------------//
/**
 * printMatches
 *
 * Description:
 *      Prints the matches to the screen.
 * Params:
 *      vector<string> matches - vector of matches
 * Returns:
 *      void
 */
void printMenu(vector<string> options) {
  int i = 1;
  for (auto s : options) {
    cout << fg::black << style::bold << i << ". " << style::reset << fg::cyan
         << s << fg::reset << bg::reset << endl;
    i++;
  }
  cout << fg::reset << bg::reset;
}
//---------------------------------------------------------------------------//
/**
 * printHighlightedSubstr
 *
 * Description:
 *      Given a word, print the substr underlined and red vs blue for the rest
 * of the word.
 * Params:
 *      string word - word to print
 *      string substr - substring to highlight in red
 *      int loc - location of substr in word
 * Returns:
 *      void
 */
void printHighlightedSubstr(string word, string substr, int loc) {
  for (int j = 0; j < word.size(); j++) {
    // if we are printing the substring turn it red
    if (j >= loc && j <= loc + substr.size() - 1) {
      cout << fg::red << style::underline << word[j] << fg::blue
           << style::reset;
    } else {
      cout << fg::blue << word[j] << fg::reset << style::reset;
    }
  }
}
//---------------------------------------------------------------------------//
/**
 * printCurrent
 *
 * Description:
 *      Prints the current key pressed and the current substr to the screen.
 * Params:
 *      char k - last character pressed.
 *      string word - current substring being printed.
 * Returns:
 *      void
 */
void printCurrent(char k, string word) {
  cout << fg::green << style::bold << "KeyPressed: \t\t" << style::reset
       << fgB::yellow;
  if (int(k) == 127) {
    cout << "del";
  } else {
    cout << k;
  }
  cout << " = " << (int)k << fg::reset << endl;
  cout << fg::green << style::bold << "Current Substr: \t" << fg::reset
       << fgB::blue << word << fg::reset << style::reset << endl;
  cout << endl;
}
//---------------------------------------------------------------------------//
/**
 * errorMessage
 *
 * Description:
 *      Prints an error message to the screen.
 * Params:
 *      string message - message to print
 * Returns:
 *      void
 */
void errorMessage(string message) {
  cout << bgB::red << fgB::gray << message << fg::reset << bg::reset << endl;
  sleep(1);
}
//---------------------------------------------------------------------------//
/**
 * loadJsonObject
 *
 * Description:
 *      Loads a json object from a json file path
 * Params:
 *      none
 * Returns:
 *      json - json object
 */
json loadJsonObject() {
  // Load from json file path
  string filePath = "./data/dictionary.json";

  // Open the file stream
  ifstream fileStream(filePath);

  // If condition on wheter the file is open
  if (!fileStream.is_open()) {
    cerr << "Failed to open file: " << filePath << endl;
    return 1;
  }

  // Parse the JSON file
  json words;
  // Reads filestream into words on a try block
  try {
    fileStream >> words;
  }
  // Catch in case of parsing error
  // Can implement some kind of handling if needed
  catch (json::parse_error &e) {
    cerr << "Parse error: " << e.what() << endl;
    // Handle errors (e.g., file not found, invalid JSON format)
    return 1;
  }

  // Close the file stream
  fileStream.close();

  // Return the json object
  return words;
}
//---------------------------------------------------------------------------//
int main() {

  console_size = getConsoleSize(); // get width and height of console
  char k;                          // holder for character being typed
  string key;                      // string version of char for printing
  string substr = "";              // var to concatenate letters to

  // Calls the loadJsonObject function to load the json file
  json words = loadJsonObject();

  vector<string> matches; // any matches found in vector of words
  int loc;                // location of substring to change its color
  bool deleting = false;
  string str = "";
  vector<string> mainMenu;

  mainMenu.push_back("Type letters and watch the results change.");
  mainMenu.push_back(
      "Hit the DEL key to erase a letter from your search string.");
  mainMenu.push_back(
      "When a single word is found, hit enter. (this is for real assignment.)");
  // Tells user how to exit the program
  mainMenu.push_back("Enter a captial Z to end the program.");

  clearConsole();
  titleBar("Getch Example", console_size.width);
  printMenu(mainMenu);

  // While capital Z is not typed keep looping
  while ((k = getch()) != 'Z') {

    clearConsole();
    titleBar("Getch Example", console_size.width);
    printMenu(mainMenu);

    // Tests for a backspace and if pressed deletes
    // last letter from "substr".
    if ((int)k == 127) {
      if (substr.size() > 0) {
        substr = substr.substr(0, substr.size() - 1);
        deleting = true;
      }
    } else {
      deleting = false;
      // Make sure a letter was pressed and only letter
      if (!isalpha(k)) {
        errorMessage("Letters only!");
        continue;
      }

      // We know its a letter, lets make sure its lowercase.
      // Any letter with ascii value < 97 is capital so we
      // lower it.
      if ((int)k < 97) {
        k += 32;
      }
      substr += k; // append char to substr
    }
    horizontalBar();
    printCurrent(k, substr);

    // vector string to feed into partialMatch function made from
    // elements of the json file
    vector<string> json;
    for (auto &element : words.items()) {
      string key = element.key();
      json.push_back(key);
    }

    // Find any words in the array that partially match
    // our substr word
    matches = partialMatch(json, substr);

    if ((int)k != 32) { // if k is not a space print it

      key = to_string(k);

      horizontalBar();

      cout << style::bold << style::underline << fg::black
           << "MATCHES: " << fg::green << matches.size() << style::reset
           << endl;

      for (auto &c : substr)
        c = tolower(c);
      // This prints out all found matches
      for (int i = 0; i < matches.size() && i < 10; i++) {
        // find the substring in the substr
        loc = matches[i].find(substr);
        // if its found
        if (loc != string::npos) {
          printHighlightedSubstr(matches[i], substr, loc);
        }
        cout << "  ";
      }
      cout << fg::reset << endl << endl << endl << endl;

      if (matches.size() != 0) {
        // if there is only one result ask user if they would like to see
        // definition
        if (matches.size() == 1) {
          // Prompt for user confirmation
          cout << "If this is the word you are searching press enter." << endl;
          // if condition reading for if enter is pressed
          if ((k = getch()) == 10) {
            cout << "The definition for the word is as follows: " << endl;
            // if condition entered to find word if it exist then give
            // definition
            if (words.find(substr) != words.end()) {
              auto value = words[substr];
              cout << "The definition of '" << substr << "' is:\n"
                   << value << endl;
            }
            // else branch in case word is not found
            else {
              cout << "The word '" << substr
                   << "' does not exist in the JSON dictionary.\n";
            }
          }
        }
        // If there is more than one condition user is asked to see first
        // or their tpyed option
        else {
          cout << "Would you like to see the definition of the word you have "
                  "typed?"
               << endl
               << "To see the definition type an equal sign\n\n";
              // if condition reading for if enter is pressed
              if ((k = getch()) == 61) {
            cout << "The definition for the word is as follows: " << endl;
            // if condition entered to find word if it exist then give
            // definition
            if (words.find(substr) != words.end()) {
              auto value = words[substr];
              cout << "The definition of '" << substr << "' is:\n"
                   << " - " << value << endl;
            }
            // else branch in case word is not found
            else {
              cout << "The word '" << substr
                   << "' does not exist in the JSON dictionary.\n";
            }
          }
        }
      }
    }
  }
  return 0;
}
