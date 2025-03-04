#include <gtest/gtest.h>
#include "snatchable_word_generator.h"

class TestSnatchableWordGenerator : public ::testing::Test {
protected:
	SnatchableWordGenerator& swg{ SnatchableWordGenerator::getInstance() };
};

TEST_F(TestSnatchableWordGenerator, NoWords) {
	std::vector<std::string> words{};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 0);
}

TEST_F(TestSnatchableWordGenerator, SingleLettersNoSnatchable) {
	std::vector<std::string> words{ "O" , "A"  "N"};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 0);
}

TEST_F(TestSnatchableWordGenerator, SingleLettersSnatchable1) {
	std::vector<std::string> words{ "P" , "I"  "T"};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	std::for_each(snatchable.begin(), snatchable.end(), [](std::string x) {std::cout << x << " "; });
	std::cout << std::endl;
	EXPECT_EQ(std::size(snatchable), 2);
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "PIT"), snatchable.end()) << "PIT expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "TIP"), snatchable.end()) << "TIP expected to be snatchable";
}
