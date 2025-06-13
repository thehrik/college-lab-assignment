#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./string/string.h" // Assuming String_new, String_read_line, String_destroy, ResultString, ERR
#include "./result/result.h" // Assuming Result, ERR, OK (for parse_to_int if used directly)
#include "./parser/parser.h" // Assuming parse_to_int
#include "./vector/vector.h" // Assuming vec_new, vec_destroy, vec_append, vec_get, etc.
#include "./input/input.h"   // Assuming int_read_line, destroy_read_result, ReadResult, READ_ERR, READ_OK, READ_STOPPED
#include "./types/types.h"   // Assuming common type definitions like ReadResult (if not already in input.h)

// Function prototypes
// Replaced your custom ResultGetInt and get_int
// Now using ReadResult from input.h directly.
// Helper function for safe integer input for single values (e.g., choice, position)
int get_menu_choice_input(const char *prompt_text); // Returns 0 on 'q' or error
int get_pos_input(const char *prompt_text);         // Returns -1 on 'q' or error
int get_value_input(const char *prompt_text);       // Returns specific sentinel on 'q' or error

// Function to get vector of integers from user, now using ReadResult
void get_vec_int_from_user(Vec *vec); // Modified to be void, prints messages internally

void insert_at_position(Vec *vec, int value, int pos);
void delete_at_position(Vec *vec, int pos);
void print_array(Vec *vec);

// Main function
int main()
{
  int pos_val, value_val;
  Vec *vec = vec_new(sizeof(int), 0); // Initialize with 0 capacity

  if (vec == NULL)
  {
    fprintf(stderr, "Error: Failed to create initial vector. Exiting.\n");
    return 1;
  }

  printf("--- Initial Vector Input ---\n");
  printf("Enter integers for the initial vector. Press 'q' to stop input.\n");
  get_vec_int_from_user(vec);

  printf("\nInitial Array: ");
  print_array(vec);

  // Main menu loop
  while (1)
  {
    printf("\n--- Array Operations Menu ---\n");
    printf("1. Insert an element\n");
    printf("2. Delete an element\n");
    printf("3. Print array\n");
    printf("4. Exit\n");

    int choice = get_menu_choice_input("Enter your choice: ");

    if (choice == 0) // User entered 'q' or an error occurred during choice input
    {
      printf("Exiting program due to user input or error.\n");
      break; // Exit the loop and clean up
    }

    switch (choice)
    {
    case 1: // Insert
      value_val = get_value_input("Enter value to insert: ");
      if (value_val == -999999)
      { // Using a specific sentinel value for user cancel/error
        fprintf(stderr, "Insert value input cancelled or invalid. Returning to menu.\n");
        break;
      }
      pos_val = get_pos_input("Enter position (0-based index): ");
      if (pos_val == -1)
      { // Using -1 for user cancel/error for position
        fprintf(stderr, "Insert position input cancelled or invalid. Returning to menu.\n");
        break;
      }
      insert_at_position(vec, value_val, pos_val);
      print_array(vec); // Print after successful insert
      break;

    case 2: // Delete
      pos_val = get_pos_input("Enter position to delete (0-based index): ");
      if (pos_val == -1)
      {
        fprintf(stderr, "Delete position input cancelled or invalid. Returning to menu.\n");
        break;
      }
      delete_at_position(vec, pos_val);
      print_array(vec); // Print after successful delete
      break;

    case 3: // Print
      print_array(vec);
      break;

    case 4: // Exit
      printf("Exiting program.\n");
      goto cleanup; // Jump to cleanup
    default:
      printf("Invalid choice. Please enter a number between 1 and 4.\n");
      break;
    }
  }

cleanup:
  printf("Cleaning up vector memory...\n");
  vec_destroy(vec); // Free the vector and its data
  printf("Program terminated.\n");
  return 0;
}

// Helper function to get menu choices (1-4). Returns 0 on 'q' or error.
int get_menu_choice_input(const char *prompt_text)
{
  ReadResult ri;
  int value;
  while (1)
  {
    printf("%s", prompt_text);
    printf(" (enter 'q' to quit): "); // Inform user about 'q'
    ri = int_read_line();

    if (ri.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer (1-4) or 'q'.\n", ri.data.err_str);
      destroy_read_result(&ri); // Free error string
      continue;                 // Retry input
    }
    else if (ri.status == READ_STOPPED)
    {
      destroy_read_result(&ri);
      return 0; // Signal user stop or general program exit
    }
    else
    { // READ_OK
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri); // Free the allocated integer
      return value;             // Return valid integer
    }
  }
}

// Helper function to get position input. Returns -1 on 'q' or error.
int get_pos_input(const char *prompt_text)
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
      fprintf(stderr, "Input Error: %s. Please enter a valid non-negative integer for position or 'q'.\n", ri.data.err_str);
      destroy_read_result(&ri);
      continue;
    }
    else if (ri.status == READ_STOPPED)
    {
      destroy_read_result(&ri);
      return -1; // Signal user stop for this operation
    }
    else
    { // READ_OK
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      if (value < 0)
      { // Positions must be non-negative
        fprintf(stderr, "Error: Position cannot be negative. Please enter a non-negative integer.\n");
        continue;
      }
      return value;
    }
  }
}

// Helper function to get value input. Returns a specific sentinel value on 'q' or error.
// Using -999999 as a sentinel, assuming it's unlikely to be a valid data input.
// A more robust solution might involve returning a custom struct or using pointers to indicate success/failure.
#define VALUE_INPUT_SENTINEL -999999
int get_value_input(const char *prompt_text)
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
      return VALUE_INPUT_SENTINEL; // Signal user stop for this operation
    }
    else
    { // READ_OK
      value = *(int *)ri.data.ok;
      destroy_read_result(&ri);
      return value; // Return the valid integer (can be 0)
    }
  }
}

// Function to get initial vector elements from user
// It continues to append values until the user enters 'q' or an error occurs.
void get_vec_int_from_user(Vec *vec)
{
  ReadResult r_val;
  while (1)
  {
    printf("Enter integer (or 'q' to finish initial input): ");
    r_val = int_read_line(); // Using your int_read_line() directly

    if (r_val.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer or 'q'.\n", r_val.data.err_str);
      destroy_read_result(&r_val); // Clean up the error string
      continue;                    // Prompt for input again
    }
    else if (r_val.status == READ_STOPPED)
    {
      printf("Initial vector input finished.\n");
      destroy_read_result(&r_val); // Clean up the stopped result
      break;                       // User wants to stop adding elements
    }
    else
    {                                           // READ_OK
      int value_to_add = *(int *)r_val.data.ok; // Get the integer value
      destroy_read_result(&r_val);              // Free the allocated integer

      // Now, append to the vector
      if (!vec_append(vec, &value_to_add))
      {
        fprintf(stderr, "Error: Failed to append value %d to vector (memory allocation?).\n", value_to_add);
        // Decide how to handle this: continue, or return an error status for get_vec_int_from_user.
        // For simplicity, we'll continue, but a robust app might exit or return a status.
        break; // If vec_append fails, likely memory issue, stop adding.
      }
    }
  }
}

// Inserts a value at a specified position in the vector
void insert_at_position(Vec *vec, int value, int pos)
{
  if (vec == NULL)
  {
    fprintf(stderr, "Error: Cannot insert into a NULL vector.\n");
    return;
  }
  // Check bounds for insertion (pos can be vec->length for appending)
  if (pos < 0 || (size_t)pos > vec->length)
  {
    printf("Error: Invalid position %d for insertion. Position must be between 0 and %zu.\n", pos, vec->length);
    return;
  }

  // Ensure there is enough capacity for the new element
  // This logic duplicates vec_append's internal realloc, but we're doing
  // manual shifting here. A dedicated vec_insert API would be better.
  if (vec->length >= vec->capacity)
  {
    size_t new_capacity = (vec->capacity == 0) ? 1 : vec->capacity * 2;
    printf("Resizing vector from capacity %zu to %zu...\n", vec->capacity, new_capacity);
    void *new_data = realloc(vec->data, new_capacity * vec->elem_size);
    if (!new_data)
    {
      fprintf(stderr, "Error: Memory allocation failed during vector resize for insertion. Cannot insert %d.\n", value);
      return;
    }
    vec->data = new_data;
    vec->capacity = new_capacity;
  }

  // Shift elements to the right to make space for the new element
  // Start from the end and move towards the insertion point
  for (size_t i = vec->length; i > (size_t)pos; i--)
  {
    memcpy((char *)vec->data + i * vec->elem_size,
           (char *)vec->data + (i - 1) * vec->elem_size,
           vec->elem_size);
  }

  // Insert the new value at the desired position
  memcpy((char *)vec->data + pos * vec->elem_size, &value, vec->elem_size);
  vec->length++; // Increment the length of the vector

  printf("Successfully inserted %d at position %d.\n", value, pos);
}

// Deletes a value at a specified position in the vector
void delete_at_position(Vec *vec, int pos)
{
  if (vec == NULL)
  {
    fprintf(stderr, "Error: Cannot delete from a NULL vector.\n");
    return;
  }
  if (vec->length == 0)
  {
    printf("Array is empty. Cannot delete any element.\n");
    return;
  }
  // Check bounds for deletion (pos must be within existing elements)
  if (pos < 0 || (size_t)pos >= vec->length)
  {
    printf("Error: Invalid position %d for deletion. Position must be between 0 and %zu.\n", pos, vec->length - 1);
    return;
  }

  // Get the value being deleted for user feedback (optional)
  int deleted_value = *(int *)vec_get(vec, pos);

  // Shift elements to the left to overwrite the deleted element
  for (size_t i = pos; i < vec->length - 1; i++)
  {
    memcpy((char *)vec->data + i * vec->elem_size,
           (char *)vec->data + (i + 1) * vec->elem_size,
           vec->elem_size);
  }
  vec->length--; // Decrement the length of the vector

  printf("Successfully deleted element '%d' at position %d.\n", deleted_value, pos);
}

// Prints all elements in the vector
void print_array(Vec *vec)
{
  if (vec == NULL)
  {
    printf("Cannot print a NULL vector.\n");
    return;
  }
  if (vec->length == 0)
  {
    printf("Array is empty (length: %zu).\n", vec->length);
    return;
  }
  printf("Array (length: %zu, capacity: %zu): |", vec->length, vec->capacity);
  for (size_t i = 0; i < vec->length; i++)
  {
    printf(" %d |", *(int *)vec_get(vec, i));
  }
  printf("\n");
}