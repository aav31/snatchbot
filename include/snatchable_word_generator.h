/**
 * @file snatchable_word_generator.h
 * @brief Header file for the SnatchableWordGenerator class.
 * 
 * Contains the declaration of the SnatchableWordGenerator class,
 * which is a Singleton class for converting the words into 
 * snatchable words.
 * 
 * @author Aled Vaghela
 */

#ifndef SNATCHABLE_WORD_GENERATOR_H
#define SNATCHABLE_WORD_GENERATOR_H
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

/**
 * @class SnatchableWordGenerator
 * @brief Singleton class for converting the words into snatchable words.
 *
 * This class follows the Singleton pattern to ensure only one instance exists.
 * It processes a vector of words and returns a list of snatchable words.
 */
class SnatchableWordGenerator {
public:
	static SnatchableWordGenerator& getInstance() {
		static SnatchableWordGenerator instance;
		return instance;
	}

	SnatchableWordGenerator(const SnatchableWordGenerator&) = delete;
	SnatchableWordGenerator& operator=(const SnatchableWordGenerator&) = delete;

	/*
	 * @brief Generates a list of snatchable words from the words currently on the board.
	 * 
	 * Snatchable words are formed from at least two other words on the board.
	 * Each word is a group of >= 1 letters. Valid snatchable words must be at least three
	 * letters in length .This is guaranteed since sortedStringToAnagrams has been made to
	 * only contain strings which are greater than three characters in length.
	 * 
	 * @param words List of words currently on the board.
	 * @return A list of snatchable words.
	 */
	std::vector<std::string> generateSnatchableWords(const std::vector<std::string>& words) {
		std::vector<std::string> snatchableWords{};
		std::vector<std::vector<std::string>> powerSetWords{ generateSubsets(std::size(words)-1, words)};
		// Snatchable words are formed from at least two other words on the board
		std::erase_if(powerSetWords, [](const std::vector<std::string>& subset) { return subset.size() <= 1; });

		for (const std::vector<std::string>& subset: powerSetWords) {
			// Join all strings in the subset into one sorted string
			std::string combinedString;
			std::for_each(subset.begin(), subset.end(), [&combinedString](const std::string& word) { combinedString += word; });
			std::sort(combinedString.begin(), combinedString.end());

			if (sortedStringToAnagrams.contains(combinedString)) {
				std::vector<std::string> anagrams{ sortedStringToAnagrams.at(combinedString) };
				std::for_each(anagrams.begin(), anagrams.end(), [&snatchableWords](std::string a) { snatchableWords.push_back(a); });
			}

		}
		return snatchableWords;
	}

private:
	/**
	 * @brief Private constructor to prevent instantiation.
	 * 
	 * @param dictionaryPath Path to the dictionary file.
	 * @throw std::runtime_error If cannot initialize.
	 */
    SnatchableWordGenerator(const char* dictionaryPath = "words_popular.txt") {
        std::ifstream infile(dictionaryPath);
		std::string word;
		if (infile.is_open()) {
			while (std::getline(infile, word)) {
				// snatchable words must be at least length three
				if (std::size(word) < 3) { continue; }
				std::transform(word.begin(), word.end(), word.begin(), [](char c) { return std::toupper(c); });
				std::string sortedWord = word;
				std::sort(sortedWord.begin(), sortedWord.end());
				sortedStringToAnagrams[sortedWord].push_back(word);
			}
			infile.close();
		}
		else {
			throw std::runtime_error("Cannot open dictionary file.");
		}
    }

    std::unordered_map<std::string, std::vector<std::string>> sortedStringToAnagrams;

	/**
	 * @brief Generate all subsets of words up to and including the ith word.
	 * 
	 * @param i An index corresponding to the words array to generate subsets for.
	 * @return All subsets of the words array up to and including the ith index.
	 */
	std::vector<std::vector<std::string>> generateSubsets(int i, const std::vector<std::string>& words) {
		std::vector<std::vector<std::string>> result{};
		if (i < 0) {
			return result;
		}
		else if (i == 0) {
			result.push_back({});
			result.push_back({words[0]});
			return result;
		}
		else {
			std::vector<std::vector<std::string>> subproblem = generateSubsets(i-1, words);
			for (const std::vector<std::string>& subset : subproblem) {
				result.push_back(subset);
				std::vector<std::string> newSubset(subset.begin(), subset.end());
				newSubset.push_back(words[i]);
				result.push_back(newSubset);
			}
			return result;
		}
	}
};

#endif
