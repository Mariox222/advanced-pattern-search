/**
	functionality for the advanced pattern search
	@file advancedPatternSearch.h
	@author Mario Basic
	@version 1.0 2020.12.23
*/

#pragma once

#include "lineTypes.h"

/** return final string output
	@param input the vector of strings which represent input lines
	@return a string with each byte shown in hex and seperated with a space. Example "00 AB F1"
*/
std::string createOutput(std::vector<std::string>& input);

/** return vector of strings representing bytes output
	@param input the vector of strings which represent input lines with no new line symbols
	@return a vector of strings where each strings represent a byte shown in hex Example ["00", "AB", "F1"]
*/
std::vector<std::string> createHexVector(std::vector<std::string>& input);

/** return which type given line is
	@param line the input line
	@return line type of input line
*/
lineType recnognizeLine(std::string& line);

/** adds strings representing bytes in memory at the location of the given word into the given string vector
	@param word the word whose bytes in memory need to be added to addTo param
	@param addTo the vector of strings the function will append the results to
*/
template <typename T>
void hexDumpWord(const T& word, std::vector<std::string>& addTo);

/** adds the memory view of a given char array to the string vector
	@param array char array whose bytes in memory need to be added to addTo param
	@param addTo the vector of strings the function will append the results to
*/
void hexDumpCharArray(const char* array, std::vector<std::string>& addTo);

/** return a vector of strings wich represent how the given value looks in memory in hex
	@param mem pointer to a memory location from which to read memory from
	@param size read size
	@return a vector of strings where each strings represent a byte read from memory shown in hex Example ["00", "AB", "F1"]
*/
std::vector<std::string> hexDump(const void* mem, unsigned int size);

/** returns the last word in the string (seperator is space)
	@param str the string to extract the last word from
	@return the last word from the given string
*/
std::string getLastWord(std::string str);

/** returns converted integer type to hex string. Examples: 9 -> 09, 10 -> 0A, 16 -> 10
	@param i integer to convert to hex string
	@return a string which is a hex representation of the given integer
*/
template< typename T >
std::string int_to_hex(T i);

/** split the given string with the given delimiter. Note splitString("abc","a") results with just "bc"
	@param str input string
	@param delim deliminator used for seperation
	@return a vector of seperated strings
*/
std::vector<std::string> splitString(const std::string& str, const std::string& delim);

/** return the text inside the outermost quotes ( " or ' ). getTextInQuotes("string \"you\'re \'ok\'\"") returns "you're 'ok'"
	@param str the input string
	@return the text in quotes
*/
std::string getTextInQuotes(const std::string & str);
