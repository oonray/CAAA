#ifdef _WIN32
#include <windows.h>
#endif
#include "munit.h"
#include "vector.h"

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
		Vector *arr = Vector_New(sizeof(int),100);

		check(arr->contents != NULL,"The content is NULL");
		check(arr->end == 0,"The end of the Vector is not at the right spot");
		check(arr->element_size == sizeof(int),"The element_size is wrong");
		check(arr->max == 100,"The max should be 100");

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_destroy(const MunitParameter params[], void* user_data_or_fixture){
		Vector_Destroy(user_data_or_fixture);
		return MUNIT_OK;
}

// Test Create ----------------------------------------------------------

static void *test_create_setup(const MunitParameter params[], void* user_data_or_fixture){
		return Vector_New(sizeof(int),100);
}

static void test_create_tear_down(void* user_data_or_fixture){
	free(user_data_or_fixture);
}

MunitResult test_create(const MunitParameter params[], void* user_data_or_fixture){
		check(user_data_or_fixture != NULL,"No data Recieved");

		int *val1 = Vector_Create(user_data_or_fixture);
		check(val1 != NULL,"Val not Created");

		int *val2 = Vector_Create(user_data_or_fixture);
		check(val2 != NULL,"Val2 not created");

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}


// Test Get ----------------------------------------------------------

MunitResult test_get(const MunitParameter params[], void* user_data_or_fixture){
		int *val1 = Vector_Create(user_data_or_fixture);
		int *val2 = Vector_Create(user_data_or_fixture);

		*val1 = 1;
		*val2 = 2;

		Vector_Set(user_data_or_fixture,0,val1);
		Vector_Set(user_data_or_fixture,1,val2);

		int *data = Vector_Get(user_data_or_fixture,0);
		check(data==val1,"Wrong Value in 1 wanted %d got %d",*val1,*data);

		data = Vector_Get(user_data_or_fixture,1);
		check(data==val2,"Wrong Value in 2 wanted %d got %d",*val2,*data);
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

// Test Set ----------------------------------------------------------

MunitResult test_set(const MunitParameter params[], void* user_data_or_fixture){
		int *val1 = Vector_Create(user_data_or_fixture);
		int *val2 = Vector_Create(user_data_or_fixture);

		Vector_Set(user_data_or_fixture,0,val1);
		Vector_Set(user_data_or_fixture,1,val2);
		return MUNIT_OK;
}

MunitResult test_remove(const MunitParameter params[], void* user_data_or_fixture){
		int *val1 = Vector_Create(user_data_or_fixture);
		int *val2 = Vector_Create(user_data_or_fixture);

		*val1 = 1;
		*val2 = 2;

		Vector_Set(user_data_or_fixture,0,val1);
		Vector_Set(user_data_or_fixture,1,val2);

		int *val_check = Vector_Remove(user_data_or_fixture,0);
		check(val_check!=NULL,"Value removed should not be NULL");	
		check(*val_check==*val1,"Wrong Value Recieved");
		check(Vector_Get(user_data_or_fixture,0) == NULL,"The data in the vector should not be null");

		Vector_Free(val_check);
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_expand_contract(const MunitParameter params[], void* user_data_or_fixture){
		int old_max = ((Vector *)user_data_or_fixture)->max;

		Vector_Expand(user_data_or_fixture);
		assert((unsigned int) ((Vector *)user_data_or_fixture)->max == old_max+((Vector *)user_data_or_fixture)->expand_rate);
		
		Vector_Contract(user_data_or_fixture);
		assert((unsigned int) ((Vector *)user_data_or_fixture)->max == ((Vector *)user_data_or_fixture)->expand_rate+1);
		return MUNIT_OK;
}

MunitResult test_push_pop(const MunitParameter params[], void* user_data_or_fixture){
		int i = 0;
		for(i = 0;i < 1000;i++){
			int *val = Vector_Create(user_data_or_fixture);
			*val = i * 333;

			Vector_Push(user_data_or_fixture,val);
		}

		check(((Vector *)user_data_or_fixture)->max == 1300,"MAx should be %d got %d",1300,((Vector *)user_data_or_fixture)->max);
		for(i=999;i>=0;i--){
			int *val = Vector_Pop(user_data_or_fixture);
			check(val!=NULL,"Pop Value Should not be NULL");
			check(*val==i*333,"Value content should be %d got %d",i*333,*val);
			Vector_Free(val);	
		}

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

#ifdef _WIN32
int WinMain (int argc, char *argv[]){
#else
int main(int argc, char *argv[]){
#endif
	MunitTest tests[] = {
		{" test_new",test_new,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_create",test_create,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_set",test_set,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_get",test_get,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_remove",test_remove,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_expand_contract",test_expand_contract,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_push_pop",test_push_pop,test_create_setup,test_create_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{" test_destroy",test_destroy,test_create_setup,NULL,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite ={
		"Vector Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

