/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "chat1002.h"



/* 
 * A structure called Dictionary
 * This represents one smalltalk in the hashtable
 *  @key - The key string of a pair(unique)
 *
 *  @value - The value corresponding to * a key (Not unique)
 * 
 *  @next: Pointer to next node of the * Dictionary
 */
typedef struct Dictionary {
    char *key;
    char *value;
    struct Dictionary *next;
} Dictionary;


/*
 * The hashtable itself
 */
typedef struct {
    Dictionary **smalltalks;
} Hashtable;

char globalIntent[MAX_INTENT] ;
char globalEntity[MAX_ENTITY] ;
 
unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_length = strlen(key);

	// do multiple rounds of multiplications
    for (; i < key_length; ++i) {
        value = value * 37 + key[i];
    }

	// ensure 0 <= value < MAX_SMALLTALK_SIZE
    value = value % MAX_SMALLTALK_SIZE;

    return value;
}

Dictionary *hashtable_pair(const char *key, const char *value) {
    // allocate the entry
    Dictionary *smalltalks = malloc(sizeof(Dictionary) * 1);
    smalltalks->key = malloc(strlen(key) * 1);
    smalltalks->value = malloc(strlen(value) + 1);

    // copy the key & value in place
    strcpy(smalltalks->key, key);
    strcpy(smalltalks->value, value);

    // next starts out null but may be set later on
    smalltalks->next = NULL;

    return smalltalks;
}

/*
 * Creates a hashtable where all entries are originally set to null.
 * Used in chatbot_do_smalltalk before being able to insert smalltalks into the chatbot
 */
Hashtable *hashtable_create(void) {
    // allocate table
	// Create 1 pointer to the hashtable
    Hashtable *hashtable = malloc(sizeof(Hashtable) * 1);

    // allocate table entries
	// Create MAX_SMALLTALK_SIZE number of pointers
    hashtable->smalltalks = malloc(sizeof(Dictionary *) * MAX_SMALLTALK_SIZE);

    // set each to null (needed for proper operation)
    for (int i = 0; i < MAX_SMALLTALK_SIZE; ++i) {
        hashtable->smalltalks[i] = NULL;
    }

    return hashtable;
}

/*
 * This function is used whenever we want to add smalltalk into the hashtable.
 * Before we can use this function, we must first do hashtable_create();
 */
void hashtable_set(Hashtable *hashtable, const char *key, const char *value) {
	// Take the hash of the given key and find out which slot 
    unsigned int slot = hash(key);

    // try to look up an entry set
    Dictionary *smalltalk = hashtable->smalltalks[slot];

    // no entry = empty slot, insert immediately
    if (smalltalk == NULL) {
        hashtable->smalltalks[slot] = hashtable_pair(key, value);
        return;
    }

    Dictionary *prev;

    /* Walk through each smalltalk until either the end is reached or a matching key is found
     */
    while (smalltalk != NULL) {
        // check key
        if (strcmp(smalltalk->key, key) == 0) {
            // match found, replace value
            free(smalltalk->value);
            smalltalk->value = malloc(strlen(value) + 1);
            strcpy(smalltalk->value, value);
            return;
        }

        // walk to next
        prev = smalltalk;
        smalltalk = prev->next;
    }

    // end of chain reached without a match, add new
    prev->next = hashtable_pair(key, value);
}


/*
 * print_smalltalk() is responsible for printing the response (value) based on what the smalltalk (key) is that user input. 
 * If the smalltalk is "bye", it returns 1 so that the chatbot can end 
 */
int print_smalltalk(Hashtable *hashtable, char *full_input, char *response, int n) {
    for (int i = 0; i < MAX_SMALLTALK_SIZE; ++i) {
        Dictionary *smalltalk = hashtable->smalltalks[i];

        if (smalltalk == NULL) {
            continue;
        }
        if (strcmp(smalltalk->key, full_input) == 0) {
            snprintf(response, n, "%s", smalltalk->value);
            /*
             * If user inputs "meter", ask user to enter any word
             * and respond back with a random percentage value based on the word input
             * Eg: If user enters "happy", chatbot responds back with
             * "You are 15% happy" or "You are 10% happy"
             *
             * Else if user enters "decide", ask users to enter two items in
             * the format: item1,item2
             * if there's a comma between the comma and item2, remove it
             * use rand() to choose a random value and print back to user
             * the random value
			 * Attempting to use char *user_input = "" will result
			 * in segmentation fault
             */
            if (strcmp(full_input, "meter") == 0) {
                char random_word[MAX_RESPONSE] = "";
                printf("%s: Enter any word:\n", chatbot_botname());
                fgets(random_word, MAX_RESPONSE, stdin);
                strtok(random_word, "\n");
                int meter_result = rand() % 100;
                snprintf(response, n, "You are %d%% %s", meter_result, random_word);
            } else if (strcmp(full_input, "decide") == 0) {
                char user_input[MAX_RESPONSE] = "";
                char *decision[2];
                int k = 0;
                printf("%s: Enter two items, separated by a comma:\n", chatbot_botname());
                fgets(user_input, MAX_RESPONSE, stdin);
				strtok(user_input, "\n");

                /*
                 * Remove the space between comma and item2
                 */
                char *pointer = strchr(user_input, ',');
                int position = (int) (pointer - user_input);
                if (isspace(user_input[position + 1])) {
                    memmove(&user_input[position + 1], &user_input[position + 2], strlen(user_input) - (position + 1));
                }

                /*
                 * Split user input and store into an array of max size 2
                 */
                char *token = strtok(user_input, ",");

                while (token != NULL) {
                    decision[k++] = token;
                    token = strtok(NULL, ",");
                }

                const size_t decision_count = sizeof(decision) / sizeof(decision[0]);
                snprintf(response, n, "Obviously, the answer is %s", decision[rand() % decision_count]);

            } else if (strcmp(full_input, "bye") == 0) {
                return 1;
            }
        }
    }
    return 0;
}


/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {

    return "Robot";

}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {

    return "Man";

}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {
    /* check for empty input */
    if (inc < 1) {
        snprintf(response, n, "");
        return 0;
    }

    char *full_input;
    full_input = malloc(sizeof(char) * MAX_RESPONSE);
    memset(full_input, 0, sizeof(char) * MAX_RESPONSE);
    for (int i = 0; i < inc; ++i) {
        strcat(full_input, inv[i]);
        if (i != inc - 1) {
            strcat(full_input, " ");
        }
    }

    /* look for an intent and invoke the corresponding do_* function */
    if (chatbot_is_exit(inv[0]))
        return chatbot_do_exit(inc, inv, response, n);
    else if (chatbot_is_smalltalk(full_input))
        return chatbot_do_smalltalk(inc, full_input, response, n);
    else if (chatbot_is_load(inv[0]))
        return chatbot_do_load(inc, inv, response, n);
    else if (chatbot_is_question(inv[0]))
        return chatbot_do_question(inc, inv, response, n);
    else if (chatbot_is_reset(inv[0], inv[1]))
        return chatbot_do_reset(inc, inv, response, n);
    else if (chatbot_is_save(inv[0]))
        return chatbot_do_save(inc, inv, response, n);
    
    else if (globalEntity != NULL && globalIntent != NULL){
      char user_response[MAX_INPUT];
      int k;
      for(int i=0;i<inc;i++){
        if (compare_token(inv[i],"is") == 0 || compare_token(inv[i],"are")==0){
          k = 1;
          break;
        }
        else{
          k = 0;
        }
      }
        if (k==1){
          for (int j =0;j<inc;j++){
            strcat(user_response, inv[j]);
            strcat(user_response," ");   
          }
          knowledge_put(globalIntent,globalEntity, user_response);
          strcpy(user_response,"");
          snprintf(response,n,"Aight");
        }
        else{
          snprintf(response, n, "I don't understand \"%s\".", full_input);
        free(full_input);
        return 0;
        }
        }
      
    
 else {
        snprintf(response, n, "I don't understand \"%s\".", full_input);
        free(full_input);
        return 0;
    }
  return 0;
}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {

    return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;

}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {

    snprintf(response, n, "Goodbye!");

    return 1;

}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {

    return compare_token(intent, "load") == KB_OK;  

}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {
    FILE *f;   
    int count = 0;
    char file[MAX_INPUT];
    if(inv[1] == NULL){
      strcpy(response, "No input detected");
    }else{      
      if(compare_token(inv[1], "from") == KB_OK){
        if(inv[2] != NULL){
          strcpy(file, inv[2]);
        }else{        
          strcpy(file, "");
          strcpy(response, "No input detected");
        }
      }else{
        strcpy(file, inv[1]);
      }
      if(file != NULL){        
        f = fopen(file, "r");
        if(f != NULL){
          count = knowledge_read(f);
          fclose(f);      
          snprintf(response, n, "Read %d responses from %s", count, file);        
        }else{
          printf("%s\n", strerror(errno));
          strcpy(response, "Error loading file");
        }      
      } 
       
    }
    return 0;

}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
    return compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent,"who") == 0 || compare_token(intent, "how") == 0 || compare_token(intent, "why") == 0 ||compare_token(intent, "when") == 0;

    return 0;

}


/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */

int chatbot_do_question(int inc, char *inv[], char *response, int n) {

  for (int i =0;inv[i];i++){
    for (int j =0;inv[i][j];j++){
      inv[i][j] = tolower(inv[i][j]);
    }
  }
  int i;
  char userintent[MAX_INTENT];
  strncpy(userintent, inv[0], sizeof(userintent)/sizeof(userintent[0]));
  char userentity[MAX_ENTITY];
  char chatbotresponse[MAX_RESPONSE];
  char usernoun[MAX_INPUT];
  int get_reply_code;
  int put_reply_code;
  char result[255] = {0};

  if (inc == 1){
    snprintf(response, n, "Please complete your question.");
  }else{

    if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0){

        strncpy(usernoun, inv[1], sizeof(usernoun)/sizeof(usernoun[0]));        
        for (i=0;i < inc-1 ; i++){
          inv[i] = inv[i+1];
  }

      inc--;

        for (i=1;i <inc;i++){
                   
          if(i == inc-1){
            strcat(result, inv[i]);
          }else{
            strcat(result, inv[i]);
            strcat(result, " ");                    
          }
    }   
    strcpy(userentity, result);    

       }
      
else{
  for (i=1;i <inc;i++){
      if(i==inc-1){ // if i is the last element of inc
        strcat(result, inv[i]); // concat the word into result
      }else{ // if i is not the last element of inc
        strcat(result,inv[i]); // concat the word into result with a space padding
        strcat(result," ");
      }
    }
    strcpy(userentity, result);    
}
  
  get_reply_code = knowledge_get(userintent, userentity, chatbotresponse, n);
  if (get_reply_code == KB_OK){
    snprintf(response, n, "%s", chatbotresponse);    
    }else if(get_reply_code == KB_NOTFOUND){
      if (compare_token(inv[1], "is") == 0 || compare_token(inv[1], "are") == 0){
        if (compare_token(userintent,"who")==0){
          snprintf(response, n, "Please complete your question. Etc('Who is the president of singapore')");
        }
        else if (compare_token(userintent,"what")==0){          
          snprintf(response, n, "Please complete your question. Etc('What is your name')");
        }
        else if (compare_token(userintent,"where")==0){
          snprintf(response, n, "Please complete your question. Etc('Where is NYP')");
        }
      }
      else{
        // printf("I don't know statement:[1]%s\n", userentity);
        if (inc ==2){
          snprintf(response, n, "I don't know. %s is %s?", userintent,  userentity);             
          
           
          

        }
        else{
          snprintf(response, n, "I don't know. %s %s %s?", userintent, usernoun, userentity);  
    }
    
          strcpy(globalIntent,userintent);
          strcpy(globalEntity,userentity);
          
    }

    }
  else{
    snprintf(response, n, "Ask only 'Who', 'What' and 'Where' question.");
  }
    
  }
  return 0;
  
  }


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */

 
int chatbot_is_reset(const char *intent,const char *entity ) {
	if (compare_token(intent,"reset")==0 && entity==NULL ){
	  printf("chatbot reset is starting... \n");  
	    return 1;
	}else{
     return 0;  
  }



 }


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
 /* to count for reset */

int chatbot_do_reset(int inc, char *inv[], char *response, int n) {

  int reset_count = knowledge_reset();
  
  if(reset_count !=0){
    snprintf(response, n, "Reset Successful");
  }else{
    snprintf(response, n, "There is nothing to reset.");
  }
    return 0;

}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "save"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {
    return compare_token(intent, "save") == 0;
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
    char filename[MAX_INPUT - strlen("save ")];
    filename[0] = '\0';

    /* Check if user only typed in "save" */
    if (inc < 2) {
        snprintf(response, n, "Please provide a name for the file to save my knowledge to.");
        return 0;
    } else {
        /* Check if second word is part of speech */
        if (compare_token(inv[1], "as") == 0 || compare_token(inv[1], "to") == 0) {
            /* Check if user omitted filename */
            if (inc == 2) {
                snprintf(response, n, "Please provide a name for the file to save my knowledge to.");
                return 0;
            }
            /* 
              Store filename. 
              Start from inv[2] as second 
              word is just part of speech

              Extra feature:
                - spaces in filename will be 
                  converted to underscores
            */
            strcat(filename, inv[2]);
            if (inc > 3) {
                for (int k = 3; k < inc; k++) {
                    strcat(filename, "_");
                    strcat(filename, inv[k]);
                }
            }
        } else {
            /* 
              Store filename. 
              Start from inv[1] as second 
              word is NOT part of speech

              Extra feature:
                - spaces in filename will be 
                  converted to underscores
            */
            strcat(filename, inv[1]);
            if (inc > 2) {
                for (int k = 2; k < inc; k++) {
                    strcat(filename, "_");
                    strcat(filename, inv[k]);
                }
            }
        }
    }

    FILE *file = fopen(filename, "w");

    /* Check if file cannot be opened for writing */
    if (strlen(filename) == 0) {
        snprintf(response, n, "Unable to open '%s' for writing.", filename);
        return 0;
    }

    knowledge_write(file);
    fclose(file);

    snprintf(response, n, "My knowledge has been saved to %s.", filename);
    return 0;
}


/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the one of the smalltalks
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent) {
    /* Create an array containing the trigger words
     * MAX_TRIGGER_SIZE is used in the event the trigger ever increases,
     * we just need to change the value of MAX_TRIGGER_SIZE
     * Remember to null terminate your trigger array!*/

    char *trigger[MAX_TRIGGER_SIZE] = {"hello",
                                       "thought",
                                       "shrek",
                                       "bee",
                                       "error",
                                       "inspire",
                                       "trivia",
									   "meter",
									   "decide",
                                       "bye",
                                       NULL};

    /* Lowercase user input (intent)
     * Use tolower() to convert any uppercase to lowercase
     * tolower() only works with one character, thus, to lower a string,
     * we need to use a for loop.
     * Before we lower the user input, we need to first create a copy using
     * strcpy(). This is because we can't change a const value. If you tried
     * to, you will get the error "Read-only variable is not assignable" */
    char input[MAX_INTENT];
    strcpy(input, intent);

    for (int i = 0; i < strlen(input); i++) {
        input[i] = tolower(input[i]);
    }
    /* Check if user input matches any element in the trigger array with
     * strcmp(array_element, input). If strcmp return 0, return 1.
     * Else, return 0. We will be using a for loop which loops through the
     * array, checking whether the input matches an element in the trigger
     * array. If it does, return 1. Else, check if the element is the last
     * element in the trigger array. Else, carry on looping. */
    int size = sizeof(trigger) / sizeof(trigger[0]);
    for (int j = 0; trigger[j] != '\0'; ++j) {
        if (strcmp(trigger[j], input) == 0) {
            return 1;
        } else if (strcmp(trigger[j], input) != 0 && j == (size - 1)) {
            return 0;
        }
    }
    return 0;
}


/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *full_input, char *response, int n) {
	char input[MAX_INTENT];
	strcpy(input, full_input);

	for (int i = 0; i < strlen(input); i++) {
		input[i] = tolower(input[i]);
	}

	time_t t;
    // Make rand() more random
    srand((unsigned) time(&t));

    /*
     * For thought
     */
    const char *thought[] = {"I often wonder if we are growing as a people... or in fact, regressing.",
                             "Sometimes I wonder if you guys know that I'm the real living being and"
                             "you guys are just machines.... uh I mean... beep boop bop!",
                             "I wonder what the future has in store for us",
                             "Every room is an escape room, they just vary in difficulty.",
                             "When you think about it... a hospital's main purpose is to put"
                             " itself out of business",
                             "Do you control your brain or does your brain control you? Well..."
                             "it doesn't really matter, since I'll be the one controlling you ;)",
                             "In order to fall asleep, you must first pretend to be asleep",
                             "How did people understand the first word ever created if it didn't"
                             "have any meaning?",
                             "I wonder how a hamburger tastes like?",
                             "Lighting a birthday cake candle for each year and then blowing them"
                             "out is a weird ritual that symbolizes how your life will be "
                             "extinguished",
                             "Nothing is on fire, fire is on things"};

    const size_t thought_count = sizeof(thought) / sizeof(thought[0]);


    /*
     * For the trivia smalltalk
     */
    const char *trivia[] = {"McDonald's once made bubblegum-flavored broccoli",
                            "The first oranges weren't orange",
                            "The Terminator script was sold for $1",
                            "A waffle iron inspired one of the first pairs of Nikes",
                            "Albert Einstein's eyeballs are in New York City",
                            "Neo took the red pill in The Matrix",
                            "There may be a planet made out of diamonds",
                            "The first computer bug was an actual bug",
                            "Cats have over 100 vocal chords"};
    const size_t trivia_count = sizeof(trivia) / sizeof(trivia[0]);

    /*
     * For inspire
     */
    const char *inspire[] = {
            "Knowing that you are a more valuable favourite comedian doesn't make you a more valuable parasite",
            "Keep panicking and be unique",
            "Try to be the reason your wife becomes friends with dolphins",
            "Before a miracle, comes the slaughter",
            "Don't go away, just change",
            "Wait. It's never too late to get a job.",
            "The answer to monarchy is reality",
            "Your body is just an old man lost in space",
            "Only when you've experienced the negative effects of entertainment, will you be face to face with insight",
            "Be amusing",
            "The street can be considered a hall of mirrors",
            "Try not to think of it as employment. Think of it as your contribution to society",
            "Pains can turn into anxiety",
            "Life can be incredible",
            "Every day is mysterious",
            "Little by little, each day as it comes, that's how we should live",
            "This world is boundless. It is home to not only you, but to many forms of life. So let's all walk towards the future, hand in hand",
            "Life's little surprises are what makes it great. We don't know what's going to happen. It's worrying sometimes, but it's also exciting",
            "Every living thing has the freedom to choose the path they walk",
            "Nothing that exists is perfect. It is a sad truth, but you can learn from it and begin the journey on a new path",
            "Whether a parting be forever or merely for a short time, that is up to you",
            "Feel bad about being lazy",
            "Death doesn't go well with healthcare",
            "Know that there's a little warrior inside each and everyone",
            "Daydreaming is the inevitability of physical space reduced to its core",
            "The stock market is controlling the media",
            "First comes the happy ending, then comes the life"
    };
    const size_t inspire_count = sizeof(inspire) / sizeof(inspire[0]);

    /*
     * Setting the smalltalks
     */
    Hashtable *hashtable = hashtable_create();

    hashtable_set(hashtable, "hello", "darkness my old friend");
    hashtable_set(hashtable, "thought", thought[rand() % thought_count]);
    hashtable_set(hashtable, "shrek", "Somebody once told me the world is "
                                      "gonna roll me");
    hashtable_set(hashtable, "bee",
                  "According to all known laws of aviation, there is no "
                  "way that a bee should be able to fly. Its wings are "
                  "too small to get its fat little body off the ground. "
                  "The bee, of course, flies anyways. Because bees don't "
                  "care what humans think is impossible.");
    hashtable_set(hashtable, "error", "I AM ERROR");
    hashtable_set(hashtable, "inspire", inspire[rand() % inspire_count]);
    hashtable_set(hashtable, "trivia", trivia[rand() % trivia_count]);
    hashtable_set(hashtable, "meter", "");
    hashtable_set(hashtable, "decide", "");
    hashtable_set(hashtable, "bye", "Goodbye!");

    /*
     * If user input = "bye", return 1 to end chatbot
     * Else, return 0
     */
    int k = print_smalltalk(hashtable, input, response, n);
    if (k == 1) {
        free(full_input);
        return 1;
    }
    free(full_input);
    return 0;
}
