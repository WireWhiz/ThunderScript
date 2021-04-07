#pragma once
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <stack>
#include "ThunderScript.h"

namespace ts
{
	// Thunder Script Reserved Words
	constexpr std::array<const std::string_view, 3> tsReservedWords = {
		// Best practices, put the longer ones first just in case
		"return", //To be removed, will be replaced with out structs
		"def",
		"float",
	};
	constexpr unsigned int GetIndexOfReservedWord(const char token[])
	{
		for (unsigned int i = 0; i < tsReservedWords.size(); i++)
		{
			if (token == tsReservedWords[i])
				return i;
		}
		return 0;
	}

	// Thunder Script Operators
	constexpr std::array<const std::string_view, 14> tsOperators = {
		// Longer Operators must be defined first or they will be mistaken for shorter ones
		"++", "--", "+=", "-=",
		"(", ")", "{", "}", "[", "]",
		";", "+", "-", "="
	};
	constexpr unsigned int GetIndexOfOperator(const char token[])
	{
		int output = 0;
		for (unsigned int i = 0; i < tsOperators.size(); i++)
		{
			if (token == tsOperators[i])
			{
				output = i;
				break;
			}
		}
		return output;
	}

	class tsToken
	{
	public:
		enum class Type
		{
			tsOperator,
			tsReservedWord,
			tsIdentifier,
		};
		Type type;
		std::string token;
		int tokenIndex;
		tsToken(Type _type, std::string& _token, int _tokenIndex)
		{
			type = _type;
			token = _token;
			tokenIndex = _tokenIndex;

		}
	};

	class tsParser
	{
	public:
		
	};

	class tsCompiler
	{
		tsContext* _context;
		std::vector<std::string> fVars;

	public:
		bool compile(const std::string& path, tsContext* context)
		{
			// Read the script file. Return if we can't find it.
			std::ifstream f;
			f.open(path);
			if (!f.is_open())
				return false;

			//Read the entire script into a string
			char c;
			std::string scriptText = "";
			while (f.get(c))
				scriptText += c;

			// Release the file
			f.close();

			tsScript script;
			removeComments(scriptText);
			processPreprocessors(scriptText, script);
			std::vector<tsToken> tokens;
			parseTokens(scriptText, tokens);

			std::cout << "Tokens: " << std::endl << std::endl;
			for (size_t i = 0; i < tokens.size(); i++)
			{
				std::cout << "Type: " << (unsigned int)tokens[i].type << " Token: " << tokens[i].token << std::endl;
			}
			_context = context;
			GenerateBytecode(tokens, script);

			context->scripts.push_back(script);

			return true;
		}

	private:

	#pragma region PreProcessing
		void processPreprocessors(std::string& scriptText, tsScript& script)
		{
			for (size_t i = 0; i < scriptText.length(); i++)
			{
				//just remove all of them for now
				if (scriptText[i] == '#')
				{
					scriptText.erase(i, scriptText.find("\n", i) - i);
					scriptText.insert(i, " ");
				}
			}
		}

		void removeComments(std::string& scriptText)
		{
			size_t len = scriptText.length();

			for (size_t i = 0; i < len - 1; i++)
			{

				std::string key({ scriptText[i],scriptText[i + 1] });
				if (key == "//")
				{
					std::cout << "found // comment" << std::endl;
					scriptText.erase(i, scriptText.find("\n", i) - i);
					scriptText.insert(i, " ");
					len = scriptText.length();
				}
				else if (key == "/*")
				{
					std::cout << "Found /**/ comment" << std::endl;
					scriptText.erase(i, scriptText.find("*/", i) + 2 - i);
					scriptText.insert(i, " ");
					len = scriptText.length();
				}
			}
		}
	#pragma endregion

	#pragma region Parser
		static void parseTokens(const std::string& scriptText, std::vector<tsToken>& tokens)
		{

			tokens.clear();
			for (size_t i = 0; i < scriptText.length(); i++)
			{
				char currentChar = scriptText[i];// First we check if the current token is equal to any operators
				std::string token = "";
				int tokenIndex = 0;
				//Did we find the start of a token?
				if (currentChar != ' ' && currentChar != '\n')
				{
					// Is it an operator?
					if (isOperator(i, scriptText, tokenIndex))
					{
						token = tsOperators[tokenIndex];

						// If it is add it to the vector and increment i
						tokens.push_back(tsToken(tsToken::Type::tsOperator, token, tokenIndex));
						i += token.length() - 1;
					}
					// Then if it wasn't an operator check if it was a reserved word
					else if (isReservedWord(i, scriptText, tokenIndex))
					{
						token = tsReservedWords[tokenIndex];
						// If it is add it to the vector and increment i
						tokens.push_back(tsToken(tsToken::Type::tsReservedWord, token, tokenIndex));
						i += token.length() - 1;
					}
					// If it was not ether of those then it's an identifier or literal of some type
					else
					{
						//Loop until we find a character that we know is not part of an identifier or literal
						int endTokenIndex = 0;
						while (i < scriptText.length()
							   && !(scriptText[i] == ' '
									|| scriptText[i] == '\n'
									|| isOperator(i, scriptText, endTokenIndex)))
						{
							token += scriptText[i];
							i++;
						}
						// We incremented one past our desired end point, we need to step back so the next token is parsed on the next loop.
						i--;
						// When the loop ends we know we have the whole token so we can add it to the array
						massert(token != "" && token != " ", "Somehow an empty identifier was found. This is wrong.");
						tokens.push_back(tsToken(tsToken::Type::tsIdentifier, token, -1));
						// If we found an end token we migh as well save ourselves some time and add it as well
						if (endTokenIndex >= 1)
						{
							std::string endToken = (std::string)tsOperators[endTokenIndex];
							i += endToken.length();
							tokens.push_back(tsToken(tsToken::Type::tsOperator, endToken, endTokenIndex));
						}
					}
				}

			}

		}
		static bool isOperator(size_t start, const std::string& script, int& token)
		{
			
			massert(start < script.length(), "Start must be less then script.length()");
			for (int o = 0; o < tsOperators.size(); o++)
			{
				size_t length = tsOperators[o].length();
				if (length + start - 1 < script.length()
					&& script.substr(start, length) == tsOperators[o])
				{
					token = o;
					return true;
				}
			}
			return false;
		}
		static bool isReservedWord(size_t start, const std::string& script, int& token)
		{
			
			for (int w = 0; w < tsReservedWords.size(); w++)
			{
				size_t length = tsReservedWords[w].length();
				if (length + start < script.length()
					&& script.substr(start, length) == tsReservedWords[w])
				{
					token = w;
					return true;
				}
			}
			return false;
		}
	#pragma endregion

	#pragma region Bytecode Generation

		void GenerateBytecode(std::vector<tsToken> tokens, tsScript& script)
		{
			
			for (size_t i = 0; i < tokens.size(); i++)
			{
				massert(tokens[i].type == tsToken::Type::tsReservedWord, "token was not of type reserved word, this means you haven't coded in error checking yet and you forgot how your own language works.");
				switch (tokens[i].tokenIndex)
				{
					case GetIndexOfReservedWord("def"):
						fVars.clear();
						GenerateFunction(i, tokens, script);
						break;
					default:
						massert(false, std::string("You need to code in another reserved word. (Token index was: ") + std::to_string(tokens[i].tokenIndex) + ")");
						break;
				}
			}
		}

		void GenerateFunction(size_t& i, std::vector<tsToken> tokens, tsScript& script)
		{
			massert(tokens[i].token == "def", "Generate function called on " + tokens[i].token);
			i++;
			tsFunction function(tokens[i].token);

			//For now we're going to ignore the paramaters of the function, because I'm impataent and want to get something working.
			GetFunctionParams(++i, tokens, function);
			GenerateStatement(i, tokens, function.bytecode);

			script.functions.push_back(function);
		}

		void GetFunctionParams(size_t& i, std::vector<tsToken> tokens, tsFunction& function)
		{
			massert(tokens[i].token == "(", "Paramater block must start with a (");
			i++;
			while (i < tokens.size() && tokens[i].token != ")")
			{
				//paramater stuff here;
				i++;
			}
			massert(tokens[i].token == ")", "Paramater block must end with and )");
			i++;
		}
		void GenerateStatement(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			while (i < tokens.size() && tokens[i].token != "}")
			{
				std::cout << "First token in statement: " << tokens[i].token;
				switch (tokens[i].type)
				{
					case tsToken::Type::tsOperator:
						switch (tokens[i].tokenIndex)
						{
							case GetIndexOfOperator("{"):
							{

								//Store how many varibles are in the parent scope
								unsigned int fVarCount = fVars.size();

								//enter a new scope
								GenerateStatement(++i, tokens, bytecode);

								//Delete the varible ids from the last scope
								fVars.resize(fVarCount);
								break;
							}
							case GetIndexOfOperator(";"):
								break;
							default:
								massert(false, "Unknown statement operator: " + std::to_string(tokens[i].tokenIndex));
								//GenerateExpression(0, i, tokens, bytecode);
								break;
						}
						break;
						
					case tsToken::Type::tsReservedWord:
						switch (tokens[i].tokenIndex)
						{
							case GetIndexOfReservedWord("float"):
								GenerateFloat(i, tokens, bytecode);
								break;
							case GetIndexOfReservedWord("return"):
								GenerateReturn(i, tokens, bytecode);
								break;
							default:
								massert(false, "Unknown reserved word: " + tokens[i].token);
								//GenerateExpression
								break;
						}
						break;
						
					default:
						GenerateExpression(i, tokens, bytecode);
						break;
				}
				++i;
			}
			
			
		}

		void GenerateExpression(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			switch (tokens[i].type)
			{
				case tsToken::Type::tsOperator:
				{
					switch (tokens[i].tokenIndex)
					{

						case GetIndexOfOperator("="):
							break;
					}
				}
					break;
				case tsToken::Type::tsIdentifier:
				{

					unsigned int idIndex = 0;
					if (GetIndexOfFloat(tokens[i].token, idIndex))
					{
						switch(tokens[++i].tokenIndex)
						{
							case GetIndexOfOperator(";"):
								bytecode.LOADF(idIndex, 1);
								break;
							case GetIndexOfOperator("+"):
								GenerateExpression(++i, tokens, bytecode);
								bytecode.LOADF(idIndex, 0);
								bytecode.ADDF(1);
								break;
							case GetIndexOfOperator("-"):
								GenerateExpression(++i, tokens, bytecode);
								bytecode.LOADF(idIndex, 0);
								bytecode.FLIPF(1);
								bytecode.ADDF(1);
								break;
							case GetIndexOfOperator("="):
								GenerateExpression(++i, tokens, bytecode);
								bytecode.STOREF(1, idIndex);
								break;
						}
					}
					else if (GetIndexOfFunction(tokens[i].token, idIndex))
					{

					}
					else
					{
						bool isConst = false;;
						try
						{
							float value = std::stof(tokens[i].token);
							isConst = true;

							switch (tokens[++i].tokenIndex)
							{
								case GetIndexOfOperator(";"):
								{
									bytecode.READF(1, value);
									return;
									break;
								}
								case GetIndexOfOperator("+"):
									GenerateExpression(++i, tokens, bytecode);
									bytecode.READF(0, value);
									bytecode.ADDF(1);
									break;
								case GetIndexOfOperator("-"):
									GenerateExpression(++i, tokens, bytecode);
									bytecode.READF(0, value);

									bytecode.FLIPF(1);
									bytecode.ADDF(1);
									break;
								default:
									break;
							}
						}
						catch (const std::invalid_argument& ia)
						{
							
						}
						massert(isConst, "Unknown identifier: " + tokens[i].token + " Token index: " + std::to_string(i));
					}

					break;
				}
				case tsToken::Type::tsReservedWord:
					massert(false, "You have not added any reserved words for expressions yet");

					break;
				default:
					massert(false, std::string("Invalid expression type: ") + std::to_string((int)tokens[i].type));
					break;
			}
			
		}

		

		bool GetIndexOfFloat(const std::string& id, unsigned int& index)
		{
			for (unsigned int i = 0; i < fVars.size(); i++)
			{
				if (fVars[i] == id)
				{
					index = i;
					return true;
				}
			}
			return false;
		}

		bool GetIndexOfFunction(const std::string& id, unsigned int& index)
		{
			return false;
		}
		
		void GenerateFloat(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "float");
			unsigned int index = fVars.size();
			fVars.push_back(tokens[++i].token);
			bytecode.ALLOCATE(tsVariable::Type::tsFloat);
			if (tokens[++i].token != ";")
				GenerateExpression(--i, tokens, bytecode);
			else
				++i;
		}

		void GenerateReturn(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			assert(tokens[i].token == "return");
			GenerateExpression(++i, tokens, bytecode);
			bytecode.RETURNF();
		}

		void GenerateStruct(size_t& i, std::vector<tsToken> tokens, tsBytecode& bytecode)
		{

		}
		
	#pragma endregion

	};



}