/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */





/* The original code is public domain -- Will Hartung 4/9/09 */
/* Modifications, public domain as well, by Antti Haapala, 11/10/17
   - Switched to getc on 5/23/19 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

// if typedef doesn't exist (msvc, blah)
typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}





#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1002.h"

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {

	if (compare_token(intent, "who")==0){
    if (headWho != NULL){
      pointerWho = headWho;
      do{
        if(compare_token(pointerWho -> entity, entity) == 0){
          strncpy(response, pointerWho->response , n);
          return KB_OK;
        }
        pointerWho = pointerWho->next;
      }while (pointerWho);
      return KB_NOTFOUND;
    }
    else{
      return KB_NOTFOUND;

    }    
  }
	return KB_INVALID;


}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {

    if (compare_token(intent, "what") == 0) {
        NODE *next, *prev;
        // NODE temp = { 
        //   "what", 
        //   (char *)malloc(sizeof(char) * strlen(entity)), 
        //   (char *)malloc(sizeof(char) * strlen(response)), 
        //   NULL 
        // };

        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        if (!headWhat) {
            headWhat = temp;
        } else {
            prev = NULL;
            next = headWhat;

            while (next) {
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else if (compare_token(intent, "where") == 0) {
        NODE *next, *prev;
        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        if (!headWhere) {
            headWhere = temp;
        } else {
            prev = NULL;
            next = headWhere;

            while (next) {
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else if (compare_token(intent, "who") == 0) {
        NODE *next, *prev;

        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        if (!headWho) {
            headWho = temp;
        } else {
            prev = NULL;
            next = headWho;

            while (next) {
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else {
        return KB_INVALID;
    }
    return KB_OK;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
  int count = 0;
  char *result;
  size_t MAX_SIZE = MAX_INTENT;
  char *line = malloc(MAX_SIZE * sizeof(char));  
  char intent[MAX_INTENT];
  char entity[MAX_ENTITY];
  char response[MAX_RESPONSE];

  while(getline(&line, &MAX_SIZE, f) != KB_NOTFOUND){
    if(strstr(line, "what")){
      strcpy(intent, "WHAT");
    }
    else if(strstr(line, "where")){
      strcpy(intent, "WHERE");
    }
    else if(strstr(line, "who")){
      strcpy(intent, "WHO");
    }
    if(strchr(line, '=')){
      result = strtok(line,"=");
      strcpy(entity, result);
      result = strtok(NULL, "=");      
      strcpy(response, result);
      knowledge_put(intent, entity, response);
      // printf("%s\n", intent);
      // printf("%s\n", entity);
      // printf("%s\n", response);
      count++;
    }
  }
  
  free(line);
  return count;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {

	/* to be implemented */

}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {
    NODE *ptr_what = headWhat,
        *ptr_where = headWhere,
        *ptr_who = headWho;

    fprintf(f, "[what]\n");
    while (ptr_what != NULL) {
        fprintf(f, "%s=%s\n", ptr_what->entity, ptr_what->response);
        ptr_what = ptr_what->next;
    }
    fprintf(f, "\n");

    fprintf(f, "[where]\n");
    while (ptr_where != NULL) {
        fprintf(f, "%s=%s\n", ptr_where->entity, ptr_where->response);
        ptr_where = ptr_where->next;
    }
    fprintf(f, "\n");

    fprintf(f, "[who]\n");
    while (ptr_who != NULL) {
        fprintf(f, "%s=%s\n", ptr_who->entity, ptr_who->response);
        ptr_who = ptr_who->next;
    }
    fprintf(f, "\n");
}
