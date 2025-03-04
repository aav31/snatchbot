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
	std::vector<std::string> words{ "O" , "A", "N"};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 0);
}

TEST_F(TestSnatchableWordGenerator, SingleLettersSnatchable) {
	std::vector<std::string> words{"P" , "I", "T", "R"};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 4);
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "PIT"), snatchable.end()) << "PIT expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "TIP"), snatchable.end()) << "TIP expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "RIP"), snatchable.end()) << "RIP expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "TRIP"), snatchable.end()) << "TRIP expected to be snatchable";
}

TEST_F(TestSnatchableWordGenerator, MultipleLettersSnatchable) {
	std::vector<std::string> words{ "PET", "RAM" };
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 1);
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "TAMPER"), snatchable.end()) << "TAMPER expected to be snatchable";
}

TEST_F(TestSnatchableWordGenerator, ComplexExample) {
	std::vector<std::string> words{ "PET", "RAM", "E"};
	std::vector<std::string> snatchable = swg.generateSnatchableWords(words);
	EXPECT_EQ(std::size(snatchable), 3);
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "TAMPER"), snatchable.end()) << "TAMPER expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "MARE"), snatchable.end()) << "MARE expected to be snatchable";
	EXPECT_NE(std::find(snatchable.begin(), snatchable.end(), "REAM"), snatchable.end()) << "REAM expected to be snatchable";
}
