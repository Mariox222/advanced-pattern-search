#include "plugin.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include <stdexcept>
#include <string>
#include <sstream>
#include <stdexcept>
#include <codecvt>
#include <locale>



enum class lineType {
	literal_bytes,
	int_declaration,
	float_declaration,
	double_declaration,
	string_declaration,
	wildcard_line,
	emptyLine,
	invalidLine,
	char_declaration,
	unsigned_char_declaration,
	signed_char_declaration,
	unsigned_int_declaration,
	signed_int_declaration,
	short_int_declaration,
	unsigned_short_int_declaration,
	signed_short_int_declaration,
	long_int_declaration,
	signed_long_int_declaration,
	unsigned_long_int_declaration,
	long_long_int_declaration,
	unsigned_long_long_int_declaration,
	long_double_declaration,
	wchar_t_declaration,
	char16_t_declaration,
	char32_t_declaration
};

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
std::string getTextInQuotes(const std::string& str);

// return final string output
std::string createOutput(std::vector<std::string>& input) {
    // remove new line symbols from strings if present
    std::for_each(input.begin(), input.end(), [](std::string& line) {
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        });

    //create byte array
    std::vector<std::string> arrayOfBytes = createHexVector(input);

    //create result string
    std::string result = "";
    std::for_each(arrayOfBytes.begin(), arrayOfBytes.end(), [&result](std::string& byteStr) {
        if (result == "") result = byteStr;
        else result = result + " " + byteStr;
        });
    return result;
}

// return vector of strings representing bytes output
std::vector<std::string> createHexVector(std::vector<std::string>& input) {
    std::vector<std::string> result;
    std::for_each(input.begin(), input.end(), [&result](std::string& line) {
        // transform to lower case
        std::transform(line.begin(), line.end(), line.begin(), ::tolower);

        // initialize helping variables
        std::vector<std::string> toAdd;
        std::string lastWord = getLastWord(line);
        try {
            switch (recnognizeLine(line)) {
            case lineType::literal_bytes:
            {
                // split bytes into individual strings
                toAdd = splitString(line, " ");

                // append new vector to result
                result.insert(std::end(result), std::begin(toAdd), std::end(toAdd));
                break;
            }

            case lineType::int_declaration:
            case lineType::signed_int_declaration:
            {
                // create the new variable
                signed i = std::stoi(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<signed int>(i, result);
                break;
            }
            case lineType::unsigned_int_declaration:
            {
                // create the new variable
                unsigned long ul = std::stoul(lastWord);
                unsigned int ui = ul;
                if (ui != ul) throw std::out_of_range(line);

                // add the hex memory view of the new variable to result
                hexDumpWord<unsigned int>(ui, result);
                break;
            }

            case lineType::float_declaration:
            {
                // create float variable
                float f = std::stof(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<float>(f, result);
                break;
            }

            case lineType::double_declaration:
            {
                // create double variable
                double d = std::stod(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<double>(d, result);
                break;
            }

            case lineType::string_declaration:
            {
                // get the text in quotes
                std::string text = getTextInQuotes(line);

                // add the hex memory view of the char array to result
                hexDumpCharArray(text.c_str(), result);
                break;
            }

            case lineType::wildcard_line:
            {
                // remove last character (B) from lastWord variable
                if (!lastWord.empty())
                    lastWord.pop_back();
                else throw std::invalid_argument("last word was empty. Line:\n" + line);
                // add question marks to result 
                for (int i = 0; i < std::stoi(lastWord); i++) {
                    result.push_back("??");
                }
                break;
            }

            case lineType::char_declaration:
            case lineType::signed_char_declaration:
            {
                // get the text in quotes
                std::string s = getTextInQuotes(line);
                if (s.length() != 1) throw std::out_of_range(line);

                // get the frist char from the string's char array
                const char* cptr = s.c_str();
                char c = cptr[0];

                // add the hex memory view of the new variable to result
                hexDumpWord<char>(c, result);
                break;
            }
            case lineType::unsigned_char_declaration:
            {
                // get the text in quotes
                std::string s = getTextInQuotes(line);
                if (s.length() != 1) throw std::out_of_range(line);

                // get the frist char from the string's char array
                const char* cptr = s.c_str();
                char sc = cptr[0];
                if (sc < 0) throw std::out_of_range(line);
                unsigned char uc = sc;

                // add the hex memory view of the new variable to result
                hexDumpWord<char>(uc, result);
                break;
            }

            case lineType::short_int_declaration:
            case lineType::signed_short_int_declaration:
            {
                // create the new variable
                int i = std::stoi(lastWord);
                signed short s = i;
                if (s != i) throw std::out_of_range(line);

                // add the hex memory view of the new variable to result
                hexDumpWord<signed short>(s, result);
                break;
            }
            case lineType::unsigned_short_int_declaration:
            {
                // create the new variable
                unsigned long ul = std::stoul(lastWord);
                unsigned short us = ul;
                if (us != ul) throw std::out_of_range(line);

                // add the hex memory view of the new variable to result
                hexDumpWord<unsigned short>(us, result);
                break;
            }

            case lineType::long_int_declaration:
            case lineType::signed_long_int_declaration:
            {
                // create the new variable
                signed long l = std::stol(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<signed long>(l, result);
                break;
            }
            case lineType::unsigned_long_int_declaration:
            {
                // create the new variable
                unsigned long ul = std::stoul(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<unsigned long>(ul, result);
                break;
            }

            case lineType::long_long_int_declaration:
            {
                // create the new variable
                long long l = std::stoll(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<long long>(l, result);
                break;
            }
            case lineType::unsigned_long_long_int_declaration:
            {
                // create the new variable
                unsigned long long ull = std::stoull(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<unsigned long long>(ull, result);
                break;
            }

            case lineType::long_double_declaration:
            {
                // create the new variable
                long double d = std::stold(lastWord);

                // add the hex memory view of the new variable to result
                hexDumpWord<long double>(d, result);
                break;
            }

            case lineType::wchar_t_declaration:
            {
                // get the text in quotes
                std::string s = getTextInQuotes(line);
                if (s.length() != 1) throw std::out_of_range(line);

                // get the frist char from the string's char array
                const char* cptr = s.c_str();
                char c = cptr[0];
                wchar_t wc = c;

                // add the hex memory view of the new variable to result
                hexDumpWord<wchar_t>(wc, result);
                break;
            }
            case lineType::char16_t_declaration:
            {
                // get the text in quotes
                std::string s = getTextInQuotes(line);
                if (s.length() != 1) throw std::out_of_range(line);

                // get the frist char from the string's char array
                const char* cptr = s.c_str();
                char c = cptr[0];
                char16_t wc = c;

                // add the hex memory view of the new variable to result
                hexDumpWord<char16_t>(wc, result);
                break;
            }
            case lineType::char32_t_declaration:
            {
                // get the text in quotes
                std::string s = getTextInQuotes(line);
                if (s.length() != 1) throw std::out_of_range(line);

                // get the frist char from the string's char array
                const char* cptr = s.c_str();
                char c = cptr[0];
                char32_t wc = c;

                // add the hex memory view of the new variable to result
                hexDumpWord<char32_t>(wc, result);
                break;
            }
            case lineType::emptyLine:
                // continue
                break;

            case lineType::invalidLine:
                throw std::invalid_argument("received invalid line:\n" + line);
                break;

            default:
                break;
            }
        }
        catch (const std::invalid_argument& e) {
            //caught invalid line
            throw;
        }

        });


    return result;
}

// return which type given line is
lineType recnognizeLine(std::string& line) {
    if (std::regex_match(line, std::regex("^(?:[0-9A-Fa-f]{2} ){0,31}[0-9A-Fa-f]{2}$"))) {
        //literal bytes
        return lineType::literal_bytes;
    }
    else if (std::regex_match(line, std::regex("((int)|(INT))( )+[+-]?[0-9]+"))) {
        //int declaration
        return lineType::int_declaration;
    }
    else if (std::regex_match(line, std::regex("((float)|(FLOAT))( )+[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"))) {
        //float declaration
        return lineType::float_declaration;
    }
    else if (std::regex_match(line, std::regex("((double)|(DOUBLE))( )+[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"))) {
        //double declaration
        return lineType::double_declaration;
    }
    else if (std::regex_match(line, std::regex("((string)|(STRING))( )+\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"|\\'([^\\'\\\\]*(\\\\.[^\\'\\\\]*)*)\\'"))) {
        //string
        return lineType::string_declaration;
    }
    else if (std::regex_match(line, std::regex("^(wildcard|WILDCARD)( )+[0-9]+(b|B)"))) {
        //wildcard
        return lineType::wildcard_line;
    }
    else if (std::regex_match(line, std::regex("(char|CHAR)( )+(\')(.)(\')"))) {
        //char
        return lineType::char_declaration;
    }
    else if (std::regex_match(line, std::regex("(unsigned char|UNSIGNED CHAR)( )+(\')(.)(\')"))) {
        //unsigned char
        return lineType::unsigned_char_declaration;
    }
    else if (std::regex_match(line, std::regex("(signed char|signed CHAR)( )+(\')(.)(\')"))) {
        //signed char
        return lineType::signed_char_declaration;
    }
    else if (std::regex_match(line, std::regex("((unsigned int)|(unsigned))( )+(\\+)?[0-9]+"))) {
        //unsigned int
        return lineType::unsigned_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((signed int)|(signed))( )+[+-]?[0-9]+"))) {
        //signed int
        return lineType::signed_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((short int)|(short))( )+[+-]?[0-9]+"))) {
        //short int
        return lineType::short_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((unsigned short int)|(unsigned short))( )+(\\+)?[0-9]+"))) {
        //unsigned short int
        return lineType::unsigned_short_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((signed short int)|(signed short))( )+[+-]?[0-9]+"))) {
        //signed short int
        return lineType::signed_short_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((long int)|(long))( )+[+-]?[0-9]+"))) {
        //long int
        return lineType::long_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((signed long int)|(signed long))( )+[+-]?[0-9]+"))) {
        //signed long int
        return lineType::signed_long_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((unsigned long int)|(unsigned long))( )+(\\+)?[0-9]+"))) {
        //unsigned long int
        return lineType::unsigned_long_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((long long int)|(long long))( )+[+-]?[0-9]+"))) {
        //long long int
        return lineType::long_long_int_declaration;
    }
    else if (std::regex_match(line, std::regex("((unsigned long long int)|(unsigned long long))( )+(\\+)?[0-9]+"))) {
        //unsigned long long int
        return lineType::unsigned_long_long_int_declaration;
    }
    else if (std::regex_match(line, std::regex("(long double)( )+[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"))) {
        //long double
        return lineType::long_double_declaration;
    }
    else if (std::regex_match(line, std::regex("(wchar_t|WCHAR_T)( )+(l')(.)(')"))) {
        //wchar_t
        return lineType::wchar_t_declaration;
    }
    else if (std::regex_match(line, std::regex("(char16_t|CHAR16_T)( )+(u')(.)(')"))) {
        //char16_t
        return lineType::char16_t_declaration;
    }
    else if (std::regex_match(line, std::regex("(char32_t|CHAR32_T)( )+(u')(.)(')"))) {
        //char32_t
        return lineType::char32_t_declaration;
    }
    else if (std::regex_match(line, std::regex("( )*"))) {
        return lineType::emptyLine;
    }
    else return lineType::invalidLine;
}

// adds strings representing bytes in memory at the loacation of the given word into the given string vector 
template<typename T>
inline void hexDumpWord(const T& word, std::vector<std::string>& addTo)
{
    // get dump
    std::vector<std::string> toAdd;
    const T* pt = &word;
    toAdd = hexDump(pt, sizeof(T));

    //append vector
    addTo.insert(std::end(addTo), std::begin(toAdd), std::end(toAdd));
    return;
}

// adds the memory view of a given char array to the string vector
void hexDumpCharArray(const char* array, std::vector<std::string>& addTo)
{
    int pos = 0;
    while (array[pos] != '\0') {
        hexDumpWord<char>(array[pos], addTo);
        pos++;
    }
    addTo.push_back("00");
}

// return a vector of strings wich represent how the given value looks in memory in hex
std::vector<std::string> hexDump(const void* mem, unsigned int size) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(mem);
    std::vector<std::string> result;

    for (int i = size - 1; i >= 0; i--) {
        auto s = int_to_hex<int>(int(p[i]));
        result.push_back(s);
    }

    return result;
}

// returns the last word in the string (seperator is space)
std::string getLastWord(std::string str)
{
    std::vector<std::string> strings;
    std::istringstream f(str);
    std::string s;
    while (getline(f, s, ' ')) {
        strings.push_back(s);
    }
    if (!(strings.empty()))return strings.back();
    else return "";
}

// split the given string with the given delimiter. Note splitString("abc","a") results with just "bc"
std::vector<std::string> splitString(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

// return the text inside the outermost quotes ( " or ' ). getTextInQuotes("string \"you\'re \'ok\'\"") returns "you're 'ok'"
std::string getTextInQuotes(const std::string& str)
{
    int pos = 0;
    while (str[pos] != '\"' && str[pos] != '\'') {
        pos++;
    }
    return str.substr(pos + 1, str.length() - 2 - pos);
}

// returns converted integer type to hex string. 9 -> 09, 10 -> 0A, 16 -> 10
template<typename T>
std::string int_to_hex(T i)
{
    std::stringstream stream;
    stream << std::hex << i;
    std::string result = stream.str();
    if (result.length() == 1) result = "0" + result;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

static bool mempat(int argc, char* argv[]) {
	if (argc != 2)
	{
		dputs("usage: \"mempat line\"\n");
		return false;
	}
    std::vector<std::string> inputs;
    std::string str = argv[1];
    inputs.push_back(std::string(argv[1]));
    std::string result = createOutput(inputs);
    dprintf("%s\n", result.c_str());

	return true;

}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, "mempat", mempat, false);
    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{

}
