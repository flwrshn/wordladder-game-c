/*
 * uqwordladder.c
 * Written by Shannon Dela Pena
 *
 * Please refer to prac directory for main comments. I was working in the 
 * wrong directory before and moved everything into ../trunk/a1 which lost most
 * of my comments.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <csse2310a1.h>

#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 9
#define DEFAULT_WORD_LENGTH 4
#define DEFAULT_STEPS 20
#define MAX_STEPS 55
#define DEFAULT_DICTIONARY "/usr/share/dict/words"

/*
 * A struct that defines a dictionary and the number of elements it contains
 */
typedef struct {
    int numLines;
    char** dictionary;
} Dictionary;

/* 
 * A struct that defines an option and a counter of that option.
 */
typedef struct {
    const char* option;
    int count;
} OptionCount;

/* invalid_command()
 * -----------------
 * Prints the usage error message to stderr
 *
 * argc: int - How many arguments are provided in the command line.
 * argv: char** - The arguments provided.
 *
 * Returns: void
 *
 * Errors: prints out the arguments provided through stderr and exits with
 *      status 11
 *
 * REF: The following block of code is teaken from code submitted by me from
 *      the Edlessons on Accessing program arguments (Week 2.1)
 */
void invalid_command(int argc, char** argv) {
    fprintf(stderr, "Usage: uqwordladder [--from initialWord]"
            " [--target toWord] [--limit maxSteps] [--length wordLen]"
            " [--dict filename]\n");
    exit(11);
}

/* is_all_letters()
 * ----------------
 * Checks if word contains only letters
 *
 * word: char* - To be checked if it is all letters
 *
 * Returns: int - 0 if it is not all letters and 1 if it is all letters
 */
int is_all_letters(const char* word) {
    int length = strlen(word);
    for (int i = 0; i < length; i++) {
        if (!isalpha(word[i])) {
            return 0;
        }
    }
    return 1;
}

/* is_numeric()
 * ------------
 * Checks if argument is numeric
 *
 * str: char* - To check if it is numeric
 *
 * Returns: int - 0 if it is not all digits and 1 if it is numeric
 */
int is_numeric(const char* str) {
    int length = strlen(str);
    for (int i = 0; i < length; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

/* check_word_length()
 * -------------------
 * Checks if the word length is consistent between word and the
 * number provided.
 * It prints an error message to stderr if the word length is inconsistent
 *
 * word: char* - To compare the specified length with
 * num: int - To check if the word haso the same length
 *
 * Returns: void
 * Error: Throws exit status 4 when the word lengths mismatch
 */
void check_word_length(const char* word, int num) {
    if (word && num != 0 && strlen(word) != num) {
        fprintf(stderr, "uqwordladder: Word length mismatch - lengths must be"
                " consistent\n");
        exit(4);
    }
}

/* print_welcome_message()
 * -----------------------
 * Prints the welcome message of the game and prints out the arguments
 * in uppercase.
 *
 * initialWord: char* - That is the beginning word
 * toWord: char* - That is the target word
 * maxSteps: int - That is the maximum amount of steps to get to the target
 *
 * Returns: void
 */
void print_welcome_message(const char* initialWord, const char* toWord,
        int maxSteps) {
    // Allocate memory for the string that has all uppercase
    char* upperInit = (char*)malloc(sizeof(char) * (strlen(initialWord) + 1));
    char* upperTarget = (char*)malloc(sizeof(char) * (strlen(toWord) + 1));

    for (int i = 0; initialWord[i]; i++) {
        upperInit[i] = toupper(initialWord[i]);
    }
    upperInit[strlen(initialWord)] = '\0';

    for (int i = 0; toWord[i]; i++) {
        upperTarget[i] = toupper(toWord[i]);
    }
    upperTarget[strlen(toWord)] = '\0';

    printf("Welcome to UQWordLadder!\n"
            "Your goal is to turn '%s' into '%s' in at most %i steps\n",
            upperInit, upperTarget, maxSteps);

    // Free the memory
    free(upperInit);
    free(upperTarget);
}

/* process_words()
 * ---------------
 * Function that checks if the words given are all letters and stores them
 * to the location of initialWord and toWord.
 *
 * argc: int - The number of arguments the user has provided including
 *      the program name
 * argv: char** - The arguments in command line
 * initialWord: char** - A pointer to a pointer to modify the actual pointer
 *      value. initialWord stores the memory address where a string is stored.
 * toWord: char** - The same as above.
 *
 * Return: void
 */
void process_words(int argc, char** argv, const char** initialWord,
        const char** toWord) {
    for (int i = 1; i < argc; i++) {
        // Checks if the --from and --target argument is given and the
        // associated value
        if (strcmp("--from", argv[i]) == 0 && (i + 1 < argc)) {
            *initialWord = argv[i + 1];
        } else if (strcmp("--target", argv[i]) == 0 && (i + 1 < argc)) {
            *toWord = argv[i + 1];
        }
    }
}

/* process_length()
 * ----------------
 * Function that stores the length provided and checks if the length is numeric
 * and is a positive decimal integer.
 *
 * argc: int - The number of command line arguments
 * argv: char** - The command line arguments
 * num: int* - A pointer to the word length
 *
 * Return: void
 * Errors: Throws error when is_numeric() is false or its not positive
 */
void process_numbers(int argc, char** argv, char* arg, int* num) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(arg, argv[i]) == 0 && i + 1 < argc) {
            if (is_numeric(argv[i + 1])) {
		// REF: This function is inspired from the strtol() man page
		char* ptr;
		long number = strtol(argv[i + 1], &ptr, 10);
		if (number < 1 || *ptr != '\0') {
		    invalid_command(argc, argv);
		}
		*num = (int)number;
            } else {
                invalid_command(argc, argv);
            }
        }
    }
}

/* process_dictionary()
 * --------------------
 * Function that stores the dictionary file name
 *
 * argc: int - The number of command lines
 * argv: char** - The command line arguments
 * dictfilename: char** - The dictionary file name to be stored
 *
 * Returns: void
 */
void process_dictionary(int argc, char** argv, const char** dictFilename) {
    for (int i = 1; i < argc; i++) {
        if (strcmp("--dict", argv[i]) == 0 && i + 1 < argc) {
            *dictFilename = argv[i + 1];
        }
    }
}

/* set_word_length()
 * -----------------
 * Function that sets the word length.
 * If none of the parameters are filled then the length is set to the default
 *
 * num: int* - A pointer to the word length
 * initialWord: char* - A pointer to the initial word
 * toWord: char* - A pointer to the target word
 *
 * Return: void
 * Errors: If the word length is not in the range then program exits with
 *      status 4
 */
void set_word_length(int* num, const char* initialWord, const char* toWord) {
    if (*num == 0 && !initialWord && !toWord) {
        *num = DEFAULT_WORD_LENGTH;
    } else if (*num == 0 && initialWord) {
        *num = strlen(initialWord);
    } else if (*num == 0 && toWord) {
        *num = strlen(toWord);
    }

    // Checks if word length is in range 2 and 9 inclusive
    if (*num < MIN_WORD_LENGTH || *num > MAX_WORD_LENGTH) {
        fprintf(stderr, "uqwordladder: Word length must be between 2 and 9 "
                "(inclusive)\n");
        exit(7);
    }
}

/* check_for_non_letters()
 * -----------------------
 * Function that throws error message if the words provided contain
 * non-letters
 *
 * initialWord: char* - The initial word
 * toWord: char* - The target word
 *
 * Returns: void
 * Errors: Prints out to stderr that words should not contain non-letters
 */
void check_for_non_letters(const char* initialWord, const char* toWord) {
    if (!is_all_letters(initialWord) || !is_all_letters(toWord)) {
        fprintf(stderr, "uqwordladder: Words should not contain "
                "non-letters\n");
        exit(3);
    }
}

/* compare_words()
 * ---------------
 * This compares two words if they are the same regardless of case
 *
 * initialWord: char* - The first word
 * toWord: char* - The second word
 *
 * Return: void
 * Errors: Exits with status 5 if the two words are the same
 */
void compare_words(const char* initialWord, const char* toWord) {
    if (strcasecmp(initialWord, toWord) == 0) {
        fprintf(stderr, "uqwordladder: Start and end words must not be the"
                " same\n");
        exit(5);
    }
}

/* check_step_bounds()
 * -------------------
 * This compares if the maximum step bounds are between the word length and
 * 55
 *
 * maxSteps: int - The max steps set for the game
 * num: int - The length of the word
 *
 * Return: void
 * Error: Error is thrown if the step limit is not in range
 */
void check_step_bounds(int maxSteps, int num) {
    if (maxSteps < num || maxSteps > MAX_STEPS || maxSteps < 0) {
        fprintf(stderr, "uqwordladder: Step limit must be word length"
                " to 55 (inclusive)\n");
        exit(6);
    }
}

/* isValidOption()
 * ---------------
 * Checks if the argument is a valid option.
 *
 * option: char* - this is the option word 
 *
 * Returns: 1 if option is valid and 0 if not
 */
int is_valid_option(const char* option) {
    return strcmp(option, "--from") == 0 ||
	    strcmp(option, "--target") == 0 ||
            strcmp(option, "--limit") == 0 ||
            strcmp(option, "--length") == 0 ||
            strcmp(option, "--dict") == 0;
}

/* is_command_line_valid()
 * -----------------------
 * Goes through command line to see if its valid 
 *
 * argc: int - the number of arguments
 * argv: char** - the command line arguments
 *
 * Returns: int 1 if its valid and otherwise its invalid 
 */
int is_command_line_valid(int argc, char** argv) {
    int index = 1;
    int isValid = 1;
    // An array of OptionCounts to store the number of times an option is 
    // mentioned
    OptionCount optionCounts[] = {{"--from", 0}, {"--target", 0},
            {"--limit", 0}, {"--length", 0}, {"--dict", 0}};
    int numOptions = sizeof(optionCounts) / sizeof(optionCounts[0]);
    while (index < argc) {
        const char* current = argv[index];
        if (is_valid_option(current)) {
            for (int i = 0; i < numOptions; i++) {
		// Checking for multiple options 
                if (strcmp(optionCounts[i].option, current) == 0) {
                    optionCounts[i].count++;
                    break;
                }
            }
            if (index + 1 < argc && !is_valid_option(argv[index + 1])) {
                index++;
            } else {
                invalid_command(argc, argv);
            }
        } else {
            invalid_command(argc, argv);
        }
        index++;
    }
    for (int i = 0; i < numOptions; i++) {
        if (optionCounts[i].count > 1) {
            invalid_command(argc, argv);
        }
    }
    return isValid;
}

/* process_arguments()
 * -------------------
 * This function processes all the arguments and sets them to their specified
 * value or the default value
 *
 * argc: int - the number of command line arguments
 * argv: char** - the command line arguments
 * initialWord: char** - the starting word
 * toWord: char** - the target word
 * num: int* - the pointer to the desired length
 * maxSteps: int* - the pointer to the maximum number of steps
 *
 * Returns: void
 */
void process_arguments(int argc, char** argv, const char** initialWord,
        const char** toWord, int* num, int* maxSteps,
        const char** dictFilename) {

    process_words(argc, argv, initialWord, toWord);
    process_numbers(argc, argv, "--length", num);
    process_numbers(argc, argv, "--limit", maxSteps);
    process_dictionary(argc, argv, dictFilename);

    // Length validity checking
    check_word_length(*initialWord, *num);
    check_word_length(*toWord, *num);
    if (*initialWord && *toWord) {
        check_word_length(*initialWord, strlen(*toWord));
    }
    set_word_length(num, *initialWord, *toWord);

    // Generates words if not provided
    if (!*initialWord) {
        // Have to copy it to another memory because get_uqwordladder_word()
        // stores its results in a single piece of memory
        const char* wordOne = get_uqwordladder_word(*num);
        char* tempOne = (char*)malloc(sizeof(char) * (*num));
        strcpy(tempOne, wordOne);
        *initialWord = strdup(tempOne);
        free(tempOne);
    }
    if (!*toWord) {
        const char* wordTwo = get_uqwordladder_word(*num);
        char* tempTwo = (char*)malloc(sizeof(char) * (*num));
        strcpy(tempTwo, wordTwo);
        *toWord = strdup(tempTwo);
        free(tempTwo);
    }

    // Check is the words contain non-letters first
    check_for_non_letters(*initialWord, *toWord);

    // Checks if initialWord and toWord are the same no matter the case
    compare_words(*initialWord, *toWord);

    // Check the step limit must be the word length to 55
    check_step_bounds(*maxSteps, *num);

    is_command_line_valid(argc, argv);
}

/* read_line()
 * -----------
 * Function that reads a line from the file
 *
 * file: FILE* - a file that is already opened
 *
 * Returns: char* - The line read or NULL if line is EOF
 *
 * REF: The following block of code is inspired by week 3.2 'Basic file
 *      handling' written by me
 */
char* read_line(FILE* file) {
    int numElem = 0;
    char* line = (char*)malloc(sizeof(char));
    if (feof(file)) {
        return NULL;
    }
    while (1) {
        int c = fgetc(file);
        if (c == EOF || c == '\n') {
            if (c == EOF && numElem == 0) {
                free(line);
                return NULL;
            }
            break;
        } else {
            // Plus 2 for the first and next line because numElem
            // is initialised to 0
            line = (char*)realloc(line, (numElem + 2) * sizeof(char));
            line[numElem] = c;
            numElem++;
        }
    }
    line[numElem] = '\0';
    return line;
}

/* init_dictionary()
 * -----------------
 * This function initialises the dictionary. It stores all the lines that
 * match the target length
 *
 * file: FILE* - the file to be read
 * targetLength: int - the target length
 *
 * Returns: a dictionary struct that contains the number of lines and a
 *      char** that contains the dictionary of words
 *
 * REF: The following code is inspired from what I have written in Edlesson
 *      week 3.2 Reading from the filesystem and week 2.1 Defining structs
 */
Dictionary init_dictionary(FILE* file, int targetLength) {
    Dictionary dict;
    dict.numLines = 0;
    dict.dictionary = (char**)malloc(sizeof(char*));

    while (1) {
        char* line = read_line(file);
        if (line) {
            // If the line length is the same as the target length
            // then store it to the dictionary
            if (strlen(line) == targetLength) {
                dict.dictionary = (char**)realloc(dict.dictionary,
                        (dict.numLines + 1) * sizeof(char*));
                dict.dictionary[dict.numLines] = strdup(line);
                dict.numLines++;
            }
            free(line);
        } else {
            free(line);
            break;
        }
    }
    return dict;
}

// Checks for the dictionaries file name
/* open_dictionary_file()
 * ----------------------
 * Opens dictionary file and if not opened then throws error message and exits
 *
 * dictFilename: char* - the path to the file to be opened
 *
 * Returns: the dictioanary file
 */
FILE* open_dictionary_file(const char* dictFilename) {
    FILE* dictFile = fopen(dictFilename, "r");
    if (dictFile == NULL) {
        fprintf(stderr, "uqwordladder: File named \"%s\" "
                "cannot be opened\n", dictFilename);
        exit(17);
    }
    return dictFile;
}

/* cleanup_dictionary()
 * --------------------
 * This cleans up the dictionaries memory
 *
 * dict: Dictionary* - the dictionary to be cleared
 */
void cleanup_dictionary(Dictionary* dict) {
    for (int i = 0; i < dict->numLines; i++) {
        free(dict->dictionary[i]);
    }
    free(dict->dictionary);
}

/* letter_difference()
 * -------------------
 * Checks if the two words provided are exactly one letter different
 *
 * firstGuess: char* - the first word to be compared
 * secondGuess: char* - the second word to be compared
 *
 * Returns: the difference count
 */
int letter_difference(char* firstGuess, char* secondGuess) {
    int diffCount = 0;
    for (int i = 0; i < strlen(firstGuess); i++) {
        if (tolower(firstGuess[i]) != tolower(secondGuess[i])) {
            diffCount++;
        }
    }
    return diffCount;
}

/* is_word_guessed()
 * -----------------
 * This checks if the word is already guessed
 *
 * guessedWords: char** - the dictionary of all guessed words and the initial
 *      word
 * numGuesses: int - the number of guesses made. Used to travel thorugh
 *      dictionary of guessedWords
 * word: char* - the word to be checked
 *
 * Returns: an int 1 if already guessed and 0 otherwise
 */
int is_word_guessed(char** guessedWords, int numGuesses, const char* word) {
    for (int i = 0; i < numGuesses; i++) {
        if (strcasecmp(guessedWords[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/* is_word_in_dict()
 * -----------------
 * Check if word is in dictionary
 *
 * dict: Dictionary* - the dictionary of words
 * word: char* - the word to be checked if its in the dictionary
 *
 * Returns: int - 1 if the word is in dictionary and 0 if not
 */
int is_word_in_dict(Dictionary* dict, const char* word) {
    for (int i = 0; i < dict->numLines; i++) {
        if (strcasecmp(dict->dictionary[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/* suggest_words()
 * ---------------
 * Prints out suggested words if the user enters '?'
 *
 * dict: Dictionary* - the dictionary
 * initialWord: char* - initial word to check its not in the dictionary
 * guesses: char** - a collection of all the previous guesses and initial word
 * stepNum: int - the step number
 * userInput: char* - the user input to check that the list only contains
 *      one letter different from this
 * toWord: char* - the target word 
 *
 * Returns: void
 */
void suggest_words(const Dictionary* dict, const char* initialWord,
        char** guesses, int stepNum, char* userInput, const char* toWord) {
    int suggestionsFound = 0;
    int targetListed = 0; 
    // Checks if target is listed
    for (int i = 0; i < dict->numLines; i++) {
	if (strcasecmp(dict->dictionary[i], toWord) == 0) {
	    targetListed = 1;
	    break;
	}
    }
    for (int i = 0; i < dict->numLines; i++) {
        if (letter_difference(dict->dictionary[i],
                guesses[stepNum - 1]) == 1 &&
                is_all_letters(dict->dictionary[i]) &&
                strcasecmp(dict->dictionary[i], initialWord) != 0 &&
                is_word_guessed(guesses, stepNum, dict->dictionary[i]) != 1 &&
		strcasecmp(dict->dictionary[i], toWord) != 0) {
            // Prints out word in all uppercase
            suggestionsFound++;
            if (suggestionsFound == 1) {
                printf("Suggestions:-----------\n");
            }
	    // Prints out target word first if letter difference to guess
	    if (targetListed == 1 && letter_difference((char*)toWord,
		    guesses[stepNum - 1]) == 1) {
		printf(" ");
		for (int k = 0; k < strlen(toWord); k++) {
		    printf("%c", toupper(toWord[k]));
		}
		printf("\n");
		targetListed++;
	    }
	    printf(" ");
            for (int j = 0; dict->dictionary[i][j]; j++) {
                printf("%c", toupper(dict->dictionary[i][j]));
            }
            printf("\n");
        }
    }
    if (suggestionsFound) {
        printf("-----End of Suggestions\n");
    } else {
        printf("No suggestions found.\n");
    }
}

/* is_input_null()
 * ---------------
 * Checks if input is null
 *
 * userInput: char* - the input to be checked
 */
void is_input_null(char* userInput) {
    if (userInput == NULL) {
        printf("Game over.\n");
        exit(12);
    }
}

/* handle_invalid_input()
 * ----------------------
 * Handles invalid user input during game. Checks the validity of input based
 * on word length, composition, dictionary inclusion, letter difference,
 * and previous word repetition. If any are met, it displays an error message.
 *
 * userInput: char* - the input to be validated
 * num: int - the length of word
 * dict: Dictionary* - the pointer to the dictionary
 * guesses: char** - the previous guesses
 * stepNum: int - the current step in game
 */
void handle_invalid_input(const char* userInput, int num, Dictionary* dict,
        char** guesses, int stepNum) {
    if (strlen(userInput) != num) {
        printf("Word must be %i characters long - try again.\n", num);
    } else if (!is_all_letters(userInput)) {
        printf("Word should contain only letters - try again.\n");
    } else if (is_word_in_dict(dict, userInput) == 0) {
        printf("Word not found in dictionary - try again.\n");
    } else if (letter_difference((char*)userInput, guesses[stepNum - 1])
            != 1) {
        printf("Word should be only one letter different - try again.\n");
    } else if (is_word_guessed(guesses, stepNum, userInput) == 1) {
        printf("You can't return to a previous word - try again.\n");
    }
}

/* play_word_ladder_game()
 * -----------------------
 * Plays the word ladder game.
 * If the step number is bigger than the limit, then the game is over.
 * If the user has achieved the target word, then they have won.
 *
 * initialWord: char* - the starting word
 * toWord: char* - the target word
 * num: int - the specified length of word
 * maxSteps: the limit of steps to take until target word
 * dictFilename: char* - the dictionary file name to be opened and read
 *
 * Return: void
 */
void play_word_ladder_game(const char* initialWord, const char* toWord,
        int num, int maxSteps, const char* dictFilename) {
    FILE* dictFile = open_dictionary_file(dictFilename);
    Dictionary dict = init_dictionary(dictFile, num);
    fclose(dictFile);
    int stepNum = 1;
    print_welcome_message(initialWord, toWord, maxSteps);
    char** guesses = (char**)malloc(sizeof(char*));
    guesses[0] = strdup(initialWord);

    while (1) {
        printf("Enter word %i (or ? for help):\n", stepNum);
        char* userInput = read_line(stdin);
        char* guess = (char*)malloc((num + 1) * sizeof(char));

        is_input_null(userInput);
        if (!strcmp("?", userInput)) {
            suggest_words(&dict, initialWord, guesses, stepNum, 
		    userInput, toWord);
            continue;
        }
        if (strlen(userInput) != num || !is_all_letters(userInput) ||
                is_word_in_dict(&dict, userInput) == 0 ||
                letter_difference(userInput, guesses[stepNum - 1]) != 1 ||
                is_word_guessed(guesses, stepNum, userInput)) {
            handle_invalid_input(userInput, num, &dict, guesses, stepNum);
            continue;
        }
        stepNum++;
        guesses = (char**)realloc(guesses, stepNum * sizeof(char*));
        strcpy(guess, userInput);
        guesses[stepNum - 1] = guess;
        // Handles game over and if user has won
        if (stepNum > maxSteps) {
            printf("Game over - no more attempts remaining.\n");
            exit(18);
        }
        if (strcasecmp(guess, toWord) == 0) {
            printf("Well done - you solved the ladder in %i steps.\n",
                    stepNum - 1);
            exit(0);
        }
    }
    // Frees memory and dictionary
    for (int i = 0; i < stepNum - 1; i++) {
        free(guesses[i]);
    }
    free(guesses);
    cleanup_dictionary(&dict);
}

int main(int argc, char** argv) {
    const char* initialWord = NULL;
    const char* toWord = NULL;
    int num = 0;
    int maxSteps = DEFAULT_STEPS;
    const char* dictFilename = DEFAULT_DICTIONARY;
    
    is_command_line_valid(argc, argv);
    // Going through command line arguments
    process_arguments(argc, argv, &initialWord, &toWord, &num, &maxSteps,
            &dictFilename);

    // Calls the game logic function
    play_word_ladder_game(initialWord, toWord, num, maxSteps, dictFilename);
    return 0;
}
