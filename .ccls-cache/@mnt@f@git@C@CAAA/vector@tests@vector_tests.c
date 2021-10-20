#include "munit.h"
#include "vector.h"
#include "bstrlib.h"

struct tagbstring string1 = bsStatic("string1");
struct tagbstring string2 = bsStatic("string2");
struct tagbstring string3 = bsStatic("string3");

Vector *v;

static void* test_setup(const MunitParameter params[], void* user_data) {
  v = Vector_New();
  return v;
}

static void test_tear_down(void* fixture) {
  Vector_Destroy((Vector *)v);
}

MunitResult test_new(const MunitParameter params[], void* user_data_or_fixture){
	    check((Vector *)user_data_or_fixture != NULL,"Could not create vector");
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_add(const MunitParameter params[], void* user_data_or_fixture){
		Vector *data = (Vector *)user_data_or_fixture;
		Vector_Add(data,(void*)&string1);
		check(data->first!=NULL,"Did not add data");
		check(data->size==1,"Wrong Size after add");

		VectorNode *node = data->first;
		check(bstrcmp((bstring)node->data,(bstring)&string1)==0,"Inserted Data Wrong");
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_get(const MunitParameter params[], void* user_data_or_fixture){
	    Vector *v1 = (Vector*)user_data_or_fixture;
	    bstring data = (bstring)Vector_Get(v1,5);
		check(data == NULL,"Got no erro when out of bounds");
		Vector_Add(v1,(void*)&string1);

	    bstring data2 = (bstring)Vector_Get(v1,0);
		check(data2!=NULL,"Got no data");
		check(bstrcmp(data2,&string1)==0,"Wrong insert")
		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_add_more(const MunitParameter params[], void* user_data_or_fixture){
	    Vector *data = (Vector *)user_data_or_fixture;
		Vector_Add(data,(void*)&string1);
		check(data->first!=NULL,"Did not add data");
		check(data->size==1,"Wrong Size after add");
	    bstring data1 = (bstring)Vector_Get(data,0);
		check(data1!=NULL,"Got no data");
		check(bstrcmp(data1,&string1)==0,"Wrong insert")

		Vector_Add(data,(void*)&string2);
		check(data->first->next!=NULL,"Did not add data 2");
		check(data->size==2,"Wrong Size after add");
	    bstring data2 = (bstring)Vector_Get(data,1);
		check(data2!=NULL,"Got no data");
		check(bstrcmp(data2,&string2)==0,"Wrong insert")

		Vector_Add(data,(void*)&string3);
		check(data->first->next->next!=NULL,"Did not add data 3");
		check(data->size==3,"Wrong Size after add");
	    bstring data3 = (bstring)Vector_Get(data,2);
		check(data3!=NULL,"Got no data");
		check(bstrcmp(data3,&string3)==0,"Wrong insert")

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

MunitResult test_del(const MunitParameter params[], void* user_data_or_fixture){
	    Vector *data = (Vector *)user_data_or_fixture;
		Vector_Add(data,(void*)&string1);
		check(data->first!=NULL,"Did not add data");
		check(data->size==1,"Wrong Size after add");
	    bstring data1 = (bstring)Vector_Get(data,0);
		check(data1!=NULL,"Got no data");
		check(bstrcmp(data1,&string1)==0,"Wrong insert")

		Vector_Add(data,(void*)&string2);
		check(data->first->next!=NULL,"Did not add data");
		check(data->size==2,"Wrong Size after add");
	    bstring data2 = (bstring)Vector_Get(data,1);
		check(data2!=NULL,"Got no data");
		check(bstrcmp(data2,&string2)==0,"Wrong insert")

		Vector_Add(data,(void*)&string3);
		check(data->first->next->next!=NULL,"Did not add data");
		check(data->size==3,"Wrong Size after add");
	    bstring data3 = (bstring)Vector_Get(data,2);
		check(data3!=NULL,"Got no data");
		check(bstrcmp(data3,&string3)==0,"Wrong insert")

		bstring out = (bstring)Vector_Del(v,1);
		check(bstrcmp(out,&string2)==0,"Did not get correct value on delete");
		check(data->size==2,"Wrong Size");

		bstring datadel = (bstring)Vector_Get(data,1);
		check(datadel!=NULL,"Got no data");
		check(bstrcmp(datadel,&string3)==0,"Wrong insert %s %s",bdata(datadel),bdata(&string3));

		return MUNIT_OK;
error:
		return MUNIT_FAIL;
}

int main(int argc, char *argv[]){
	MunitTest tests[] = {
		{"test_new",test_new,test_setup,test_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{"test_add",test_add,test_setup,test_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{"test_get",test_get,test_setup,test_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{"test_add_more",test_add_more,test_setup,test_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{"test_del",test_del,test_setup,test_tear_down,MUNIT_TEST_OPTION_NONE,NULL},
		{NULL,NULL,NULL,NULL,MUNIT_TEST_OPTION_NONE,NULL}
	};
	
	const MunitSuite suite = {
		"Vector Tests",
		tests,
		NULL,
		1,
		MUNIT_SUITE_OPTION_NONE
	};

	return munit_suite_main(&suite, NULL,0,NULL);
}

