#include "test.h"


int main() {

	DynArray<int> arr_1 = DynArray<int>(10);
	arr_1.size();
	arr_1.capacity();


	DynArray<int> arr_2(std::move(arr_1));
	arr_1.size();
	arr_1.capacity();
	arr_2.size();
	arr_2.capacity();

	arr_2 = std::move(arr_1);
	arr_1.size();
	arr_1.capacity();
	arr_2.size();
	arr_2.capacity();

 }
