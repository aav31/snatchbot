/**
 * @file snatchable_word_generator.h
 * @brief Header file for the SnatchableWordGenerator class.
 * 
 * Contains the declaration of the SnatchableWordGenerator class,
 * which converts the words on the board into snatchable words.
 * 
 * @author Aled Vaghela
 */

#ifndef SNATCHABLE_WORD_GENERATOR_H
#define SNATCHABLE_WORD_GENERATOR_H
#include <vector>
#include <string>
#include <fstream>

class SnatchableWordGenerator {
public:
	static SnatchableWordGenerator& getInstance() {
		static SnatchableWordGenerator instance;
		return instance;
	}

	SnatchableWordGenerator(const SnatchableWordGenerator&) = delete;
	SnatchableWordGenerator& operator=(const SnatchableWordGenerator&) = delete;

private:
	/**
	 * @brief Private constructor to prevent instantiation.
	 * 
	 * @param dictionaryPath Path to the dictionary file.
	 */
    SnatchableWordGenerator(const char* dictionaryPath = "collins_scrabble_words_2019.txt") {
        std::ifstream infile(dictionaryPath);
		std::string word;
		if (infile.is_open()) {
			while (std::getline(infile, word)) {
				dictionary.insert(word);
			}
			infile.close();
		}
		else {
			throw std::runtime_error("Cannot open dictionary file.");
		}
    }
    std::unordered_set<std::string> dictionary;
};

#endif
