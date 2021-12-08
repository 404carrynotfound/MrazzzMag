#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../priority_queue.h"

TEST_SUITE("Priority_queue")
{
	TEST_CASE("Priority queue is empty")
	{
		priority_queue<int> pq;

		CHECK(pq.size() == 0);
		CHECK(pq.empty());
	}

	TEST_CASE("Push")
	{
		priority_queue<int> pq;
		pq.push(1, 1);
			
		CHECK(pq.size() == 1);
		CHECK(!pq.empty());
	}

	TEST_CASE("Pop")
	{
		priority_queue<int> pq;
		pq.push(1, 1);
		pq.push(20, 12);

		CHECK(pq.size() == 2);
		CHECK(!pq.empty());

		SUBCASE("one item")
		{
			pq.pop();

			CHECK(pq.size() == 1);
			CHECK(!pq.empty());
		}

		SUBCASE("empty queue")
		{
			pq.pop();
			pq.pop();
			
			CHECK(pq.size() == 0);
			CHECK(pq.empty());
		}
	}

	TEST_CASE("Accessors")
	{
		priority_queue<int> pq;
		pq.push(1, 1);
		pq.push(2, 2);
		pq.push(20, -2);

		REQUIRE(pq.size() == 3);

		SUBCASE("Front")
		{
			CHECK(pq.front() == 20);
		}
		
		SUBCASE("Back")
		{
			CHECK(pq.back() == 2);
		}
		
		SUBCASE("Top")
		{
			CHECK(pq.top() == 20);
		}
	}	
}