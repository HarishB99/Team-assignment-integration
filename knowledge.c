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
  if (compare_token(intent,"what")==0){
    if (headWhat != NULL){
      pointerWhat = headWhat;
      do{
        if (compare_token(pointerWhat -> entity, entity)==0){
          strncpy(response, pointerWhat->response, n);
          return KB_OK;
        }
        pointerWhat = pointerWhat->next;
      }while (pointerWhat);
      return KB_NOTFOUND;
    }
    else{
      return KB_NOTFOUND;
    }
  }

  if (compare_token(intent,"where")==0){
    if(headWhere != NULL){
      pointerWhere = headWhere;
      do{
        if (compare_token(pointerWhere -> entity, entity) == 0){
          strncpy(response, pointerWhere->response, n);
          return KB_OK;
        }
        pointerWhere = pointerWhere-> next;
      }while (pointerWhere);
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

        /* Allocate memory to store temp node */
        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        /* Check for memory allocation failure */
        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        /* Store data in node */
        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        /* 
          Add new knowledge
        */
        if (!headWhat) {
            headWhat = temp;
        } else {
            prev = NULL;
            next = headWhat;

            while (next) {
                /*
                  If knowledge already exists, 
                  update knowledge
                */
                if (compare_token(temp->entity, next->entity) == 0) {
                    strcpy(next->response, temp->response);
                    return KB_OK;
                }
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else if (compare_token(intent, "where") == 0) {
        NODE *next, *prev;

        /* Allocate memory to store temp node */
        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        /* Check for memory allocation failure */
        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        /* Store data in node */
        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        /* 
          Add new knowledge (temp node)
        */
        if (!headWhere) {
            headWhere = temp;
        } else {
            prev = NULL;
            next = headWhere;

            while (next) {
                /*
                  If knowledge already exists, 
                  update knowledge
                */
                if (compare_token(temp->entity, next->entity) == 0) {
                    strcpy(next->response, temp->response);
                    return KB_OK;
                }
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else if (compare_token(intent, "who") == 0) {
        NODE *next, *prev;

        /* Allocate memory to store temp node */
        NODE *temp = (NODE *)malloc(sizeof(NODE));

        temp->intent = (char *)malloc(sizeof(char) * strlen(intent));
        temp->entity = (char *)malloc(sizeof(char) * strlen(entity));
        temp->response = (char *)malloc(sizeof(char) * strlen(response));

        /* Check for memory allocation failure */
        if (temp == NULL || temp->intent == NULL || temp->entity == NULL || temp->response == NULL) {
            return KB_NOMEM;
        }

        /* Store data in node */
        strcpy(temp->intent, intent);
        strcpy(temp->entity, entity);
        strcpy(temp->response, response);
        temp->next = NULL;

        /* 
          Add new knowledge (temp node)
        */
        if (!headWho) {
            headWho = temp;
        } else {
            prev = NULL;
            next = headWho;

            while (next) {
                /*
                  If knowledge already exists, 
                  update knowledge
                */
                if (compare_token(temp->entity, next->entity) == 0) {
                    strcpy(next->response, temp->response);
                    return KB_OK;
                }
                prev = next;
                next = next->next;
            }

            prev->next = temp;
        }
    } else {
        /* Invalid intent */      
        return KB_INVALID;
    }
    /* 
      If code manages to reach here, 
      everything went smoothly
    */
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
  size_t MAX_SIZE = MAX_RESPONSE;
  char *line = malloc(MAX_SIZE * sizeof(char));
  char intent[MAX_INTENT];
  char entity[MAX_ENTITY];
  char response[MAX_RESPONSE];
  

  while(getline(&line, &MAX_SIZE, f) != KB_NOTFOUND){    //keep reading next line until -1
    if(strstr(line, "where")){ // checks the string if contains word where
      strcpy(intent, "WHERE");
    }    
    else if(strstr(line, "what")){ //checking
      strcpy(intent, "WHAT");
    }
    else if(strstr(line, "who")){ //checking
      strcpy(intent, "WHO");
    }
    if(strchr(line, '=')){      
      result = strtok(line,"=");      // splits string by delimeter =
      strcpy(entity, result);   // copies content of left portion of string into entity
      result = strtok(NULL, "=");      
      strcpy(response, result); //copies content of right portion of string into entity
      knowledge_put(intent, entity, response);    // store current values into knowledge_put
      count++;
    }
  }
  
  free(line);
  return count;
  
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
int knowledge_reset() {
  int return_count = 0;
    if (headWhat !=NULL) {
      
      NODE *pointer = headWhat;

      
        while (headWhat != NULL){

          pointer = pointer -> next;

          free(headWhat);

          
          headWhat = pointer;
        }
        return_count++;
    }
        if (headWho !=NULL) {
      
      NODE *pointer = headWho;

      
        while (headWho != NULL){
          pointer = pointer -> next;

          free(headWho);
          
          headWho = pointer;
        }
        return_count++;
    }
            if (headWhere !=NULL) {
      
      NODE *pointer = headWhere;

      
        while (headWhere != NULL){
          pointer = pointer -> next;

          free(headWhere);
          headWhere = pointer;
        }
        return_count++;
    }
    return return_count;

}

int knowledge_delete(NODE *del) {
  NODE *next, *prev, *current;
  if (strcmp(del->intent,"what") == 0){
    current = headWhat;
            while (headWhat != NULL){
              printf("removing %s from %s where %s\n",headWhat->entity, headWhat->intent, headWhat->response);
              if(strcmp(del->entity,headWhat->entity) ==0){
                next = headWhat->next;
                prev->next = next;
                headWhat = current;
                free(headWhat);

                
                break;
                
              }else{
                prev = headWhat;
                headWhat = headWhat -> next;
              }
          

        }

  }
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
