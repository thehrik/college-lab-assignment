#include <stdio.h>
#include <stdlib.h>
#include "./string/string.h" // Assuming String_new, String_read_line, String_destroy, ResultString, ERR
#include "./result/result.h" // Assuming Result, ERR, OK
#include "./input/input.h"   // Assuming int_read_line, destroy_read_result, ReadResult, READ_ERR, READ_OK, READ_STOPPED
#include "./types/types.h"   // Assuming common type definitions like ReadResult

// --- Linked List Node Structure ---
typedef struct Node
{
  int data;
  struct Node *next;
} Node;

// --- Function Prototypes ---
Node *new_node(int data);
void print_list(Node *head);
// Insertion operations
void insert_at_head(Node **head, int data);
void insert_at_tail(Node **head, int data);
void insert_at_index(Node **head, int data, int index);
// Deletion operations
void delete_at_head(Node **head);
void delete_at_tail(Node **head);
void delete_at_index(Node **head, int index);
void destroy_list(Node **head); // Function to free all nodes

// --- Main Function ---
int main()
{
  Node *head = NULL;
  int choice_val;
  ReadResult input_res; // Use ReadResult for all inputs

  printf("--- Linked List Operations ---\n");
  printf("1. Insert at head\n");
  printf("2. Insert at tail\n");
  printf("3. Insert at index\n");
  printf("4. Delete at head\n");
  printf("5. Delete at tail\n");
  printf("6. Delete at index\n");
  printf("7. Print list\n");
  printf("8. Exit\n");

  while (1)
  {
    printf("\nEnter your choice (enter 'q' to quit): ");
    input_res = int_read_line(); // Directly use your int_read_line()

    if (input_res.status == READ_STOPPED)
    {
      fprintf(stderr, "Program terminated by user request.\n");
      destroy_read_result(&input_res); // Ensure cleanup of the stopped result
      break;                           // Exit the loop and go to cleanup
    }
    else if (input_res.status == READ_ERR)
    {
      fprintf(stderr, "Input Error: %s. Please enter a valid integer or 'q'.\n", input_res.data.err_str);
      destroy_read_result(&input_res); // Clean up the error string
      continue;                        // Prompt for choice again
    }

    // If READ_OK, get the value and destroy the result
    choice_val = *(int *)input_res.data.ok;
    destroy_read_result(&input_res); // Free the allocated integer for choice

    switch (choice_val)
    {
    case 1: // Insert at head
      printf("Enter data to insert (enter 'q' to cancel): ");
      input_res = int_read_line();
      if (input_res.status == READ_STOPPED)
      {
        fprintf(stderr, "Insert operation cancelled by user.\n");
        destroy_read_result(&input_res); // Clean up the stopped result
        break;
      }
      else if (input_res.status == READ_ERR)
      {
        fprintf(stderr, "Input Error: %s. Insert operation cancelled.\n", input_res.data.err_str);
        destroy_read_result(&input_res);
        break;
      }
      insert_at_head(&head, *(int *)input_res.data.ok);
      destroy_read_result(&input_res); // Free the allocated integer for data
      break;

    case 2: // Insert at tail
      printf("Enter data to insert (enter 'q' to cancel): ");
      input_res = int_read_line();
      if (input_res.status == READ_STOPPED)
      {
        fprintf(stderr, "Insert operation cancelled by user.\n");
        destroy_read_result(&input_res);
        break;
      }
      else if (input_res.status == READ_ERR)
      {
        fprintf(stderr, "Input Error: %s. Insert operation cancelled.\n", input_res.data.err_str);
        destroy_read_result(&input_res);
        break;
      }
      if (head == NULL)
      {                                                   // Handle case for empty list where tail insert is head insert
        insert_at_head(&head, *(int *)input_res.data.ok); // insert_at_head prints success
      }
      else
      {
        insert_at_tail(&head, *(int *)input_res.data.ok);
      }
      destroy_read_result(&input_res);
      break;

    case 3: // Insert at index
      printf("Enter data to insert (enter 'q' to cancel): ");
      input_res = int_read_line();
      if (input_res.status == READ_STOPPED)
      {
        fprintf(stderr, "Insert operation cancelled by user.\n");
        destroy_read_result(&input_res);
        break;
      }
      else if (input_res.status == READ_ERR)
      {
        fprintf(stderr, "Input Error: %s. Insert operation cancelled.\n", input_res.data.err_str);
        destroy_read_result(&input_res);
        break;
      }
      int data_to_insert = *(int *)input_res.data.ok;
      destroy_read_result(&input_res); // Free data_to_insert result

      printf("Enter index (enter 'q' to cancel): ");
      input_res = int_read_line();
      if (input_res.status == READ_STOPPED)
      {
        fprintf(stderr, "Insert operation cancelled by user.\n");
        destroy_read_result(&input_res);
        break;
      }
      else if (input_res.status == READ_ERR)
      {
        fprintf(stderr, "Input Error: %s. Insert operation cancelled.\n", input_res.data.err_str);
        destroy_read_result(&input_res);
        break;
      }
      int insert_index = *(int *)input_res.data.ok;
      destroy_read_result(&input_res); // Free index result

      insert_at_index(&head, data_to_insert, insert_index);
      break;

    case 4: // Delete at head
      delete_at_head(&head);
      break;

    case 5: // Delete at tail
      delete_at_tail(&head);
      break;

    case 6: // Delete at index
      printf("Enter index to delete at (enter 'q' to cancel): ");
      input_res = int_read_line();
      if (input_res.status == READ_STOPPED)
      {
        fprintf(stderr, "Delete operation cancelled by user.\n");
        destroy_read_result(&input_res);
        break;
      }
      else if (input_res.status == READ_ERR)
      {
        fprintf(stderr, "Input Error: %s. Delete operation cancelled.\n", input_res.data.err_str);
        destroy_read_result(&input_res);
        break;
      }
      int delete_index = *(int *)input_res.data.ok;
      destroy_read_result(&input_res); // Free index result
      delete_at_index(&head, delete_index);
      break;

    case 7: // Print list
      print_list(head);
      break;

    case 8: // Exit
      printf("Exiting program.\n");
      goto cleanup; // Jump to cleanup label

    default:
      printf("Invalid choice. Please enter a number between 1 and 8.\n");
      break;
    }
  }

cleanup:
  printf("Cleaning up linked list memory...\n");
  destroy_list(&head); // Free all remaining nodes
  printf("Program terminated.\n");
  return 0;
}

// --- Linked List Operations Implementation ---

// Creates a new node and allocates memory for it
Node *new_node(int data)
{
  Node *node = malloc(sizeof(Node));
  if (node == NULL)
  {
    fprintf(stderr, "Error: Memory allocation failed for new node (data: %d). Returning NULL.\n", data);
  }
  node->data = data;
  node->next = NULL;
  return node;
}

// Prints all elements in the list
void print_list(Node *head)
{
  if (head == NULL)
  {
    printf("List is empty.\n");
    return;
  }
  Node *current = head;
  printf("List: ");
  while (current != NULL)
  {
    printf("%d ", current->data);
    current = current->next;
  }
  printf("\n");
}

// Inserts a new node at the beginning of the list
void insert_at_head(Node **head, int data)
{
  Node *new = new_node(data);
  if (new == NULL)
  { // Handle allocation failure from new_node
    return;
  }
  new->next = *head;
  *head = new;
  printf("Successfully inserted %d at head.\n", data);
}

// Inserts a new node at the end of the list
void insert_at_tail(Node **head, int data)
{
  Node *new = new_node(data);
  if (new == NULL)
  { // Handle allocation failure
    return;
  }
  if (*head == NULL)
  {
    *head = new; // If list is empty, new node becomes the head
  }
  else
  {
    Node *current = *head;
    while (current->next != NULL)
    {
      current = current->next;
    }
    current->next = new;
  }
  printf("Successfully inserted %d at tail.\n", data);
}

// Inserts a new node at a specified index
void insert_at_index(Node **head, int data, int index)
{
  if (index < 0)
  {
    printf("Error: Index cannot be negative (%d).\n", index);
    return;
  }
  if (index == 0)
  {
    insert_at_head(head, data); // This already prints success
    return;
  }
  Node *current = *head;
  // Traverse to the node *before* the insertion point
  for (int i = 0; i < index - 1 && current != NULL; i++)
  {
    current = current->next;
  }
  if (current == NULL)
  {
    printf("Error: Index %d is out of bounds. List has fewer than %d elements to insert at this position.\n", index, index);
    return;
  }
  Node *new = new_node(data);
  if (new == NULL)
  { // Handle allocation failure
    return;
  }
  new->next = current->next;
  current->next = new;
  printf("Successfully inserted %d at index %d.\n", data, index);
}

// Deletes the node at the beginning of the list
void delete_at_head(Node **head)
{
  if (*head == NULL)
  {
    printf("List is empty. Cannot delete from head.\n");
    return;
  }
  Node *current = *head;
  *head = current->next; // Move head to the next node
  printf("Deleted head node with data: %d.\n", current->data);
  free(current); // Free the old head node
}

// Deletes the node at the end of the list
void delete_at_tail(Node **head)
{
  if (*head == NULL)
  {
    printf("List is empty. Cannot delete from tail.\n");
    return;
  }
  Node *current = *head;
  if (current->next == NULL) // Only one node in the list
  {
    printf("Deleted the only node (tail) with data: %d.\n", current->data);
    free(current);
    *head = NULL;
    return;
  }
  // Traverse to the second to last node
  while (current->next->next != NULL)
  {
    current = current->next;
  }
  printf("Deleted tail node with data: %d.\n", current->next->data);
  free(current->next);  // Free the last node
  current->next = NULL; // Set the new last node's next to NULL
}

// Deletes the node at a specified index
void delete_at_index(Node **head, int index)
{
  if (*head == NULL)
  {
    printf("List is empty. Cannot delete at index %d.\n", index);
    return;
  }
  if (index < 0)
  {
    printf("Error: Index cannot be negative (%d).\n", index);
    return;
  }
  if (index == 0) // If deleting at head, use delete_at_head
  {
    delete_at_head(head); // This already prints success
    return;
  }

  Node *current = *head;
  // Traverse to the node *before* the deletion point
  for (int i = 0; i < index - 1 && current != NULL; i++)
  {
    current = current->next;
  }

  if (current == NULL || current->next == NULL)
  {
    printf("Error: Index %d is out of bounds. No node to delete at this position.\n", index);
    return;
  }

  Node *node_to_delete = current->next;
  current->next = node_to_delete->next; // Bypass the node to be deleted
  printf("Deleted node at index %d with data: %d.\n", index, node_to_delete->data);
  free(node_to_delete); // Free the node
}

// Frees all nodes in the linked list
void destroy_list(Node **head)
{
  Node *current = *head;
  Node *next_node;
  while (current != NULL)
  {
    next_node = current->next; // Save pointer to the next node
    free(current);             // Free the current node
    current = next_node;       // Move to the next node
  }
  *head = NULL; // Set head to NULL after freeing all nodes
}