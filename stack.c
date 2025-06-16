#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./input/input.h"  
#include "./types/types.h"   
#include "./vector/vector.h" 

// --- Stack Structure Definition ---
typedef struct
{
  Vec *elements; // The Vec will now manage the underlying array
} Stack;

// --- Function Prototypes ---

// Stack Operations
Stack *stack_new(size_t initial_capacity);
void stack_destroy(Stack *s);
int stack_push(Stack *s, int value);      // Returns 1 on success, 0 on failure
int stack_pop(Stack *s, int *out_value);  // Returns 1 on success, 0 on failure
int stack_peek(Stack *s, int *out_value); // Returns 1 on success, 0 on failure
int stack_is_empty(Stack *s);
size_t stack_size(Stack *s);
void stack_display(Stack *s);

// Input Helper Functions (using your int_read_line)
int get_menu_choice_input(const char *prompt_text);
int get_integer_value_input(const char *prompt_text); // Returns specific sentinel on 'q' or error

// --- Main Function (remains mostly the same) ---
int main()
{
  Stack *myStack = stack_new(5); // Initial capacity of 5

  if (myStack == NULL)
  {
    fprintf(stderr, "Error: Failed to create stack. Exiting.\n");
    return 1;
  }

  int choice;
  int value;
  int popped_value;
  int peeked_value;

  printf("--- Array-based Stack Operations (using your Vec) ---\n");

  while (1)
  {
    printf("\nStack Menu:\n");
    printf("1. Push\n");
    printf("2. Pop\n");
    printf("3. Peek\n");
    printf("4. Display\n");
    printf("5. Exit\n");

    choice = get_menu_choice_input("Enter your choice: ");

    if (choice == 0)
    {
      printf("Exiting program due to user input or error.\n");
      break;
    }

    switch (choice)
    {
    case 1: // Push
      value = get_integer_value_input("Enter value to push: ");
      if (value == -999999)
      { // Sentinel for cancel/error
        fprintf(stderr, "Push operation cancelled or invalid input. Returning to menu.\n");
        break;
      }
      if (stack_push(myStack, value))
      {
        printf("%d pushed onto the stack.\n", value);
      }
      else
      {
        fprintf(stderr, "Error: Failed to push %d onto the stack.\n", value);
      }
      break;
    case 2: // Pop
      if (stack_pop(myStack, &popped_value))
      {
        printf("%d popped from the stack.\n", popped_value);
      }
      else
      {
        printf("Stack is empty. Cannot pop.\n");
      }
      break;
    case 3: // Peek
      if (stack_peek(myStack, &peeked_value))
      {
        printf("Top element: %d\n", peeked_value);
      }
      else
      {
        printf("Stack is empty. Cannot peek.\n");
      }
      break;
    case 4: // Display
      stack_display(myStack);
      break;
    case 5: // Exit
      printf("Exiting program.\n");
      goto cleanup;
    default:
      printf("Invalid choice. Please enter a number between 1 and 5.\n");
      break;
    }
  }

cleanup:
  printf("Cleaning up stack memory...\n");
  stack_destroy(myStack);
  printf("Program terminated.\n");
  return 0;
}

// --- Input Helper Functions (No Change from previous version) ---

// Returns the valid integer choice, or 0 if 'q' is entered or an error occurs.
int get_menu_choice_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s", prompt_text);
    printf(" (enter 'q' to quit): ");
    ri = int_read_line();

    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer or 'q'.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      destroy_read_result(&ri);
      return 0; // Signal user stop
    }
    else
    { // READ_OK
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      return value;
    }
  }
}

// Returns the valid integer value, or -999999 (sentinel) if 'q' is entered or an error occurs.
#define VALUE_INPUT_SENTINEL -999999
int get_integer_value_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s", prompt_text);
    printf(" (enter 'q' to cancel): ");
    ri = int_read_line();

    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer or 'q'.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      destroy_read_result(&ri);
      return VALUE_INPUT_SENTINEL; // Signal user stop
    }
    else
    { // READ_OK
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      return value;
    }
  }
}

// --- Stack Operations Implementation (MODIFIED to use Vec) ---

// Creates a new stack by creating an underlying Vec
Stack *stack_new(size_t initial_capacity)
{
  Stack *s = malloc(sizeof(Stack));
  if (s == NULL)
  {
    fprintf(stderr, "Error: Memory allocation failed for Stack struct.\n");
    return NULL;
  }

  // Use your vec_new to create the underlying dynamic array for integers
  // The elem_size for int is sizeof(int)
  s->elements = vec_new(sizeof(int), initial_capacity);
  if (s->elements == NULL)
  {
    fprintf(stderr, "Error: Failed to create underlying Vec for stack data.\n");
    free(s); // Free the partially allocated Stack struct
    return NULL;
  }

  printf("Stack created with initial capacity: %zu\n", s->elements->capacity);
  return s;
}

// Destroys the stack by destroying the underlying Vec
void stack_destroy(Stack *s)
{
  if (s == NULL)
  {
    return;
  }
  // Use your vec_destroy to free the underlying dynamic array
  vec_destroy(s->elements); // This handles freeing s->elements->data and s->elements itself
  s->elements = NULL;       // Prevent double-free issues if stack_destroy is called again
  free(s);                  // Free the Stack struct itself
  printf("Stack destroyed.\n");
}

// Pushes an element onto the stack using vec_append
int stack_push(Stack *s, int value)
{
  if (s == NULL || s->elements == NULL)
  {
    fprintf(stderr, "Error: Cannot push to an uninitialized stack.\n");
    return 0;
  }
  // vec_append handles the dynamic resizing internally
  if (vec_append(s->elements, &value))
  {
    return 1; // Success
  }
  else
  {
    fprintf(stderr, "Error: Failed to append value to underlying vector (memory allocation?).\n");
    return 0; // Failure
  }
}

// Pops an element from the stack. Returns 1 on success, 0 on failure.
int stack_pop(Stack *s, int *out_value)
{
  if (s == NULL || s->elements == NULL)
  {
    fprintf(stderr, "Error: Cannot pop from an uninitialized stack.\n");
    return 0;
  }
  if (stack_is_empty(s))
  {
    return 0; // Stack is empty
  }

  // Get the element at the 'top' (last element in the Vec)
  // The top of the stack corresponds to vec->length - 1
  if (out_value != NULL)
  {
    *out_value = *(int *)vec_get(s->elements, s->elements->length - 1);
  }

  // Manually decrease the length to simulate pop.
  // Vec doesn't have a direct 'pop_back' or 'remove_last' function,
  // so we just decrement length. The memory for the 'popped' element
  // is technically still there but no longer considered part of the active length.
  s->elements->length--;

  return 1; // Success
}

// Peeks at the top element without removing it. Returns 1 on success, 0 on failure.
int stack_peek(Stack *s, int *out_value)
{
  if (s == NULL || s->elements == NULL)
  {
    fprintf(stderr, "Error: Cannot peek from an uninitialized stack.\n");
    return 0;
  }
  if (stack_is_empty(s))
  {
    return 0; // Stack is empty
  }

  // Get the element at the 'top' (last element in the Vec)
  if (out_value != NULL)
  {
    *out_value = *(int *)vec_get(s->elements, s->elements->length - 1);
  }
  return 1; // Success
}

// Checks if the stack is empty.
int stack_is_empty(Stack *s)
{
  if (s == NULL || s->elements == NULL)
  {
    return 1; // Consider uninitialized or NULL stack as empty
  }
  return (s->elements->length == 0); // Stack is empty if its underlying Vec has 0 length
}

// Returns the current number of elements in the stack.
size_t stack_size(Stack *s)
{
  if (s == NULL || s->elements == NULL)
  {
    return 0;
  }
  return s->elements->length;
}

// Displays the elements of the stack from top to bottom.
void stack_display(Stack *s)
{
  if (s == NULL || s->elements == NULL)
  {
    printf("Stack is NULL or uninitialized and cannot be displayed.\n");
    return;
  }
  if (stack_is_empty(s))
  {
    printf("Stack is empty. (Size: %zu, Capacity: %zu)\n", stack_size(s), s->elements->capacity);
    return;
  }

  printf("Stack (Size: %zu, Capacity: %zu): \n", stack_size(s), s->elements->capacity);
  printf("TOP -> ");
  // Iterate from the last element (top) down to the first element (base)
  printf("| %d |\n", *(int *)vec_get(s->elements, s->elements->length - 1));
  printf("       -----\n");
  for (int i = s->elements->length - 2; i >= 0; i--)
  {
    printf("       | %d |\n", *(int *)vec_get(s->elements, i));
    if (i > 0)
    {
      printf("       -----\n");
    }
  }
  printf("       BASE\n");
}