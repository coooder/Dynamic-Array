# Dynamic-Array
Develop and verify a C++ implementation of the dynamic_array class.
The dynamic_array class has 11 public functions and 5 private functions.
The correct behaviour of the public methods is specified as follows:

### Dynamic_array()
**Purpose**

         Create an array with size 0.

### Dynamic_array(Dynamic_array & d)
**Purpose**
            
         Create an array containing a copy of the elements in d

### Dynamic_array &operator=(Dynamic_array & d)
**Purpose**

         Copy the contents of d. Return a reference to this object.

### ~Dynamic_array()
**Purpose**

         Delete all memory dynamically allocated by this object.

### int get_size(void)
**Purpose**

         Return the size of the array.

### void print_state(void)
**Purpose**

         Write the internal state of the object to standard output.

### int& operator[](int i)
**Purpose**

         Return a reference to the element with index i.

### void insert(int x, int i)
**Purpose**

         Insert x at position i. 
         Shift elements at positions i and higher one position to the right.

### void insert(Dynamic_array & d, int i)
**Purpose**

         Insert the elements of d at position i. 
         Shift elements at positions i and higher n positions to the right, where n is the size of d.

### void remove(int i)
**Purpose**

         Remove the element at position i. 
         Shift elements i..n-1 one position to the left, where n is the size of the array.

### void remove(int start, int end)
**Purpose**

         Remove the elements at positions start..end-1. 
         Shift the elements in positions end..size-1 to the left by end-start positions.
