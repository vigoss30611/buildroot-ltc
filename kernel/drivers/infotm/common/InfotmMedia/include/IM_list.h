/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
**
*****************************************************************************/ 

#ifndef __IM_LIST_H__
#define __IM_LIST_H__

template<typename T>
class IM_List{
public:	
	IM_List() :mHead(IM_NULL), mTail(IM_NULL), mCur(IM_NULL),
		mLength(0){
	}
	virtual ~IM_List(){
		clear();
	}

	typedef struct _node{
		T *me;
		_node *prev;
		_node *next;
	}node;

	/* returns true if the list is empty */
	IM_BOOL empty(){
		return (mLength==0)?IM_TRUE:IM_FALSE;
	}

	/* return #of elements in list */
	IM_INT32 size(){
		return mLength;
	}

	T *get(){
		if(mCur != IM_NULL){
			return mCur->me;
		}
		return IM_NULL;
	}

	/* Return the first/prev/next/tail/position element */
	T* begin(){
		if(mHead != IM_NULL){
			mCur = mHead;
			return mCur->me;
		}
		return IM_NULL;
	}

	T *next(){
		if(mCur != IM_NULL){
			if(mCur->next != IM_NULL){
				mCur = mCur->next;
				return mCur->me;
			}else{
				return IM_NULL;
			}
		}
		return IM_NULL;
	}

	T *prev(){
		if(mCur != IM_NULL){
			if(mCur->prev != IM_NULL){
				mCur = mCur->prev;
				return mCur->me;
			}else{
				return IM_NULL;
			}
		}
		return IM_NULL;
	}

	T* end(){
		if(mTail != IM_NULL){
			mCur = mTail;
			return mCur->me;
		}
		return IM_NULL;
	}

	/* add the object to the head or tail or after current or position of the list */
	IM_RET put_front(T* element){
		node *tmp = new node;
		if(tmp == IM_NULL){
			return IM_RET_NOMEMORY;
		}
		tmp->me = new T;
		if(tmp->me == IM_NULL){
			delete tmp;
			return IM_RET_NOMEMORY;
		}

		IM_ASSERT((tmp != IM_NULL) && (tmp->me != IM_NULL));
		memcpy(tmp->me, (void *)element, sizeof(T));

		if(mHead == IM_NULL){
			IM_ASSERT(mTail == IM_NULL);
			IM_ASSERT(mLength == 0);
			mHead = tmp;
			mHead->prev = IM_NULL;
			mHead->next = IM_NULL;
			mTail = mHead;
		}else{
			tmp->next = mHead;
			tmp->prev = IM_NULL;
			mHead->prev = tmp;
			mHead = tmp;
		}

		mCur = mHead;
		mLength++;
		return IM_RET_OK;
	}

	IM_RET put_back(T* element){
		node *tmp = new node;
		if(tmp == IM_NULL){
			return IM_RET_NOMEMORY;
		}
		tmp->me = new T;
		if(tmp->me == IM_NULL){
			delete tmp;
			return IM_RET_NOMEMORY;
		}

		IM_ASSERT((tmp != IM_NULL) && (tmp->me != IM_NULL));
		memcpy(tmp->me, (void *)element, sizeof(T));
	
		if(mTail == IM_NULL){
			IM_ASSERT(mHead == IM_NULL);
			IM_ASSERT(mLength == 0);
			mHead = tmp;
			mHead->prev = IM_NULL;
			mHead->next = IM_NULL;
			mTail = mHead;
		}else{
			tmp->prev = mTail;
			tmp->next = IM_NULL;
			mTail->next = tmp;
			mTail = tmp;
		}

		mCur = mTail;
		mLength++;
		return IM_RET_OK;
	}

	IM_RET insert(T* element){	// insert to current back, and this element will be current.
		if((mCur == IM_NULL) || (mCur == mTail)){
			return put_back(element);
		}
	
		node *tmp = new node;
		if(tmp == IM_NULL){
			return IM_RET_NOMEMORY;
		}
		tmp->me = new T;
		if(tmp->me == IM_NULL){
			delete tmp;
			return IM_RET_NOMEMORY;
		}

		IM_ASSERT((tmp != IM_NULL) && (tmp->me != IM_NULL));
		memcpy(tmp->me, (void *)element, sizeof(T));

		mCur->next->prev = tmp;
		tmp->next = mCur->next;
		tmp->prev = mCur;
		mCur->next = tmp;
		mCur = tmp;
		mLength++;
		return IM_RET_OK;
	}

/*	IM_BOOL find(T *element){
		node *tmp = mHead;
		while(tmp != IM_NULL){
			if(tmp->me == element){
				mCur = tmp;
				return IM_TRUE;
			}
			tmp = tmp->next;
		}
		return IM_FALSE;
	}*/

	/* remove one entry; returns iterator at next node*/
	T* erase(T* element){
		node *tmp = mHead;
		for(IM_INT32 i=0; i<mLength; i++)
		{
			if(tmp->me == element){	// found it.
				if(tmp == mHead){
					mHead = tmp->next;
					if(mHead == IM_NULL){	// just 1 element, head=tail=cur
						mTail = mCur = IM_NULL;
					}else{
						mHead->prev = IM_NULL;
						mCur = mHead;
					}
				}else if(tmp == mTail){
					IM_ASSERT(mHead != IM_NULL);
					mTail = tmp->prev;
					mTail->next = IM_NULL;
					//mCur = mTail;
					mCur = IM_NULL;
				}else{
					tmp->prev->next = tmp->next;
					tmp->next->prev = tmp->prev;
					mCur = tmp->next;
				}

				delete tmp->me;
				delete tmp;
				mLength--;
				if(mCur != IM_NULL){
					return mCur->me;
				}
				return IM_NULL;
			}else{
				tmp = tmp->next;
			}
		}
		//IM_ASSERT(0);	// shouldn't be here.
		return IM_NULL;
	}

	/* remove all contents of the list */
	void clear(){
		if(mLength == 0){
			return;
		}

		T* tmp = mHead->me;
		while(tmp != IM_NULL){
			tmp = erase(tmp);
		}
	}

	T* operator++(){
		return next();
	}
	T* operator--(){
		return prev();
	}

private:
	node *mHead, *mTail, *mCur;
	IM_INT32 mLength;
};


template<typename T>
class IM_Ring{
public:	
	IM_Ring(IM_INT32 depth){
		if(depth < 1){
			mDepth = 1;
		}else{
			mDepth = depth;
		}
	}
	~IM_Ring(){
		clear();
	}
	IM_RET put(IN T *data, OUT T *overflow = IM_NULL){
		if(mQueue.size() < mDepth){
			return mQueue.put_back(data);
		}else{
			IM_ASSERT(mQueue.size() == mDepth);
			T *over = mQueue.begin();
			if(overflow != IM_NULL){
				memcpy((void *)overflow, (void *)over, sizeof(T));
			}
			mQueue.erase(over);
			if(mQueue.put_back(data) != IM_RET_OK){
				return IM_RET_FAILED;
			}
			return IM_RET_OVERFLOW;
		}
	}
	IM_RET get(OUT T *data){
		if(data == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}
		T *tmp = mQueue.begin();
		if(tmp != IM_NULL){
			memcpy((void *)data, (void *)tmp, sizeof(T));
			mQueue.erase(tmp);
			return IM_RET_OK;
		}else{
			return IM_RET_FAILED;
		}
	}
	IM_INT32 size(){
		return mQueue.size();
	}
	IM_BOOL empty(){
		return mQueue.empty();
	}
	void clear(){
		mQueue.clear();
	}

private:
	IM_List<T> mQueue;
	IM_INT32 mDepth;
};

//
//
//
#include <IM_list_c.h>


#endif	// __IM_LIST_H__
