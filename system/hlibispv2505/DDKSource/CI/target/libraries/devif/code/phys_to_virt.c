//#include <map>
//#include <iostream>
#include <devif_api.h>

#ifdef WIN32
#include <vector>


extern "C" {
// : Replace this list with a binary tree or similar to make it faster.

//
// Keep track of virtual address of all known pages,
// using the physical address of the page as the key.
//
typedef struct
{
	IMG_UINT64	ui64Phyical;
	IMG_PVOID	pvHostAddr;
	IMG_UINT32	uiSize;
} MEM_ALLOC;

std::vector< MEM_ALLOC > AllocMappings;


// add an entry to the physical->virtual map
// replacing any existing entry.
IMG_BOOL
DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_UINT32 uiSize, IMG_VOID * virt)
{
	MEM_ALLOC MemAlloc;
	MemAlloc.pvHostAddr = virt;
	MemAlloc.ui64Phyical = phys;
	MemAlloc.uiSize = uiSize;
	AllocMappings.push_back( MemAlloc );

	return IMG_TRUE;
}


//
// lookup the virtual address, using the physical address as the key.
// return NULL if no such physical address.
//
IMG_VOID *
DEVIF_FindPhysToVirtEntry(IMG_UINT64 phys)
{
	std::vector< MEM_ALLOC >::iterator itr = AllocMappings.begin();
	while( itr != AllocMappings.end() )
	{
		if( ( phys >= itr->ui64Phyical ) && (phys < (itr->ui64Phyical + itr->uiSize ) ) )
		{
			return (IMG_VOID *)(((char*)itr->pvHostAddr) + ( phys - itr->ui64Phyical ))  ;
		}
		itr++;
	}
	return NULL;
}

IMG_VOID
DEVIF_RemovePhysToVirtEntry(IMG_UINT64 phys)
{
	std::vector< MEM_ALLOC >::iterator itr = AllocMappings.begin();
	while( itr != AllocMappings.end() )
	{
		if(  phys== itr->ui64Phyical )
		{
			AllocMappings.erase( itr );
			return;
		}
		itr++;
	}

}

}

#else

typedef struct TREE_NODE TREE_NODE;

struct TREE_NODE
{
    IMG_UINT64 	ui64PhysAddr;
    IMG_UINT64 	ui64VirtAddr;
    TREE_NODE* 	left;
    TREE_NODE* 	right;
};

TREE_NODE * pPhysToVirtTree = IMG_NULL;

/* Helper function that allocates a new node with the given data and IMG_NULL left and right pointers. */
TREE_NODE* newNode(const IMG_UINT64 ui64PhysAddr, const IMG_UINT64 ui64VirtAddr)
{
	TREE_NODE* node 	= malloc(sizeof(TREE_NODE));
	node->ui64PhysAddr 	= ui64PhysAddr;
	node->ui64VirtAddr 	= ui64VirtAddr;
	node->left 			= IMG_NULL;
	node->right 		= IMG_NULL;

  return(node);
}

/* Given a binary tree, return the virtual addr if a node with the target physical addr is found in the tree. */
static IMG_UINT64 lookup(const TREE_NODE* node, const IMG_UINT64 target)
{
  	/* Detect an empty tree, or the end of a branch */
  	if (node == IMG_NULL)
  	{
		return(IMG_NULL);
  	}
  	else
  	{
    	/* Detect if the address is found at this level in the tree */
		if (target == node->ui64PhysAddr)
		{
			return(node->ui64VirtAddr);
		}
    	else
    	{
	      	/* Still not found so recur down the correct subtree */
	      	if (target < node->ui64PhysAddr)
	      	{
				return(lookup(node->left, target));
		  	}
	      	else
	      	{
				return(lookup(node->right, target));
			}
		}
	}
}

/*  Inserts a new node with the given addresses in the correct place in the tree. Returns the new root pointer which the
	caller should then use (the standard trick to avoid using reference parameters). */
TREE_NODE* insert(TREE_NODE* node, const IMG_UINT64 ui64PhysAddr, const IMG_UINT64 ui64VirtAddr)
{
	// 1. If the tree is empty, return a new, single node
	if (node == IMG_NULL)
	{
		return(newNode(ui64PhysAddr, ui64VirtAddr));
	}
	else
	{
	// 2. Otherwise, recur down the tree
		if (ui64PhysAddr < node->ui64PhysAddr)
		{
			node->left = insert(node->left, ui64PhysAddr, ui64VirtAddr);
		}
		else if (ui64PhysAddr == node->ui64PhysAddr)
		{
			/* Already in the tree, just update the mapping */
			node->ui64VirtAddr = ui64VirtAddr;
		}
		else
		{
			node->right = insert(node->right, ui64PhysAddr, ui64VirtAddr);
		}

		return(node); // return the (unchanged) node pointer
	}
}

IMG_BOOL DEVIF_AddPhysToVirtEntry(IMG_UINT64 phys, IMG_VOID * virt)
{
	pPhysToVirtTree = insert(pPhysToVirtTree, phys, (IMG_UINT64)((long)virt));
	return IMG_TRUE;
}



/* lookup the virtual address, using the physical address as the key.
   return NULL if no such physical address. */
IMG_VOID * DEVIF_FindPhysToVirtEntry(IMG_UINT64 phys)
{
	/* Either the matching virtual address will be returned, or NULL if it cannot be found */
	return (IMG_VOID *)((long)lookup(pPhysToVirtTree, phys));
}

IMG_VOID DEVIF_RemovePhysToVirtEntry(IMG_UINT64 phys)
{
	// Do nothing for now -  - implement this to avoid memory leaks

}

#endif
