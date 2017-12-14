#include "lst.h"
#include "dq.h"
#include "tre.h"
#include "stdio.h"


// Basic Compile Test should be upgraded to proper unit test

struct testlist
{
	LST_LINK;
	int num;
};



int main(int argc, char *argv[])
{
	LST_T list;
	int i;
	struct testlist testitems[5], *listitem;
	
	LST_init(&list);
	for (i = 0; i < 5; i++)
	{
		testitems[i].num = i;
		LST_add(&list, &testitems[i]);
	}

	listitem = LST_first(&list);
	for (i = 0; i < 5; i++)
	{
		if (listitem->num != i)
		{
			printf("List order invalid\n");
			return -1;
		}
		listitem = LST_next(listitem);
	}

	LST_empty(&list);
	return 0;
}

