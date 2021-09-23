#include "pch.h"
#include "CppUnitTest.h"
#include "../EE308_LAB2/CounterKeys2.0_final.cpp"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		int arr_size = sizeof(keywords) / sizeof(keywords[0]);
		TEST_METHOD(TestMethod1)
		{
				int arr_size = sizeof(keywords) / sizeof(keywords[0]);
				string text = "if{}else if{if{}else if{}}else{}";
				Count count;
				count.GetReady(keywords, arr_size, 4);
				count.countWork(text,4);
				Assert::AreEqual(count.getOutput()->if_else_num, 0);
				Assert::AreEqual(count.getOutput()->if_elseif_else_num, 1);
			
		}
		TEST_METHOD(TestMethod2)
		{
			int arr_size = sizeof(keywords) / sizeof(keywords[0]);
			string text = "if{if  {}else if {} else{}}else {}";
			Count count;
			count.GetReady(keywords, arr_size, 4);
			count.countWork(text, 4);
			Assert::AreEqual(count.getOutput()->if_else_num, 1);
			Assert::AreEqual(count.getOutput()->if_elseif_else_num, 1);

		}
		TEST_METHOD(TestSwitchCase) {
			string text = "switch{case 1: break; case 2: break;}switch{case 1: break;}";
			Count count;
			count.GetReady(keywords, arr_size, 4);
			count.countWork(text, 4);
			Assert::AreEqual(count.getOutput()->switch_num, 2);
			vector<int> vec = count.getOutput()->case_arr;
			Assert::AreEqual(vec[0], 2);
			Assert::AreEqual(vec[1], 1);
		}
	};
}
