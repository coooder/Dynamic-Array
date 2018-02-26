#include <iostream>										//-
#include <string.h>										//-
#include "dynamic_array.h"									//-
												//-
using namespace std;										//-
												//-
// ********** public functions **********							//-
												//-
Dynamic_array::Dynamic_array() {								//-
	head_p = NULL;										//-
	size = 0;										//-
}												//-
												//-
Dynamic_array::Dynamic_array(Dynamic_array & d) {						//-
	if(d.size==0){
		head_p = NULL;
		size = 0;
	}
	Block * new_block_p = copy_blocks(d.head_p);
	head_p = new_block_p;
	size=d.size;
}												//-
												//-
Dynamic_array &Dynamic_array::operator=(Dynamic_array & d) {					//-
	if(d.size==0){
		head_p = NULL;
		size = 0;
		return *this;
	}
	Block * new_block_p = copy_blocks(d.head_p);
	head_p = new_block_p;
	size=d.size;
	return *this;										//-
}												//-
												//-
Dynamic_array::~Dynamic_array() {								//-
	delete head_p;
	size=0;
}												//-
												//-
void Dynamic_array::print_state(void) {								//-
	cout << "size:" << size << endl;							//-
	for (Block * p = head_p; p != NULL; p = p->next_p) {					//-
		cout << "\tsize:" << p->size << " elements:";					//-
		for (int i = 0; i < p->size; i++) {						//-
			cout << p->a[i] << ' ';							//-
		}										//-
		cout << endl;									//-
	}											//-
}												//-
												//-
int Dynamic_array::get_size(void) {								//-
	return size;
}												//-
												//-
int& Dynamic_array::operator[](int i) {								//-
	// check i for range error								//-
	if (i < 0 || i >= size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// return element at index i								//-
	return position.block_p->a[position.i];							//-
}												//-
												//-
void Dynamic_array::insert(int x, int i) {							//-
	// case 1: range error									//-
	if (i < 0 || i > size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// case 2: empty array									//-
	if (size == 0) {									//-
		// create linked list consisting of a single new block				//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->a[0] = x;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// insert new block								//-
		insert_blocks(NULL, new_block_p);						//-
												//-
		// update total size								//-
		size++;										//-
												//-
		return;										//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// case 3: non-empty array; new block not needed					//-
	if (position.block_p->size < BLOCK_SIZE) {						//-
		// shift block array right							//-
		for (int j = position.block_p->size; j > position.i; j--) {			//-
			position.block_p->a[j] = position.block_p->a[j-1];			//-
		}										//-
												//-
		// assign x									//-
		position.block_p->a[position.i] = x;						//-
												//-
		// update array and block size							//-
		size++;										//-
		position.block_p->size++;							//-
												//-
	// case 4: non-empty array; new block needed						//-
	} else {										//-
		// create new block								//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// case 4.a: insert x into old block						//-
		if (position.i < BLOCK_SIZE) {							//-
			// copy last array element to new block					//-
			new_block_p->a[0] = position.block_p->a[BLOCK_SIZE-1];			//-
												//-
			// shift old block array right						//-
			for (int j = position.block_p->size-1; j > position.i; j--) {		//-
				position.block_p->a[j] = position.block_p->a[j-1];		//-
			}									//-
												//-
			// assign x into old block						//-
			position.block_p->a[position.i] = x;					//-
												//-
		// case 4.b: insert x into new block						//-
		} else {									//-
			new_block_p->a[0] = x;							//-
		}										//-
												//-
		// update total size								//-
		size++;										//-
												//-
		// insert new block into linked list						//-
		insert_blocks(position.block_p, new_block_p);					//-
	}											//-
}												//-
												//-
void Dynamic_array::insert(Dynamic_array &p, int i) {						//-
	// case 1: range error									//-
	if(i<0 || i> size){
		throw Subscript_range_exception();
	}
	// case 2: parameter array empty							//-
	if(p.size==0){
		return;
	}
	// case 3: array empty									//-
	if(size==0){
		Block * new_block_p = copy_blocks(p.head_p);
		insert_blocks(NULL,new_block_p);
		size=size+p.size;
		return;
	}
	// find target block and index								//-
	Block_position position = find_block(i);						//-
	// case 4: array non-empty; new blocks not needed					//-
   	 if(position.block_p->size!=0 && position.block_p->size + p.size <=BLOCK_SIZE){
		for(int j = position.i; j < position.block_p->size; j++){
			position.block_p->a[j+p.size] = position.block_p->a[j];
		}
		for(int k=position.i;k<position.i+p.size;k++){
			position.block_p->a[k]=p[k-position.i];
		}
	size += p.size;
	position.block_p->size +=p.size;
	return;
	}
	// case 5: array non-empty; new blocks needed						//-
	if(size!=0 && position.block_p->size + p.size>BLOCK_SIZE){
		// copy p									//-
		Block * copy_p = copy_blocks(p.head_p);						//-

		// case 5.a: insert position at start of block					//-
		if(position.i==0){
			insert_blocks(position.pre_block_p,copy_p);
		}

		// case 5.b: insert position at middle of block					//-
		if(position.i>0 && position.i<BLOCK_SIZE){
			Block * new_block_p = new Block;
			new_block_p->size=0;
			new_block_p->next_p = NULL;
			Block * block = new Block;
			block->size = 0;
			block->next_p = copy_p;
			for(int j=position.i;j<position.block_p->size;j++){
				new_block_p->a[j-position.i]= position.block_p->a[j];
				new_block_p->size++;
			}
			for(int j=0;j<position.i;j++){
				block->a[j] = position.block_p->a[j];
				block->size++;
			}
		insert_blocks(position.pre_block_p,block);
		copy_p->next_p = new_block_p;
		}
		// case 5.c: insert position after end of block					//-
		if(i==size){
			insert_blocks(position.block_p,copy_p);
			copy_p->next_p =NULL;
		}
		// update total size								//-
		size=size+p.size;
	}
}												//-
												//-
void Dynamic_array::remove(int i) {								//-
	// case 1: range error									//-
	if(i<0 || i >= size){
		throw Subscript_range_exception();
	}
	// find target block and index								//-
	Block_position pos = find_block(i);
	// case 2: block size == 1								//-
	if(pos.block_p->size==1){
		remove_blocks(pos.pre_block_p,pos.block_p,pos.block_p);
	}
	// case 3: block size > 1								//-
	if(pos.block_p->size>1){
		for(int j=pos.i;j<pos.block_p->size;j++){
			pos.block_p->a[j]=pos.block_p->a[j+1];
		}
	pos.block_p->size--;
	}
	// update total size									//-
	size--;
}												//-
												//-
void Dynamic_array::remove(int start, int end) {						//-
	//case 1:range error
	
	if(start<0 || start>end || end>size){
		throw Subscript_range_exception();
	}
	//case 2:start = end return
	
	if(start==end){
		return;
	}
	if(size==0){
		return;
	}
	//find target block and index
	
	Block_position post = find_block(start);
	Block_position poen = find_block(end);
	
	//remove blocks
	if(post.i==0&&poen.i==0){
		remove_blocks(post.pre_block_p,post.block_p,poen.pre_block_p);
		size -= (end-start);
		return;
	}
	//start index !=0 but end =5;
	
	if(post.i!=0&&poen.i==0){
			
			Block * block = new Block;
			block->size =0;
			block->next_p = poen.block_p;
			for(int k=0;k<post.i;k++){
				block->a[k]=post.block_p->a[k];
				block->size++;
			}
			remove_blocks(post.pre_block_p,post.block_p,poen.pre_block_p);
			insert_blocks(poen.pre_block_p,block);
			size -= (end-start);
			return;
	}
	//start index =0 but end !=0
	
	if(post.i==0&&poen.i!=0){
		if(end==size){
		remove_blocks(post.pre_block_p,post.block_p,poen.block_p);
		}
		else{
			Block * block = new Block;
			block->size = 0;
			block->next_p =poen.block_p->next_p;
			for(int k=poen.i;k<poen.block_p->size;k++){
				block->a[k-poen.i] = poen.block_p->a[k];
				block->size++;
			}
			remove_blocks(post.pre_block_p,post.block_p,poen.block_p);
			insert_blocks(post.pre_block_p,block);
			}
			size -= (end-start);
			return;
	}
	//start index!=0 while end index!=0
	
	if(post.i!=0&&poen.i!=0){		
		if(end==size){
		Block * block = new Block;
		block->size = 0;
		block->next_p = NULL;
		for(int k=0;k<post.i;k++){
			block->a[k] = post.block_p->a[k];
			block->size++;
		}
		remove_blocks(post.pre_block_p,post.block_p,poen.block_p);
		insert_blocks(post.pre_block_p,block);
		}
		else if(post.block_p!=poen.block_p){
			Block * block = new Block;
			block->size = 0;
			block->next_p = NULL;
			Block * newblock = new Block;
			newblock->size = 0;
			newblock->next_p = poen.block_p->next_p;
			for(int k =0;k<post.i;k++){
				block->a[k] = post.block_p->a[k];
				block->size++;
			}
			for(int k=poen.i;k<poen.block_p->size;k++){
				newblock->a[k-poen.i] = poen.block_p->a[k];
				newblock->size++;
			}
			remove_blocks(post.pre_block_p,post.block_p,poen.block_p);
			insert_blocks(post.pre_block_p,block);
			block->next_p = newblock;
		}
		else{
			Block * block = new Block;
			block->size = 0;
			block->next_p = NULL;
			for(int k=0;k<post.i;k++){
				block->a[k] = post.block_p->a[k];
				block->size++;
			}
			for(int k=poen.i;k<poen.block_p->size;k++){
				block->a[k-poen.i+post.i] = poen.block_p->a[k];
				block->size++;
			}
			remove_blocks(post.pre_block_p,post.block_p,poen.block_p);
			insert_blocks(post.pre_block_p,block);
		}
		size -= (end-start);
	}
}												//-
												//-
// ********** private functions **********							//-
												//-
// purpose											//-
//	return b where										//-
//		if i < size									//-
//			b.block_p->a[b.i] is the ith element overall				//-
//		else										//-
//			b.block_p->a[b.i-1] is the i-1st element overall			//-
//												//-
//		if b.block_p == head_p								//-
//			b.pre_block_p is NULL							//-
//		else										//-
//			b.pre_block_p points to block preceding b.block_p			//-
// preconditions										//-
//	i in [1..size]										//-
Dynamic_array::Block_position Dynamic_array::find_block(int i) {				//-
	Block_position position;								//-
												//-
	// scan Block list									//-
	position.i = i;										//-
	position.pre_block_p = NULL;								//-
	for (position.block_p = head_p;								//-
	position.block_p != NULL;								//-
	position.block_p = position.block_p->next_p) {						//-
		// found in current block							//-
		if (position.i < position.block_p->size) {					//-
			break;									//-
		}										//-
												//-
		// special case: insert just after last element					//-
		if (i == size && position.block_p->next_p == NULL) {				//-
			break;									//-
		}										//-
												//-
		// not found yet: advance							//-
		position.pre_block_p = position.block_p;					//-
		position.i -= position.block_p->size;						//-
	}											//-
												//-
	return position;									//-
}												//-
												//-
// purpose											//-
//	create a new linked list which is a copy of the list pointed to p			//-
//	return a pointer to the head of the new linked list					//-
// preconditions										//-
//	p is the head of a possibly empty linked list of blocks					//-
Dynamic_array::Block * Dynamic_array::copy_blocks(Block * p) {					//-
	Block * new_head_p = NULL;								//-
	Block * new_p;										//-
	while (p != NULL) {									//-
		// allocate and link in new block						//-
		if (new_head_p == NULL) {							//-
			new_p = new Block;							//-
			new_head_p = new_p;							//-
		} else {									//-
			new_p->next_p = new Block;						//-
			new_p = new_p->next_p;							//-
		}										//-
												//-
		// copy the elements								//-
		new_p->size = p->size;								//-
		for (int i = 0; i < p->size; i++) {						//-
			new_p->a[i] = p->a[i];							//-
		}										//-
												//-
		// advance									//-
		p = p->next_p;									//-
	}											//-
												//-
	// terminate new list									//-
	if (new_head_p != NULL) {								//-
		new_p->next_p = NULL;								//-
	}											//-
												//-
	return new_head_p;									//-
}												//-
												//-
// purpose											//-
//	insert the list headed by src_p into the list headed by head_p				//-
//	if dst_p is NULL									//-
//		insert the list at the start of the list headed by head_p			//-
//	else											//-
//		insert the list just after block dst_p						//-
// preconditions										//-
//	list headed by src_p is non-empty							//-
//	list headed by src_p has no blocks in common with the list headed by head_p		//-
void Dynamic_array::insert_blocks(Block * dst_p, Block * src_p) {				//-
	// find the last block in the list headed by src_p					//-
	Block * p = src_p;									//-
	while (p->next_p != NULL) {								//-
		p = p->next_p;									//-
	}											//-
												//-
	// insert at head									//-
	if (dst_p == NULL) { // insert at head							//-
		p->next_p = head_p;								//-
		head_p = src_p;									//-
												//-
	// insert after dst_p									//-
	} else { // insert after dst_p								//-
		p->next_p = dst_p->next_p;							//-
		dst_p->next_p = src_p;								//-
	}											//-
}												//-
												//-
// purpose											//-
//	remove the blocks pointed to by start_p and end_p, and all the blocks between		//-
// preconditions										//-
//	start_p and end_p point to blocks in the list headed by head_p				//-
//	end_p points to either the same block as start_p or a block to its right		//-
//	if start_p == head_p									//-
//		pre_start_p == NULL								//-
//	else											//-
//		pre_start_p points to the block immediately preceding start_p			//-
//												//-
void Dynamic_array::remove_blocks(Block * pre_start_p, Block * start_p, Block * end_p) {	//-
	// release the blocks									//-
	while (1) {										//-
		// release start_p								//-
		Block * p = start_p->next_p;							//-
		delete start_p;									//-
												//-
		// advance									//-
		if (start_p == end_p) {								//-
			break;									//-
		} else {									//-
			start_p = p;								//-
		}										//-
	}											//-
												//-
	// link left and right sublists								//-
	if (pre_start_p == NULL) {								//-
		head_p = end_p->next_p;								//-
	} else {										//-
		pre_start_p->next_p = start_p->next_p;						//-
	}											//-
}												//-
