/*
Copyright 2013 Google Inc. 2020 LiGuer.
All Rights Reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
=======================================================================*/
#ifndef VOCABULARY_H
#define VOCABULARY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
struct VocabHashTable_word {
	long long frequence;
	char* word;
};
class VocabHashTable {
public:
	typedef float real;								// Precision of float numbers
	typedef long long INT64S;
	typedef unsigned long long INT64U;
	/*--------------------------------[ »ù´¡Á¿ ]--------------------------------*/
	const int vocabHashSize = 30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary
	static const int maxStringLength = 100;
	VocabHashTable_word* vocab;
	int* vocabHash;
	INT64S vocabMemSize = 1000, vocabSize = 0;
	/*--------------------------------[ »ù´¡º¯Êý ]--------------------------------*/
	VocabHashTable() { init(); }
	void init();
	int getNewWordHash(char* word);					//Get New Hash Value
	int addWord(char* word);						//Ìí¼Ó
	int searchWord(char* word);						//¼ìË÷
	int searchWordHash(char* word);					//¼ìË÷ Hash Value
	void save(const char* save_vocab_file);			//´æ´¢
	void readVocab(char* read_vocab_file);			//ÔØÈë´Ê»ã¿â
	void reduceInfrequentWord(int min_reduce);		//Çå³ýµÍÆµ´Ê
	void sort(VocabHashTable_word* begin, VocabHashTable_word* end, bool(*cmd)(VocabHashTable_word a, VocabHashTable_word b));
	void getWordFromText(const char* url);
};
#endif
