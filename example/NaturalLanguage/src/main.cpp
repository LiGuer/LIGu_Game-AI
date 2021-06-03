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
#include "VocabHashTable.h"
#include "D:/wamp64/www/LiGu/code/LiGu_AlgorithmLib/BasicAlgorithm.h"
#include "Word2Vec.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

Word2Vec w2v;
/*--------------------------------[ TrainModel ]--------------------------------*/
#ifdef _MSC_VER
DWORD WINAPI TrainModelThread_win(LPVOID tid) { w2v.TrainModelThread(tid);	return 0; }
#endif
void TrainModel(int threadNum) {
#ifdef _MSC_VER
	HANDLE* pt = (HANDLE*)malloc(threadNum * sizeof(HANDLE));
	for (int i = 0; i < threadNum; i++)
		pt[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)TrainModelThread_win, (void*)i, 0, NULL);
	WaitForMultipleObjects(threadNum, pt, TRUE, INFINITE);
	for (int i = 0; i < threadNum; i++) CloseHandle(pt[i]);
	free(pt);
#elif defined  linux 
	pthread_t* pt = (pthread_t*)malloc(threadNum * sizeof(pthread_t));
	for (int a = 0; a < threadNum; a++) pthread_create(&pt[a], NULL, TrainModelThread, (void*)a);
	for (int a = 0; a < threadNum; a++) pthread_join(pt[a], NULL);
#endif
}

int main() {
	w2v.trainFile = "C:/Users/ÐÜè÷ºÀ/Desktop/data.txt";
	w2v.init();

	printf("Start TrainModel\n");
	TrainModel(w2v.threadNum);
	w2v.saveModel("D:/Vec.txt");
}