/*

INTEGRANTES
    - Meza Gamboa Fernando Gustavo  18200163
    - Romero Ovalle Antony Samuel   20200207 

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define MAX_WORDS 1000
#define MAX_DOCS 3
#define MAX_THREADS 3

typedef struct {
    char word[50];
    int count;
} Term;

typedef struct {
    char document[50];
    Term terms[MAX_WORDS];
    int numTerms;
} Document;

void readDocument(char *filename, Document *doc) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    strcpy(doc->document, filename);
    doc->numTerms = 0;

    char word[50];
    while (fscanf(file, "%s", word) != EOF) {
        int i;
        for (i = 0; i < doc->numTerms; i++) {
            if (strcmp(doc->terms[i].word, word) == 0) {
                doc->terms[i].count++;
                break;
            }
        }
        if (i == doc->numTerms) {
            strcpy(doc->terms[i].word, word);
            doc->terms[i].count = 1;
            doc->numTerms++;
        }
    }

    fclose(file);
}

void *calculateTFIDFParallel(void *data) {
    ThreadData *tData = (ThreadData *)data;
    Document *docs = tData->docs;
    int numDocs = tData->numDocs;
    int threadID = tData->threadID;

    int docsPerThread = numDocs / MAX_THREADS;
    int start = threadID * docsPerThread;
    int end = (threadID == MAX_THREADS - 1) ? numDocs : (start + docsPerThread);

    for (int i = start; i < end; i++) {
        printf("Documento %d: %s\n", i + 1, docs[i].document);
        printf("\n");

        for (int j = 0; j < docs[i].numTerms; j++) {
            int df = 0;
            int k;

            for (k = 0; k < numDocs; k++) {
                int l;
                for (l = 0; l < docs[k].numTerms; l++) {
                    if (strcmp(docs[i].terms[j].word, docs[k].terms[l].word) == 0) {
                        df++;
                        break;
                    }
                }
            }

            double tf = (double)docs[i].terms[j].count / docs[i].numTerms;
            double idf = log((double)numDocs / df);
            double tfidf = tf * idf;

            printf("Término: %s\n", docs[i].terms[j].word);
            printf("TF: %lf\n", tf);
            printf("IDF: %lf\n", idf);
            printf("TF-IDF: %lf\n", tfidf);
            printf("\n");
        }
    }
    pthread_exit(NULL);
}

void calculateTFIDFParallelWrapper(Document *docs, int numDocs) {
    pthread_t threads[MAX_THREADS];
    ThreadData threadData[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++) {
        threadData[i].docs = docs;
        threadData[i].numDocs = numDocs;
        threadData[i].threadID = i;

        pthread_create(&threads[i], NULL, calculateTFIDFParallel, (void *)&threadData[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
	Document docs[MAX_DOCS];
    int numDocs = MAX_DOCS;
    int i;

    char filenames[MAX_DOCS][50] = {
        "sample1.txt",
        "sample2.txt",
        "sample3.txt"
    };

    clock_t start, end;
    double cpu_time_used;

    // Ejecución secuencial
    start = clock();
    for (i = 0; i < numDocs; i++) {
        readDocument(filenames[i], &docs[i]);
    }
    calculateTFIDF(docs, numDocs);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución (secuencial): %f segundos\n", cpu_time_used);


	//Ejecucion parelela
	print("\nEJECUCION PARELELA")

    start = clock();
    for (i = 0; i < numDocs; i++) {
        readDocument(filenames[i], &docs[i]);
    }
    calculateTFIDFParallelWrapper(docs, numDocs);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución (paralelo): %f segundos\n", cpu_time_used);
    

    return 0;

}

