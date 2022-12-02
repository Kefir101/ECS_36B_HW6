
// ecs36b first program

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ecs36b_Exception.h"

// JSON RPC part
#include "hw6client.h"
#include <jsonrpccpp/client/connectors/httpclient.h>

#define WORD_LENGTH 5
#define MAX_TRIES 10
#define ALPHA_SIZE 27
#define SOLUTION_WORD_COUNT 2315

#ifdef WIN32
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

#define err(x)                                                                 \
	fprintf(stderr, EOL "[%s:%i] Fatal error: %s" EOL, __FILE__, __LINE__, x); \
	abort();

using namespace jsonrpc;
using namespace std;

int guesses = 2;
FILE *fpAll, *fpSol;
char guessedWord[WORD_LENGTH + 1] = {0};
int yellows = 0;
char guessedWords[MAX_TRIES + 1][WORD_LENGTH + 1] = {{0}};

char greenCharacters[WORD_LENGTH + 1] = "_____";
char yellowCharacters[WORD_LENGTH + 1] = {0};
int greyCharacters[ALPHA_SIZE - 1] = {0};

// http://127.0.0.1:8384" //hosting server locally
// https://2ada-73-66-168-157.ngrok.io //using class server
HttpClient httpclient("http://127.0.0.1:8384");
hw6Client myClient(httpclient, JSONRPC_CLIENT_V2);
Json::Value myv;
Json::Value jv_list;

void guessWord(const std::string &word);
void playGame();
void sendGuessToServer(const std::string &word);
void makeGuess(int guesses);
void printKnowledgeFromPastGuesses();
void updateKnowledge(const char *guess, const std::string &accuracy);

int main()
{
	jv_list[0] = "921214234";
	jv_list[1] = "921287685";
	jv_list[2] = "915430532";
	jv_list[3] = "919747807";
	try
	{
		// myv = myClient.set_name("set_name", jv_list, "BSSK");
		myv = myClient.obtain("obtain", "Wordle", "00000000");
		if (!myv["game_id"].isNull() && myv["game_id"].isString())
		{
			playGame();
			std::cout << myv.toStyledString() << std::endl;
		}
		myv = myClient.submit("submit", myv["game_id"].asString(), "BSSK");
	}
	catch (JsonRpcException &e)
	{
		cerr << e.what() << endl;
	}
	return 0;
}
void playGame()
{
	sendGuessToServer("crane");
	updateKnowledge("crane", myv["guesses"]["data"][0]["result"].asString());
	sendGuessToServer("light");
	updateKnowledge("light", myv["guesses"]["data"][1]["result"].asString());

	setbuf(stdout, NULL);
	fpAll = fopen("lists/ALL.TXT", "r");
	fpSol = fopen("lists/SOLUTION.TXT", "r");
	if (fpAll == NULL || fpSol == NULL)
	{
		err("error opening wordle lists");
	}
	char guess[WORD_LENGTH + 1] = {0};
	while (guesses < MAX_TRIES)
	{
		makeGuess(guesses);
		strncpy(guess, guessedWord, WORD_LENGTH);
		guess[WORD_LENGTH] = '\0';
		sendGuessToServer(guess);
		updateKnowledge(guess, myv["guesses"]["data"][guesses]["result"].asString());
		memcpy(guessedWords[guesses], guess, sizeof(guessedWords[guesses]));
		guesses++;
		// if guess is correct, stop guessing
		if (myv["guesses"]["data"][guesses - 1]["result"].asString() == "#####")
		{
			break;
		}
	}
	fclose(fpAll);
	fclose(fpSol);
}
void sendGuessToServer(const std::string &word)
{
	myv = myClient.guess("guess", "Wordle", myv["game_id"].asString(), word);
}
void makeGuess(int guesses)
{
	if(guesses > 0) printKnowledgeFromPastGuesses();
	// the line array acts as the word being guessed, and is later copied to the guess variable
	char line[6];
	// for the first two guesses, guess is predefined (based on my wordle strategy)
	if (guesses == 0)
	{
		memcpy(line, "crane", sizeof(line));
	}
	else if (guesses == 1)
	{
		memcpy(line, "light", sizeof(line));
	}
	else
	{
		int i = 0;
		fseek(fpSol, 0, SEEK_SET);
		// printf("\n\033[0;31m");
		while (i <= SOLUTION_WORD_COUNT && fgets(line, WORD_LENGTH + 1, fpSol) != NULL)
		{
			// for every word, if it is the right length, check that it meets three criteria:
			// has all the greens and yellows from all the previous guesses (so that it could be the correct word)
			// and isnt a word that has already been guessed
			if (strlen(line) == WORD_LENGTH)
			{
				// if(i < 10) printf("%d: %s, ", i, line);
				bool hasAllGreens = true;
				for (int i = 0; i < WORD_LENGTH + 1; i++)
				{
					// check if the line (word) contains the specified yellow character at the specific index
					if (greenCharacters[i] != '_' && greenCharacters[i] != line[i])
					{
						hasAllGreens = false;
						break;
					}
				}
				bool hasAllYellows = true;
				for (int i = 0; i < WORD_LENGTH + 1; i++)
				{
					if (yellowCharacters[i] != '_')
					{
						bool hasYellow = false;
						// check if the line (word) contains the specified yellow character
						for (int j = 0; j < WORD_LENGTH + 1; j++)
						{
							if (line[j] == yellowCharacters[i])
							{
								hasYellow = true;
								break;
							}
						}
						if (!hasYellow)
							hasAllYellows = false;
					}
				}
				bool alreadyGuessed = false;
				for (int i = 0; i < MAX_TRIES + 1; i++)
				{
					if (strcmp(line, guessedWords[i]) == 0)
					{
						alreadyGuessed = true;
					}
				}
				bool hasNoGreys = true;
				for (int i = 0; i < WORD_LENGTH; i++)
				{
					// check if the letter is grey
					if (greyCharacters[((int)line[i]) - 97] == 1)
					{
						hasNoGreys = false;
						break;
					}
				}

				// if word in SOLUTION.TXT but not in ALL.txt, don't guess that exact word but guess 'zonal'
				if (strcmp(line, "agora") == 0 || strcmp(line, "lynch") == 0 || strcmp(line, "slave") == 0 || strcmp(line, "pupal") == 0)
					;
				else if (hasAllGreens && hasAllYellows && !alreadyGuessed && hasNoGreys)
					break;
				i++;
			}
		}
		printf("\033[0m; ");
	}
	memcpy(guessedWord, line, sizeof(guessedWord));
	guesses++;
	printf("\nGuess %d: %s \n", guesses, guessedWord);
}
void printKnowledgeFromPastGuesses()
{
	printf("\n\033[0;32m");
	printf("Greens: ");
	for (int i = 0; i < WORD_LENGTH + 1; i++)
	{
		printf("%c", greenCharacters[i]);
	}
	printf("\033[0m; ");
	printf("\033[0;33m");
	printf("Yellows: ");
	for (int i = 0; i < WORD_LENGTH + 1; i++)
	{
		printf("%c", yellowCharacters[i]);
	}
	printf("\033[0m; ");
	printf("\033[0;90m");
	printf("Greys: ");
	for (int i = 0; i < ALPHA_SIZE - 1; i++)
	{
		if (greyCharacters[i] == 1)
		{
			printf("%c", i + 97);
		}
	}
	printf("\033[0m");
}
void updateKnowledge(const char *guess, const std::string &accuracy)
{
	int pos = -1;
	char result[WORD_LENGTH + 1];
	result[WORD_LENGTH] = 0;
	// Do all correct positions first
	for (int i = 0; i < accuracy.length(); i++)
	{
		if (accuracy[i] == '#')
		{
			greenCharacters[i] = guess[i];
		}
	}
	for (int i = 0; i < accuracy.length(); i++)
	{
		if (accuracy[i] == 'o')
		{
			bool charAlreadyFound = false;
			for (int j = 0; j < yellows; j++)
			{
				if (yellowCharacters[j] == guess[i])
				{
					charAlreadyFound = true;
					break;
				}
			}
			if (!charAlreadyFound)
			{
				yellowCharacters[yellows++] = guess[i];
			}
		}
	}
	int i = 0;
	while (guess[i])
	{
		bool isGreen = false;
		for (int j = 0; j < WORD_LENGTH + 1; j++)
		{
			if (greenCharacters[j] == guess[i])
			{
				isGreen = true;
				break;
			}
		}
		bool isYellow = false;
		for (int j = 0; j < WORD_LENGTH + 1; j++)
		{
			if (yellowCharacters[j] == guess[i])
			{
				isYellow = true;
				break;
			}
		}
		if (!isYellow && !isGreen)
			greyCharacters[(int)guess[i] - 97] = 1;
		++i;
	}
	// printing out the accuracy of the guess, but with green and yellow colors added
	  for(int i = 0; i < WORD_LENGTH + 1; i++){
	    if(result[i] != '_'){
	      if(result[i] == '#') printf("\033[0;32m");
	      if(result[i] == 'o') printf("\033[0;33m");
	      printf("%c", guess[i]);
	    }else {
	      printf("_");
	    }
	    printf("\033[0m");
	  }
}