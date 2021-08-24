/*
 * src/tutorial/IntSet.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
//#include "libpq/pqformat.h"		/* needed for send/recv functions */ /* 不需要send 或者 receive，因此这个h文件可以不需要 */

PG_MODULE_MAGIC;

typedef struct IntSet
{
	char* set; // internal representation of the IntSet // 要用这种方式
	int length;
} IntSet;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

// 老师：Interface between the sql and you defined in the source file。
// 老师：它被编译成.o文件, which is linked to the postgres server and then become a data type.
// 老师：All your functions need to have this start
PG_FUNCTION_INFO_V1(IntSet_in);

Datum							// 老师：It returns a datum which is a postgres attribute value
	IntSet_in(PG_FUNCTION_ARGS) // 老师：They need to have this definition of this argument.
{
	char *str = PG_GETARG_CSTRING(0); // 老师：这表示它的input argument 是 string，对ass可能有用'{1,2,3}'. 这是如何build internal representation

	IntSet *result;

	if (sscanf(str, " ( %lf , %lf )", &x, &y) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type %s: \"%s\"",
						"IntSet", str)));

	result = (IntSet *)palloc(sizeof(IntSet)); // 老师：用palloc代替malloc，要返回pointer。他也不知道有啥区别，这两个
	result->x = x;
	result->y = y;
	PG_RETURN_POINTER(result); // 老师：A pointer of value wrapped in a datum，这个会造成问题，需要自己去看macros for making sure that you use the right one.老师的week3的exercise的slides里讲了这点的原因。
}

PG_FUNCTION_INFO_V1(IntSet_out);

Datum
	IntSet_out(PG_FUNCTION_ARGS)
{
	IntSet *IntSet = (IntSet *)PG_GETARG_POINTER(0);
	char *result;

	result = psprintf("(%g,%g)", IntSet->x, IntSet->y);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * New Operators
 *
 * A practical IntSet datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(IntSet_add);

Datum
	IntSet_add(PG_FUNCTION_ARGS)
{
	IntSet *a = (IntSet *)PG_GETARG_POINTER(0);
	IntSet *b = (IntSet *)PG_GETARG_POINTER(1);
	IntSet *result;

	result = (IntSet *)palloc(sizeof(IntSet));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	PG_RETURN_POINTER(result);
}