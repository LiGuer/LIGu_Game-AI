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
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <windows.h>
#else
#include <pthread.h>
#endif
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "VocabHashTable.h"
#include "D:/wamp64/www/LiGu/code/LiGu_AlgorithmLib/BasicAlgorithm.h"

class Word2Vec {
public:
	typedef float real;                    // Precision of float numbers
	typedef long long INT64S;
	typedef unsigned long long INT64U;
	/*--------------------------------[ 基础量 ]--------------------------------*/
	const char* trainFile;
	const static int MAX_SENTENCE_LENGTH = 1000;
	const int MAX_EXP = 6, EXP_TABLE_SIZE = 1000, UnigramTable_size = 1e8;
	INT64S cbow = 1, window = 5, negative = 5,layer1_size = 100, train_words = 0, word_count_actual = 0, iter = 5, threadNum = 12;
	real starting_learnRate = 0.025, sample = 1e-3;
	real* syn0, * expTable, * syn1neg;
	VocabHashTable vocab;
	int* codelen, *UnigramTable;
	unsigned long long* codeHuff;
	/*--------------------------------[ 初始化 ]--------------------------------
	*	[流程]:
			[1] 载入词汇库
			[2] Huffman编码
			[3] 初始化神经网络
			[4] exp打表
	--------------------------------------------------------------------------*/
	void init() {
		// [1] 
		vocab.addWord((char*)"</s>");
		vocab.vocab[vocab.searchWord((char*)"</s>")].frequence = 6;
		vocab.getWordFromText(trainFile);
		vocab.reduceInfrequentWord(5);
		vocab.sort(&vocab.vocab[1], &vocab.vocab[vocab.vocabSize], [](VocabHashTable_word a, VocabHashTable_word b) {
			return a.frequence > b.frequence;
		});	// Sort the vocabulary and keep </s> at the first position
		for (int i = 0; i < vocab.vocabSize; i++)train_words += vocab.vocab[i].frequence;
		// [2]
		int* weight = (int*)calloc(vocab.vocabSize, sizeof(int));
		codelen = (int*)calloc(vocab.vocabSize, sizeof(int)), codeHuff = (unsigned long long*)calloc(vocab.vocabSize, sizeof(unsigned long long));
		for (int i = 0; i < vocab.vocabSize; i++)weight[i] = vocab.vocab[i].frequence;
		HuffmanCode<int>(weight, vocab.vocabSize, codelen, codeHuff);
		// [3]
		initNet();
		InitUnigramTable();
		initExpTable(MAX_EXP, EXP_TABLE_SIZE);	// [4]
	}
	void initExpTable(int MAX_EXP, int EXP_TABLE_SIZE) {
		expTable = (real*)malloc((EXP_TABLE_SIZE + 1) * sizeof(real));
		for (int i = 0; i <= EXP_TABLE_SIZE; i++)
			expTable[i] = exp((i / (real)EXP_TABLE_SIZE * 2 - 1) * MAX_EXP) / (expTable[i] + 1);	// Precompute the exp(6·(2x-1)) UnigramTable // Precompute f(x) = x / (x + 1)
	}
	/*--------------------------------[ 初始化神经网络 ]--------------------------------*/
	void initNet() {
#ifdef _MSC_VER
		syn0 = (real*)_aligned_malloc((INT64S)vocab.vocabSize * layer1_size * sizeof(real), 128);
#elif defined  linux
		int a = posix_memalign((void**)&syn0, 128, (INT64S)vocab.vocabSize * layer1_size * sizeof(real));
#endif
		if (syn0 == NULL) exit(1);
		INT64U next_random = 1;
		for (INT64S i = 0; i < vocab.vocabSize * layer1_size; i++) {
			next_random *= (INT64U)25214903917 + 11;
			syn0[i] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / layer1_size;	// 随机初始化
		}
		// negative
#ifdef _MSC_VER
		syn1neg = (real*)_aligned_malloc((INT64S)vocab.vocabSize * layer1_size * sizeof(real), 128);
#elif defined  linux
		a = posix_memalign((void**)&syn1neg, 128, (INT64S)vocab_size * layer1_size * sizeof(real));
#endif
		if (syn1neg == NULL) exit(1);
		for (INT64S i = 0; i < vocab.vocabSize * layer1_size; i++)syn1neg[i] = 0;
	}
	/*--------------------------------[ 读词 ]--------------------------------*/
	char* readWord(FILE* fin) {
		int cur = 0, MAX_STRING = 100;
		char* word = (char*)calloc(MAX_STRING, sizeof(char));
		while (!feof(fin)) {
			char ch = fgetc(fin);
			if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
			if (ch >= 'a' && ch <= 'z') {
				word[cur++] = ch;
				if (cur >= MAX_STRING - 1) cur--;   // Truncate too long words
			}
			else if (cur > 0)break;
		}
		word[cur++] = '\0';
		return word;
	}
	/*--------------------------------[ 训练神经网络 ]--------------------------------
	---------------------------------------------------------------------------------*/
	void TrainModelThread(void* id) {
		//[1] 训练数据文件
		FILE* fi = fopen(trainFile, "rb");
		fseek(fi, 0, SEEK_END);
		INT64S file_size = ftell(fi);
		rewind(fi);
		fseek(fi, file_size / (INT64S)threadNum * (INT64S)id, SEEK_SET);
		//基础变量
		INT64S local_iter = iter, word_count = 0, last_word_count = 0;
		int sentence_position = 0, sentence_length = 0, sen[MAX_SENTENCE_LENGTH + 1];
		INT64U next_random = (INT64S)id;
		real learnRate = starting_learnRate;
		real* neu1 = (real*)calloc(layer1_size, sizeof(real));
		real* neu1e = (real*)calloc(layer1_size, sizeof(real));
		clock_t start = clock();
		// Start TrainModel
		while (true) {
			if (word_count - last_word_count > 10000) {
				word_count_actual += word_count - last_word_count;
				last_word_count = word_count;

				clock_t now = clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, learnRate,
					word_count_actual / (real)(iter * train_words + 1) * 100,
					word_count_actual / ((real)(now - start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);

				learnRate = starting_learnRate * (1 - word_count_actual / (real)(iter * train_words + 1));
				if (learnRate < starting_learnRate * 0.0001) learnRate = starting_learnRate * 0.0001;
			}
			// get sentence
			if (sentence_length == 0) {
				while (sentence_length < MAX_SENTENCE_LENGTH) {
					int word = vocab.searchWord(readWord(fi));
					if (feof(fi)) break;
					else if (word == -1) continue;
					word_count++;
					if (word == 0) break;
					// The subsampling randomly discards frequent words while keeping the ranking same
					if (sample > 0) {
						real ran = (sqrt(vocab.vocab[word].frequence / (sample * train_words)) + 1) * (sample * train_words) / vocab.vocab[word].frequence;
						next_random *= (INT64U)25214903917 + 11;
						if (ran < (next_random & 0xFFFF) / (real)65536) continue;
					}
					sen[sentence_length++] = word;
				}sentence_position = 0;
			}
			if (feof(fi) || (word_count > train_words / threadNum)) {
				word_count_actual += word_count - last_word_count;
				if (--local_iter == 0) break;
				word_count = last_word_count = sentence_length = 0;
				fseek(fi, file_size / (INT64S)threadNum * (INT64S)id, SEEK_SET);
				continue;
			}
			int word = sen[sentence_position];
			if (word == -1) continue;
			//[] cbow or skip-gram
			memset(neu1, 0, sizeof(real) * layer1_size);
			memset(neu1e, 0, sizeof(real) * layer1_size);
			next_random *= (INT64U)25214903917 + 11;
			int b = next_random % window;	
			if (cbow) {  //train the cbow architecture
				// in -> hidden
				INT64S cw = 0;
				for (int i = b; i < window * 2 + 1 - b; i++) {
					int c = sentence_position - window + i;
					if (i == window || c < 0 || c >= sentence_length) continue;
					int last_word = sen[c];
					if (last_word == -1) continue;
					for (int j = 0; j < layer1_size; j++) neu1[j] += syn0[j + last_word * layer1_size];
					cw++;
				}
				if (cw) {
					for (int i = 0; i < layer1_size; i++) neu1[i] /= cw;
					// NEGATIVE SAMPLING
					INT64S target, label;
					for (int i = 0; i < negative + 1; i++) {
						if (i == 0) { target = word; label = 1; }
						else {
							next_random = next_random * (INT64U)25214903917 + 11;
							target = UnigramTable[(next_random >> 16) % UnigramTable_size];
							if (target == 0) target = next_random % (vocab.vocabSize - 1) + 1;
							if (target == word) continue;
							label = 0;
						}
						INT64S l2 = target * layer1_size;
						real f = 0, g = 0;
						for (int i = 0; i < layer1_size; i++) f += neu1[i] * syn1neg[i + l2];

						if (f > MAX_EXP) g = (label - 1) * learnRate;
						else if (f < -MAX_EXP) g = (label - 0) * learnRate;
						else g = (label - expTable[(int)((f + MAX_EXP) * (EXP_TABLE_SIZE / MAX_EXP / 2))]) * learnRate;
					
						for (int i = 0; i < layer1_size; i++) neu1e[i] += g * syn1neg[i + l2];
						for (int i = 0; i < layer1_size; i++) syn1neg[i + l2] += g * neu1[i];
					}
					// hidden -> in
					for (int i = b; i < window * 2 + 1 - b; i++){
						int c = sentence_position - window + i;
						if (i == window || c < 0 || c >= sentence_length) continue;
						int last_word = sen[c];
						if (last_word == -1) continue;
						for (int j = 0; j < layer1_size; j++) syn0[j + last_word * layer1_size] += neu1e[j];
					}
				}
			}
			else {  //Train skip-gram
				for (int i = b; i < window * 2 + 1 - b; i++) {
					int c = sentence_position - window + i;
					if (i == window || c < 0 || c >= sentence_length) continue;
					int last_word = sen[c];
					if (last_word == -1) continue;
					memset(neu1e, 0, sizeof(real) * layer1_size);
					// Learn weights input -> hidden
					for (int j = 0; j < layer1_size; j++) syn0[j + last_word * layer1_size] += neu1e[c];
				}
			}
			if (++sentence_position >= sentence_length) sentence_length = 0;
		}fclose(fi); free(neu1); free(neu1e);
#ifdef _MSC_VER
		_endthreadex(0);
#elif defined  linux 
		pthread_exit(NULL);
#endif
	}
	/*--------------------------------[ saveModel ]--------------------------------*/
	void saveModel(const char* outputFile) {
		FILE* fo = fopen(outputFile, "wb");
		// Save the word vectors
		fprintf(fo, "%lld %lld", vocab.vocabSize, layer1_size);
		for (int i = 0; i < vocab.vocabSize; i++) {
			fprintf(fo, "\n%s ", vocab.vocab[i].word);
			for (int j = 0; j < layer1_size; j++) fwrite(&syn0[i * layer1_size + j], sizeof(real), 1, fo);
		}fclose(fo);
	}
	/*--------------------------------[ 初始化UnigramTable ]--------------------------------*/
	void InitUnigramTable() {
		double train_words_pow = 0, power = 0.75;
		UnigramTable = (int*)malloc(UnigramTable_size * sizeof(int));
		for (int i = 0; i < vocab.vocabSize; i++) train_words_pow += pow(vocab.vocab[i].frequence, power);
		int a = 0;
		double d1 = pow(vocab.vocab[a].frequence, power) / train_words_pow;
		for (int i = 0; i < UnigramTable_size; i++) {
			UnigramTable[i] = a;
			if (i / (double)UnigramTable_size > d1)
				d1 += pow(vocab.vocab[++a].frequence, power) / train_words_pow;
			if (a >= vocab.vocabSize) a = vocab.vocabSize - 1;
		}
	}
};