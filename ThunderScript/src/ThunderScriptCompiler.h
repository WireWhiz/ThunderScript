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
	constexpr std::array<const std::string_view, 16> tsOperators = {
		// Longer Operators must be defined first or they will be mistaken for shorter ones
		"++", "--", "+=", "-=",
		"(", ")", "{", "}", "[", "]",
		";", "+", "-", "=", "*", "/"
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

	
	// 0: variables, 1: add and subtract, 2: multiply and divide, 3: negate, 4: scope
	class tsOperation
	{
	public:
		enum class DepSide
		{
			none,
			left, 
			right,
			both
		};
		virtual unsigned int GetPriority() = 0;
		virtual DepSide GetDepSide()
		{
			return DepSide::none;
		};
		virtual unsigned int  GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex) = 0;
	};

	// An operation dependant upon one other operation
	class tsDOperator : public tsOperation
	{
	protected:
		std::unique_ptr<tsOperation> _dep;
	public:
		void SetDep(std::unique_ptr<tsOperation>& dep)
		{
			_dep = std::move(dep);
		}
	};

	// An operator with a dependancy on the left
	class tsLOperator : public tsDOperator
	{
	public:
		DepSide GetDepSide()
		{
			return DepSide::left;
		}
	};
	// An operator with a dependancy on the right
	class tsROperator : public tsDOperator
	{
	public:
		DepSide GetDepSide()
		{
			return DepSide::right;
		}
	};

	// An operation that has two dependancies
	class tsDDOperator : public tsOperation
	{
	protected:
		std::unique_ptr<tsOperation> _dep1;
		std::unique_ptr<tsOperation> _dep2;
	public:
		DepSide GetDepSide()
		{
			return DepSide::both;
		}
		void SetDeps(std::unique_ptr<tsOperation>& dep1, std::unique_ptr<tsOperation>& dep2)
		{
			std::cout << "Input deps null: " << (dep1.get() == nullptr) << " and " << (dep1.get() == nullptr) << std::endl;
			massert((dep1.get() != nullptr) && (dep2.get() != nullptr), "Input pointer(s) were null")
			_dep1 = std::move(dep1);
			_dep2 = std::move(dep2);
			std::cout << "Deps null: " << (_dep1.get() == nullptr) << " and " << (_dep2.get() == nullptr) << std::endl;
		}
	};

	
	class tsScopeOperation : public tsOperation
	{
	protected:
		std::vector<std::unique_ptr<tsOperation>> operations;
	public:
		std::vector<std::unique_ptr<tsOperation>>& Operations()
		{
			return operations;
		}
		unsigned int GetPriority()
		{
			return 5;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			
			unsigned int localWorkingVar = bytecode.ALLOCATEF();
			for (size_t i = 0; i < operations.size(); i++)
			{
				operations[i]->GetValueIndex(bytecode, localWorkingVar);
			}
			return localWorkingVar;
		}
	};
	

	class tsFloatOperation : public tsOperation
	{
	public:
		unsigned int varIndex;
		tsFloatOperation(unsigned int index)
		{
			varIndex = index;
		}
		unsigned int GetPriority()
		{
			return 1;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			std::cout << "Getting value index of float" << std::endl;
			return varIndex;
		}
	};

	class tsFloatAssignOperation : public tsDDOperator
	{
	public:
		unsigned int GetPriority()
		{
			return 0;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			bytecode.PUSH();
			bytecode.MOVEF(_dep2->GetValueIndex(bytecode, workingVarIndex), _dep1->GetValueIndex(bytecode, 1));
			bytecode.POP();
			return workingVarIndex;
		}
	};

	class tsAddOperation : public tsDDOperator
	{
	public:
		unsigned int GetPriority()
		{
			return 2;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			std::cout << "Getting value index of add operation" << std::endl;
			massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
			bytecode.ADDF(_dep1->GetValueIndex(bytecode, workingVarIndex), _dep2->GetValueIndex(bytecode, 1), workingVarIndex);
			return workingVarIndex;
		}
	};

	class tsNegateOperation : public tsROperator
	{
		unsigned int GetPriority()
		{
			return 4;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			bytecode.MOVEF(_dep->GetValueIndex(bytecode, workingVarIndex), workingVarIndex);
			bytecode.FLIPF(workingVarIndex);
			return workingVarIndex;
		}
	};

	class tsSubtractOperation : public tsDDOperator
	{
	public:
		unsigned int GetPriority()
		{
			return 2;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			std::cout << "Getting value index of subtract operation" << std::endl;
			massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
			bytecode.MOVEF(_dep2->GetValueIndex(bytecode, workingVarIndex), 1);
			bytecode.FLIPF(1);
			bytecode.ADDF(_dep1->GetValueIndex(bytecode, workingVarIndex), 1, workingVarIndex);
			return workingVarIndex;
		}
	};

	class tsMultiplyOperation : public tsDDOperator
	{
		unsigned int GetPriority()
		{
			return 3;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			std::cout << "Getting value index of multiply operation" << std::endl;
			massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
			bytecode.MULF(_dep1->GetValueIndex(bytecode, workingVarIndex), _dep2->GetValueIndex(bytecode, workingVarIndex), workingVarIndex);
			return workingVarIndex;
		}
	};

	class tsDivideOperation : public tsDDOperator
	{
		unsigned int GetPriority()
		{
			return 3;
		}
		unsigned int GetValueIndex(tsBytecode& bytecode, const unsigned int& workingVarIndex)
		{
			std::cout << "Getting value index of divide operation" << std::endl;
			massert((_dep1.get() != nullptr) && (_dep2.get() != nullptr), "Dependancy pointer(s) were null " + std::to_string(_dep1.get() != nullptr) + " " + std::to_string(_dep2.get() != nullptr));
			bytecode.DIVF(_dep1->GetValueIndex(bytecode, workingVarIndex), _dep2->GetValueIndex(bytecode, workingVarIndex), workingVarIndex);
			return workingVarIndex;
		}
	};

	class tsCompiler
	{
		std::shared_ptr<tsContext> _context;
		std::vector<std::string> fVars;

	public:
		bool compile(const std::string& path, std::shared_ptr<tsContext>& context)
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


			fVars.clear();
			fVars.emplace_back("storage var");// We know this will never be confused for a user defined varible because they cannot use spaces.
			fVars.emplace_back("working var");
			function.bytecode.ALLOCATEF();
			function.bytecode.ALLOCATEF();

			//For now we're going to ignore the paramaters of the function, because I'm impataent and want to get something working.
			GetFunctionParams(++i, tokens, function);
			GenerateConstVars(i, tokens, function.bytecode);
			GenerateStatement(i, tokens, function.bytecode);

			script.functions.push_back(function);
		}
		void GenerateConstVars(size_t& i, std::vector<tsToken>& tokens, tsBytecode& bytecode)
		{
			for(unsigned int token = 0; token < tokens.size(); token++)
			{
				if (tokens[token].type == tsToken::Type::tsIdentifier)
				{
					try
					{
						float value = std::stof(tokens[token].token); // try to cast string to float, if it fails it is not a float
						std::cout << "Found const float: " << value << std::endl;
						bool foundVar = false;
						std::string key = "const float " + std::to_string(value);
						for (unsigned int var = 0; var < fVars.size() && !foundVar; var++)
						{
							if (key == fVars[var])
							{
								std::cout << "Found predifined const float" << std::endl;
								foundVar = true;
							}
						}
						if (!foundVar)
						{
							std::cout << "Made new const float" << std::endl;
							unsigned int varIndex = fVars.size();
							bytecode.ALLOCATEF();
							bytecode.LOADF(varIndex, value);
							fVars.push_back(key);
						}
						tokens[token].token = key;
					}
					catch (...)
					{

					}
				}
			}
		}

		void GetFunctionParams(size_t& i, const std::vector<tsToken>& tokens, tsFunction& function)
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
		void GenerateStatement(size_t& i, const std::vector<tsToken> tokens, tsBytecode& bytecode)
		{
			while (i < tokens.size() && tokens[i].token != "}")
			{
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
								std::cout << "Generating statement" << std::endl;
								GenerateStatement(++i, tokens, bytecode);
								//Delete the varible ids from the last scope
								fVars.resize(fVarCount);
								break;
							}
							case GetIndexOfOperator(";"):
								break;
							default:
								massert(false, "Unknown statement operator: " + tokens[i].token);
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
						std::cout << i << std::endl;
						break;
				}
				++i;
			}
			
			
		}

		void GenerateExpression(size_t& i, const std::vector<tsToken>& tokens, tsBytecode& bytecode)
		{
			std::cout << "Generating expression" << std::endl;
			std::vector<std::unique_ptr<tsOperation>> operations;
			size_t end = 0;
			while (tokens[i + end].token != ";") ++end;
			std::cout << "expression is " << end << " long " << std::endl;
			std::vector opTokens(tokens.begin() + i, tokens.begin() + i + end);
			GenerateOperations(opTokens, operations);

			for (size_t op = 0; op < operations.size(); op++)
			{
				std::cout << "ran get value index" << std::endl;
				operations[op]->GetValueIndex(bytecode, 0);
			}
			i += end;
			std::cout << "next token after expression: " << tokens[i].token << std::endl;
			std::cout << i << std::endl;

		}

		void GenerateOperations(const std::vector<tsToken>& tokens, std::vector<std::unique_ptr<tsOperation>>& operations)
		{
			for (size_t i = 0; i < tokens.size(); i++)
			{
				tsToken token = tokens[i];
				if (token.type == tsToken::Type::tsIdentifier)
				{
					unsigned int index;
					if (GetIndexOfFloat(token.token, index))
					{
						std::cout << "Found var" << std::endl;
						operations.push_back(std::make_unique<tsFloatOperation>(index));
					}
				}
				else if (token.type == tsToken::Type::tsOperator)
				{

					switch (tokens[i].tokenIndex)
					{
						case GetIndexOfOperator("("):
						{
							std::cout << "Found (" << std::endl;
							int pCount = 1;
							std::vector<tsToken> subTokens;
							i++;
							while (pCount > 0)
							{
								if (tokens[i].token == "(")
									pCount++;
								else if (tokens[i].token == ")")
									pCount--;
								if (pCount > 0)
								{
									subTokens.push_back(tokens[i]);
								}
								i++;
							}
							std::unique_ptr<tsScopeOperation> subExpression = std::make_unique<tsScopeOperation>();
							GenerateOperations(subTokens, subExpression->Operations());
							operations.push_back(std::move(subExpression));
						}
							break;
						case GetIndexOfOperator("+"):
							std::cout << "Found +" << std::endl;
							operations.push_back(std::make_unique<tsAddOperation>());
							break;
						case GetIndexOfOperator("-"):
							std::cout << "Found -" << std::endl;
							if(i == 0 || (tokens[i - 1].type == tsToken::Type::tsOperator && tokens[i - 1].token != ")"))
								operations.push_back(std::make_unique<tsNegateOperation>());
							else
								operations.push_back(std::make_unique<tsSubtractOperation>());
							break;
						case GetIndexOfOperator("*"):
							std::cout << "Found *" << std::endl;
							operations.push_back(std::make_unique<tsMultiplyOperation>());
							break;
						case GetIndexOfOperator("/"):
							std::cout << "Found /" << std::endl;
							operations.push_back(std::make_unique<tsDivideOperation>());
							break;
						case GetIndexOfOperator("="):
							std::cout << "Found =" << std::endl;
							operations.push_back(std::make_unique<tsFloatAssignOperation>());
							break;
					}
				}
				else
					massert(false, "Unknown expression reserved word: " + tokens[i].token);
			}
			
			for (int priority = 5; priority >= 0; priority--)
			{
				for (unsigned int i = 0; i < operations.size(); i++)
				{
					if (operations[i] -> GetPriority() == priority)
					{
						switch (operations[i] -> GetDepSide())
						{
							case tsOperation::DepSide::both:
								std::cout << "Found double sided operator at index " << i << " at priority " << priority << std::endl;
								dynamic_cast<tsDDOperator*>(operations[i].get()) -> SetDeps(operations[i - 1], operations[i + 1]);
								
								// We need to erase these pointers as they are no longer assigned
								// Need to erase +1 first since otherwise the index for it would be off
								operations.erase(operations.begin() + i + 1);
								operations.erase(operations.begin() + i - 1);
								// Need to step back because we deleted and index before this one
								i--;
								break;
							case tsOperation::DepSide::right:
								std::cout << "Found right sided operator at index " << i << " at priority " << priority << std::endl;
								dynamic_cast<tsROperator*>(operations[i].get())->SetDep(operations[i + 1]);

								operations.erase(operations.begin() + i + 1);
								break;
							case tsOperation::DepSide::left:
								std::cout << "Found left sided operator at index " << i << " at priority " << priority << std::endl;
								dynamic_cast<tsROperator*>(operations[i].get())->SetDep(operations[i - 1]);

								operations.erase(operations.begin() + i - 1);
								// Need to step back because we deleted and index before this one
								i--;
								break;
						}
					}
				}
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
			fVars.push_back(tokens[++i].token);
			bytecode.ALLOCATEF();
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