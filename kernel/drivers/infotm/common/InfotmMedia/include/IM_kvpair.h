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
** v1.4.0	leo@2012/08/13: modify the 3 of IM_Kvpair::getKey() parameters to OUT.
**
*****************************************************************************/ 

#ifndef __IM_KVPAIR_H__
#define __IM_KVPAIR_H__

class IM_Kvpair{
public:	
	typedef struct{
		IM_INT32 key;
		void *value;
		IM_INT32 size;
	}kv_pair;

	IM_Kvpair(){mQueue.clear();}
	~IM_Kvpair(){clear();}

	IM_RET setKey(IN IM_INT32 key, IN void *value, IN IM_INT32 size){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}

		kv_pair *kv = mQueue.begin();
		while(kv != IM_NULL){
			if(kv->key == key){
				if(kv->size != size){
					//void *pv = (void *)new IM_UINT8[size];
					void *pv = malloc(size);
					if(pv == IM_NULL){
						return IM_RET_NOMEMORY;
					}
					//delete []kv->value;
					free(kv->value);
					kv->value = pv;
					kv->size = size;
				}
				memcpy(kv->value, value, size);
				return IM_RET_OK;
			}
			kv = mQueue.next();
		}
			
		kv_pair kvnew;
		//kvnew.value = (void *)new IM_UINT8[size];
		kvnew.value = malloc(size);
		if(kvnew.value == IM_NULL){
			return IM_RET_NOMEMORY;	
		}
		memcpy(kvnew.value, value, size);
		kvnew.key = key;
		kvnew.size = size;
		if(mQueue.put_back(&kvnew) != IM_RET_OK){
			//delete []kvnew.value;
			free(kvnew.value);
			return IM_RET_FAILED;
		}
		return IM_RET_OK;
	}

	IM_RET getKey(IN IM_INT32 key, OUT void **value, OUT IM_INT32 *size=IM_NULL){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}
		
		kv_pair *kv = mQueue.begin();
		while(kv != IM_NULL){
			if(kv->key == key){
				*value = kv->value;
				if(size != IM_NULL){
					*size = kv->size;
				}
				return IM_RET_OK;
			}
			kv = mQueue.next();
		}
		return IM_RET_FAILED;
	}

	IM_RET compareKey(IN IM_INT32 key, IN void *value, IN IM_INT32 size){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}

		kv_pair *kv = mQueue.begin();
		while(kv != IM_NULL){
			if(kv->key == key){
				if(size < kv->size){
					return IM_RET_FAILED;
				}
				IM_UINT8 *dst = (IM_UINT8 *)kv->value;
				IM_UINT8 *src = (IM_UINT8 *)value;
				for(IM_INT32 i=0; i<kv->size; i++){
					if(dst[i] != src[i]){
						return IM_RET_FALSE;
					}
				}
				return IM_RET_OK;
			}
			kv = mQueue.next();
		}
		return IM_RET_FAILED;
	}

	IM_RET begin(OUT IM_INT32 *key, OUT void **value, OUT IM_INT32 *size){
		kv_pair *kv = mQueue.begin();
		if(kv != IM_NULL){
			*key = (IM_INT32)kv->key;
			*value = kv->value;
			*size = (IM_INT32)kv->size;
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET next(OUT IM_INT32 *key, OUT void **value, OUT IM_INT32 *size){
		kv_pair *kv = mQueue.next();
		if(kv != IM_NULL){
			*key = (IM_INT32)kv->key;
			*value = kv->value;
			*size = (IM_INT32)kv->size;
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET prev(OUT IM_INT32 *key, OUT void **value, OUT IM_INT32 *size){
		kv_pair *kv = mQueue.prev();
		if(kv != IM_NULL){
			*key = (IM_INT32)kv->key;
			*value = kv->value;
			*size = (IM_INT32)kv->size;
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET end(OUT IM_INT32 *key, OUT void **value, OUT IM_INT32 *size){
		kv_pair *kv = mQueue.end();
		if(kv != IM_NULL){
			*key = (IM_INT32)kv->key;
			*value = kv->value;
			*size = (IM_INT32)kv->size;
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET removeKey(IN IM_INT32 key){
		kv_pair *kv = mQueue.begin();
		while(kv != IM_NULL){
			if(kv->key == key){
				//delete []kv->value;
				free(kv->value);
				mQueue.erase(kv);
				return IM_RET_OK;
			}
			kv = mQueue.next();
		}
		return IM_RET_OK;
	}

	void clear(){
		kv_pair *kv = mQueue.begin();
		while(kv != IM_NULL){
			//delete []kv->value;
			free(kv->value);
			kv = mQueue.erase(kv);
		}
		IM_ASSERT(mQueue.empty() == IM_TRUE);
	}

	IM_INT32 size(){
		return mQueue.size();
	}
	IM_BOOL empty(){
		return mQueue.empty();
	}

private:
	IM_List<kv_pair> mQueue;
};

// vector, mey used in key & multi-values enumerator.
class IM_Vector{
public:
	typedef struct{
		void *value;
		IM_INT32 size;
	}vector;

	IM_Vector(){mQueue.clear();}
	~IM_Vector(){clear();}

	IM_RET begin(OUT void **value, OUT IM_INT32 *size = IM_NULL){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}	
		vector *vec = mQueue.begin();
		if(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			*value = vec->value;
			if(size != IM_NULL){
				*size = vec->size;
			}
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET next(OUT void **value, OUT IM_INT32 *size = IM_NULL){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}	
		vector *vec = mQueue.next();
		if(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			*value = vec->value;
			if(size != IM_NULL){
				*size = vec->size;
			}
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET prev(OUT void **value, OUT IM_INT32 *size = IM_NULL){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}	
		vector *vec = mQueue.prev();
		if(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			*value = vec->value;
			if(size != IM_NULL){
				*size = vec->size;
			}
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET end(OUT void **value, OUT IM_INT32 *size = IM_NULL){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}	
		vector *vec = mQueue.end();
		if(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			*value = vec->value;
			if(size != IM_NULL){
				*size = vec->size;
			}
			return IM_RET_OK;
		}
		return IM_RET_FAILED;
	}

	IM_RET append(IN void *value, IN IM_INT32 size){
		if((value == IM_NULL) || (size <= 0)){
			return IM_RET_INVALID_PARAMETER;
		}

		IM_UINT8 *dst, *src;
		IM_INT32 i;
		vector *vec = mQueue.begin();
		while(vec != IM_NULL){
			if(vec->size == size){//only same size the value may be equal.
				dst = (IM_UINT8 *)vec->value;
				src = (IM_UINT8 *)value;
				for(i=0; i<size; i++){
					if(dst[i] != src[i]){
						break;
					}
				}
				if(i == size){//equal.
					return IM_RET_OK;
				}
			}
			vec = mQueue.next();
		}

		vector vecnew;
		//vecnew.value = (void *)new IM_UINT8[size];
		vecnew.value = malloc(size);
		if(vecnew.value == IM_NULL){
			return IM_RET_NOMEMORY;
		}
		memcpy(vecnew.value, value, size);
		vecnew.size = size;
		if(mQueue.put_back(&vecnew) != IM_RET_OK){
			//delete []vecnew.value;
			free(vecnew.value);
			return IM_RET_FAILED;
		}
		return IM_RET_OK;
	}

	IM_RET remove(IN void *value){
		if(value == IM_NULL){
			return IM_RET_INVALID_PARAMETER;
		}	
		vector *vec = mQueue.begin();
		while(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			if(value == vec->value){
				//delete []vec->value;
				free(vec->value);
				mQueue.erase(vec);
				return IM_RET_OK;
			}
			vec = mQueue.next();
		}
		return IM_RET_OK;
	}

	IM_INT32 size(){
		return mQueue.size();
	}

	void clear(){
		vector *vec = mQueue.begin();
		while(vec != IM_NULL){
			IM_ASSERT(vec->value != IM_NULL);
			//delete []vec->value;
			free(vec->value);
			vec = mQueue.erase(vec);
		}
		IM_ASSERT(mQueue.empty() == IM_TRUE);
	}

private:
	IM_List<vector> mQueue;
};	

//
//
//
#include <IM_kvpair_c.h>


#endif	// __IM_KVPAIR_H__

