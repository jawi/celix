/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * array_list_test.c
 *
 *  \date       Aug 4, 2010
 *  \author    	<a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Automated.h>

#include <apr_general.h>

#include "celixbool.h"

#include "array_list.h"
#include "array_list_private.h"

apr_pool_t *memory_pool;
array_list_pt list;

int setup(void) {
	apr_initialize();
	apr_pool_create(&memory_pool, NULL);
	arrayList_create(&list);
	if (list == NULL) {
		return 1;
	}
	return 0;
}

void test_arrayList_create(void) {
	CU_ASSERT_PTR_NOT_NULL_FATAL(list);
	CU_ASSERT_EQUAL(list->size, 0);
}

void test_arrayList_trimToSize(void) {
	char * entry;
	arrayList_clear(list);

	entry = "entry";
	arrayList_add(list, entry);
	CU_ASSERT_EQUAL(list->size, 1);
	CU_ASSERT_EQUAL(list->capacity, 10);

	arrayList_trimToSize(list);
	CU_ASSERT_EQUAL(list->size, 1);
	CU_ASSERT_EQUAL(list->capacity, 1);
}

void test_arrayList_ensureCapacity(void) {
	int i;
	arrayList_create(&list);
	arrayList_clear(list);
	CU_ASSERT_EQUAL(list->capacity, 10);
	CU_ASSERT_EQUAL(list->size, 0);
	for (i = 0; i < 100; i++) {
		arrayList_add(list, "entry");
	}
	CU_ASSERT_EQUAL(list->capacity, 133);
	CU_ASSERT_EQUAL(list->size, 100);
	arrayList_create(&list);
}

void test_arrayList_size(void) {
	char * entry;
	char * entry2;
	char * entry3;
	arrayList_clear(list);
	CU_ASSERT_EQUAL(list->size, 0);

	entry = "entry";
	arrayList_add(list, entry);
	CU_ASSERT_EQUAL(list->size, 1);

	entry2 = "entry";
	arrayList_add(list, entry2);
	CU_ASSERT_EQUAL(list->size, 2);

	entry3 = "entry";
	arrayList_add(list, entry3);
	CU_ASSERT_EQUAL(list->size, 3);
}

void test_arrayList_isEmpty(void) {
	arrayList_clear(list);
	CU_ASSERT_EQUAL(list->size, 0);
	CU_ASSERT_TRUE(arrayList_isEmpty(list));
}

void test_arrayList_contains(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = NULL;
	bool contains;

	arrayList_clear(list);

	arrayList_add(list, entry);

	arrayList_add(list, entry2);

	CU_ASSERT_TRUE(arrayList_contains(list, entry));
	CU_ASSERT_TRUE(arrayList_contains(list, entry2));
	contains = arrayList_contains(list, NULL);
	CU_ASSERT_FALSE(contains);

	arrayList_add(list, entry3);

	CU_ASSERT_TRUE(arrayList_contains(list, entry3));
}

void test_arrayList_indexOf(void) {
	char * entry = "entry";
	char * entry2 = "entry2";

	arrayList_clear(list);

	arrayList_add(list, entry);

	arrayList_add(list, entry2);
	arrayList_add(list, entry2);
	arrayList_add(list, entry2);
	arrayList_add(list, entry2);

	CU_ASSERT_EQUAL(arrayList_indexOf(list, entry), 0);
	CU_ASSERT_EQUAL(arrayList_indexOf(list, entry2), 1);
	CU_ASSERT_EQUAL(arrayList_lastIndexOf(list, entry), 0);
	CU_ASSERT_EQUAL(arrayList_lastIndexOf(list, entry2), 4);
}

void test_arrayList_get(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = NULL;
	char * get;
	
	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	get = arrayList_get(list, 0);
	CU_ASSERT_EQUAL(entry, get);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry2, get);

	arrayList_add(list, entry3);

	get = arrayList_get(list, 2);
	CU_ASSERT_PTR_NULL(get);

	get = arrayList_get(list, 42);
	CU_ASSERT_PTR_NULL(get);
}

void test_arrayList_set(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = "entry3";
	char * get;
	char * old;

	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry2, get);

	old = arrayList_set(list, 1, entry3);
	CU_ASSERT_EQUAL(entry2, old);
	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry3, get);
}

void test_arrayList_add(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = "entry3";
	char * get;

	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry2, get);

	arrayList_addIndex(list, 1, entry3);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry3, get);

	get = arrayList_get(list, 2);
	CU_ASSERT_EQUAL(entry2, get);
}

void test_arrayList_addAll(void) {
    char * entry = "entry";
    char * entry2 = "entry2";
    char * entry3 = "entry3"; 
    char * get;
	array_list_pt toAdd;
	bool changed;
	
	arrayList_clear(list);

	arrayList_create(&toAdd);
    arrayList_add(toAdd, entry);
    arrayList_add(toAdd, entry2);

    arrayList_add(list, entry3);

    get = arrayList_get(list, 0);
    CU_ASSERT_EQUAL(entry3, get);

    changed = arrayList_addAll(list, toAdd);
    CU_ASSERT_TRUE(changed);
    CU_ASSERT_EQUAL(arrayList_size(list), 3);

    get = arrayList_get(list, 1);
    CU_ASSERT_EQUAL(entry, get);

    get = arrayList_get(list, 2);
    CU_ASSERT_EQUAL(entry2, get);
}

void test_arrayList_remove(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = "entry3";
	char * get;
	char * removed;

	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry2, get);

	// Remove first entry
	removed = arrayList_remove(list, 0);
	CU_ASSERT_EQUAL(entry, removed);

	// Check the new first element
	get = arrayList_get(list, 0);
	CU_ASSERT_EQUAL(entry2, get);

	// Add a new element
	arrayList_add(list, entry3);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry3, get);
}

void test_arrayList_removeElement(void) {
	char * entry = "entry";
	char * entry2 = "entry2";
	char * entry3 = "entry3";
	char * get;

	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	// Remove entry
	CU_ASSERT_TRUE(arrayList_removeElement(list, entry));

	// Check the new first element
	get = arrayList_get(list, 0);
	CU_ASSERT_EQUAL(entry2, get);

	// Add a new element
	arrayList_add(list, entry3);

	get = arrayList_get(list, 1);
	CU_ASSERT_EQUAL(entry3, get);
}

void test_arrayList_clear(void) {
	char * entry = "entry";
	char * entry2 = "entry2";

	arrayList_clear(list);

	arrayList_add(list, entry);
	arrayList_add(list, entry2);

	CU_ASSERT_EQUAL(arrayList_size(list), 2);
	arrayList_clear(list);
	CU_ASSERT_EQUAL(arrayList_size(list), 0);
}



int main (int argc, char** argv) {
	CU_pSuite pSuite = NULL;

	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
	  return CU_get_error();

	/* add a suite to the registry */
	pSuite = CU_add_suite("Suite_1", setup, NULL);
	if (NULL == pSuite) {
	  CU_cleanup_registry();
	  return CU_get_error();
	}

	/* add the tests to the suite */
	if (NULL == CU_add_test(pSuite, "Array List Creation Test", test_arrayList_create)
		|| NULL == CU_add_test(pSuite, "Array List Trim Test", test_arrayList_trimToSize)
		|| NULL == CU_add_test(pSuite, "Array List Capacity Test", test_arrayList_ensureCapacity)
		|| NULL == CU_add_test(pSuite, "Array List Size Test", test_arrayList_size)
		|| NULL == CU_add_test(pSuite, "Array List Is Empty Test", test_arrayList_isEmpty)
		|| NULL == CU_add_test(pSuite, "Array List Contains Test", test_arrayList_contains)
		|| NULL == CU_add_test(pSuite, "Array List Index Of Test", test_arrayList_indexOf)
		|| NULL == CU_add_test(pSuite, "Array List Get Test", test_arrayList_get)
		|| NULL == CU_add_test(pSuite, "Array List Set Test", test_arrayList_set)
		|| NULL == CU_add_test(pSuite, "Array List Add Test", test_arrayList_add)
		|| NULL == CU_add_test(pSuite, "Array List Remove Test", test_arrayList_remove)
		|| NULL == CU_add_test(pSuite, "Array List Remove Element Test", test_arrayList_removeElement)
		|| NULL == CU_add_test(pSuite, "Array List Clear Test", test_arrayList_clear)
		|| NULL == CU_add_test(pSuite, "Array List Add All", test_arrayList_addAll)
	)
	{
	  CU_cleanup_registry();
	  return CU_get_error();
	}

	CU_set_output_filename(argv[1]);
	CU_list_tests_to_file();
	CU_automated_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}

