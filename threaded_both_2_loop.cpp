#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/stdc++.h>
#include "libarff/arff_parser.h"
#include "libarff/arff_data.h"
#include <algorithm>
#include <threads.h>

// to track memory used
#include <sys/resource.h>
void print_memory_usage() {
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    printf("Memory usage: %ld KB\n", r_usage.ru_maxrss);
}

// std::ofstream outFile("threaded_check1.txt");

const float eps = 1e-7;

using namespace std;

struct ThreadArgs {
    ArffData *train;
    ArffData *test;
    int num_classes;
    int k;
    int start;
    int end;
    int *predictions;
    int num_threads;
};

struct ThreadKeyArgs {
    ArffData *train;
    ArffData *test;
    int num_classes;
    int k;
    int start;
    int end;
    float *candidates;
    int num_threads;
    int queryIndex;
};

// Calculates the distance between two instances
float distance(float* instance_A, float* instance_B, int num_attributes) {
    float sum = 0;
    
    for (int i = 0; i < num_attributes-1; i++) {
        float diff = instance_A[i] - instance_B[i];
        sum += diff*diff;
    }
    
    return sqrt(sum);
}

// ================== start new logic =================================
void *KnnRunKeyThread(void * arguments){
    struct ThreadKeyArgs *args = (struct ThreadKeyArgs *) arguments;
    // int k = args->k;
    // int num_attributes = args->train->num_attributes();

    // Pointers representing the dataset as a 2D matrix
    float* train_matrix = args->train->get_dataset_matrix();
    float* test_matrix = args->test->get_dataset_matrix();
    //The static keyword here means that this mutex is shared 
    // among all instances of the function.

    for (int keyIndex = args->start; keyIndex < args->end; keyIndex++) {
        float dist = distance(&test_matrix[args->queryIndex * args->train->num_attributes()], &train_matrix[keyIndex*args->train->num_attributes()], args->train->num_attributes());
        // Add to our candidates
        // critical session
        // protect_each_key_m.lock();
        bool updated = false;
        for(int c = 0; c < args->k; c++){
            if(dist < args->candidates[3*c] || dist - args->candidates[3*c] < eps && args->candidates[3*c + 2] > keyIndex) {
                // Found a new candidate
                // Shift previous candidates down by one
                for(int x = args->k-2; x >= c; x--) {
                    args->candidates[3*x+3] = args->candidates[3*x];
                    args->candidates[3*x+4] = args->candidates[3*x+1];
                    args->candidates[3*x+5] = args->candidates[3*x+2];
                }
                
                // Set key vector as potential k NN
                args->candidates[3*c] = dist;
                args->candidates[3*c+1] = train_matrix[keyIndex*args->train->num_attributes() + args->train->num_attributes() - 1]; // class value
                args->candidates[3*c+2] = keyIndex; // id in the train
                updated = true;

                // cout << "Query Index: " << args->queryIndex << " Key index: " << keyIndex << "\n";
                // for (int i = 0; i < 3*k; ++i){
                //     if (i % 3 == 2) continue;
                //     cout << args->candidates[i] << " ";
                // }
                // cout << "\n";

                break;
            }
        }

        // protect_each_key_m.unlock();
        // critical session
        if (updated){
            // This helps reduce contention on the mutex
            continue;
        }
    }
        
    pthread_exit(0);
}

// pthread_t key_threads[MAX_THREADS];
// struct ThreadKeyArgs key_thread_args[MAX_THREADS];
// New Logic
void *KnnRunQueryThread(void *arguments) {
    struct ThreadArgs *args = (struct ThreadArgs *) arguments;
    int k = args->k;
    int num_attributes = args->train->num_attributes();

    // Pointers representing the dataset as a 2D matrix
    float* train_matrix = args->train->get_dataset_matrix();
    float* test_matrix = args->test->get_dataset_matrix();
    int num_threads_used = args->num_threads;
    int n = args->train->num_instances();
    int num_classes = args->train->num_classes();

    pthread_t* key_threads = (pthread_t *) malloc(num_threads_used * sizeof(pthread_t));
    struct ThreadKeyArgs* key_thread_args = (struct ThreadKeyArgs *) malloc(num_threads_used * sizeof(struct ThreadKeyArgs));

    float* candidates = (float*) calloc(k*3, sizeof(float));
    int* classCounts = (int*)calloc(num_classes, sizeof(int));
    for (int queryIndex = args->start; queryIndex < args->end; queryIndex++) {
        // print_memory_usage();
        // stores k-NN candidates for a query vector as a sorted 2d array. First element is inner product, second is class, third is the index of the element.
        for(int i = 0; i < 3*k; i++){ candidates[i] = FLT_MAX; }
        memset(classCounts, 0, num_classes * sizeof(int));

        // stores k-NN candidates for a query vector as a sorted 2d array. First element is distance, second is class.
        for (int i = 0; i < num_threads_used; i++) {
            int start = i * n / num_threads_used;
            int end = (i + 1) * n / num_threads_used;
            // the last thread might have less than n/num_threads_used queries
            if (i == num_threads_used - 1)
                end = n;
            
            key_thread_args[i].train = args->train;
            key_thread_args[i].test = args->test;
            key_thread_args[i].num_classes = args->num_classes;
            key_thread_args[i].k = k;
            key_thread_args[i].start = start;
            key_thread_args[i].end = end;
            // key_thread_args[i].predictions = predictions;
            key_thread_args[i].candidates = candidates;
            key_thread_args[i].num_threads = num_threads_used;
            key_thread_args[i].queryIndex = queryIndex;
            // cout << "Check before go to key function:\n";
            // print_memory_usage();
            pthread_create(&key_threads[i], NULL, KnnRunKeyThread, (void *) &key_thread_args[i]);
        }

        for (int i = 0; i < num_threads_used; i++) {
            pthread_join(key_threads[i], NULL);
        }

        for(int i = 0; i < k; i++) {
            classCounts[(int)candidates[3*i+1]] += 1;
        }

        // outFile << "Query Index: " << queryIndex << "\n";
        // for (int i = 0; i < 3*k; ++i){
        //     outFile << candidates[i] << " ";
        // }
        // outFile << "\n";

        int max_value = -1;
        int max_class = 0;
        for(int i = 0; i < num_classes; i++) {
            if(classCounts[i] > max_value) {
                max_value = classCounts[i];
                max_class = i;
            }
        }

        // cout << max_class << "\n";
        
        // Make prediction with 
        args->predictions[queryIndex] = max_class;
        // reset candidate + class count
        for(int i = 0; i < 3*k; i++){ candidates[i] = FLT_MAX; }
        memset(classCounts, 0, num_classes * sizeof(int));
    }
    free(candidates);
    free(classCounts);

    free(key_threads);
    free(key_thread_args);

    pthread_exit(0);
}
// ================== end new logic =================================

// ================== start old logic =================================
// void *KnnRunQueryThread(void *arguments) {
//     struct ThreadArgs *args = (struct ThreadArgs *) arguments;
//     int k = args->k;
//     int num_attributes = args->train->num_attributes();
//     int train_num_instances = args->train->num_instances();

//     // Pointers representing the dataset as a 2D matrix
//     float* train_matrix = args->train->get_dataset_matrix();
//     float* test_matrix = args->test->get_dataset_matrix();

//     for (int queryIndex = args->start; queryIndex < args->end; queryIndex++) {
//         // stores k-NN candidates for a query vector as a sorted 2d array. First element is distance, second is class.
//         float *candidates = (float *) calloc(k * 2, sizeof(float));
//         for (int i = 0; i < 2 * k; i++) { candidates[i] = FLT_MAX; }

//         // Stores bincounts of each class over the final set of candidate NN
//         int *classCounts = (int *) calloc(args->num_classes, sizeof(int));

//         for (int keyIndex = 0; keyIndex < train_num_instances; keyIndex++) {
//             float dist = distance(&test_matrix[queryIndex*num_attributes], &train_matrix[keyIndex*num_attributes], num_attributes);

//             // Add to our candidates
//             for (int c = 0; c < k; c++) {
//                 if (dist < candidates[2 * c]) {
//                     // Found a new candidate
//                     // Shift previous candidates down by one
//                     for (int x = k - 2; x >= c; x--) {
//                         candidates[2 * x + 2] = candidates[2 * x];
//                         candidates[2 * x + 3] = candidates[2 * x + 1];
//                     }

//                     // Set key vector as potential k NN
//                     candidates[2 * c] = dist;
//                     candidates[2 * c + 1] = train_matrix[keyIndex*num_attributes + num_attributes - 1]; // class value

//                     break;
//                 }
//             }
//         }

//         // Bincount the candidate labels and pick the most common
//         for (int i = 0; i < k; i++) {
//             classCounts[(int) candidates[2 * i + 1]] += 1;
//         }

//         int max_value = -1;
//         int max_class = 0;
//         for (int i = 0; i < args->num_classes; i++) {
//             if (classCounts[i] > max_value) {
//                 max_value = classCounts[i];
//                 max_class = i;
//             }
//         }

//         args->predictions[queryIndex] = max_class;

//         free(candidates);
//         free(classCounts);
//     }

//     pthread_exit(0);
// }
// ================== end old logic =================================
// pthread_t threads[MAX_THREADS];
// struct ThreadArgs thread_args[MAX_THREADS];
int *thread_POSIX_KNN(ArffData *train, ArffData *test, int k, int num_threads) {
    int num_threads_used = 1;               // use num_threads_used for query
    int num_threads_left = num_threads;     // use num_threads_left for key, the sum will be num_threads_left * num_threads_used
    for (int i = 2; i < sqrt(num_threads); ++i){
        if (num_threads % i == 0){
            num_threads_used = i;
            num_threads_left = num_threads / i;
        }
    }
    int *predictions = (int *) malloc(test->num_instances() * sizeof(int));
    int num_classes = train->num_classes();

    int n = test->num_instances();
    pthread_t* threads = (pthread_t *) malloc(num_threads_used * sizeof(pthread_t));
    struct ThreadArgs* thread_args = (struct ThreadArgs *) malloc(num_threads_used * sizeof(struct ThreadArgs));

    // Split query into num_threads_used threads, 
    // KnnRunQueryThread(i) will handle n/num_threads_used queries
    // Except for the last thread
    for (int i = 0; i < num_threads_used; i++) {
        int start = i * n / num_threads_used;
        int end = (i + 1) * n / num_threads_used;
        // the last thread might have less than n/num_threads_used queries
        if (i == num_threads_used - 1)
            end = n;
        
        thread_args[i].train = train;
        thread_args[i].test = test;
        thread_args[i].num_classes = num_classes;
        thread_args[i].k = k;
        thread_args[i].start = start;
        thread_args[i].end = end;
        thread_args[i].predictions = predictions;
        thread_args[i].num_threads = num_threads_left;

        // cout << "Check memory at the first:\n";
        // print_memory_usage();

        pthread_create(&threads[i], NULL, KnnRunQueryThread, (void *) &thread_args[i]);
    }

    for (int i = 0; i < num_threads_used; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(thread_args);

    return predictions;
}

int* computeConfusionMatrix(int* predictions, ArffData* dataset)
{
    int* confusionMatrix = (int*)calloc(dataset->num_classes() * dataset->num_classes(), sizeof(int)); // matrix size numberClasses x numberClasses
    
    for(int i = 0; i < dataset->num_instances(); i++) { // for each instance compare the true class and predicted class    
        int trueClass = dataset->get_instance(i)->get(dataset->num_attributes() - 1)->operator int32();
        int predictedClass = predictions[i];
        
        confusionMatrix[trueClass*dataset->num_classes() + predictedClass]++;
    }
    
    return confusionMatrix;
}

float computeAccuracy(int* confusionMatrix, ArffData* dataset)
{
    int successfulPredictions = 0;
    
    for(int i = 0; i < dataset->num_classes(); i++) {
        successfulPredictions += confusionMatrix[i*dataset->num_classes() + i]; // elements in the diagonal are correct predictions
    }
    
    return 100 * successfulPredictions / (float) dataset->num_instances();
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("Usage: ./program datasets/train.arff datasets/test.arff k num_threads");
        exit(0);
    }

    // freopen("threaded.txt", "w", stdout);
    
    // k value for the k-nearest neighbors
    int k = strtol(argv[3], NULL, 10);
    int num_threads = strtol(argv[4], NULL, 10);

    // Open the datasets
    ArffParser parserTrain(argv[1]);
    ArffParser parserTest(argv[2]);
    ArffData *train = parserTrain.parse();
    ArffData *test = parserTest.parse();
    
    struct timespec start, end;
    int* predictions = NULL;
    
    // Initialize time measurement
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    
    predictions = thread_POSIX_KNN(train, test, k, num_threads);
    
    // Stop time measurement
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // Compute the confusion matrix
    int* confusionMatrix = computeConfusionMatrix(predictions, test);
    // Calculate the accuracy
    float accuracy = computeAccuracy(confusionMatrix, test);

    uint64_t time_difference = (1000000000L * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec) / 1e6;

    printf("POSIX Thread num_thread: %d\n", num_threads);
    printf("The %i-NN classifier for %lu test instances and %lu train instances required %llu ms CPU time for threaded with %d threads. Accuracy was %.2f\%\n", k, test->num_instances(), train->num_instances(), (long long unsigned int) time_difference, accuracy, num_threads);
    // print_memory_usage();
    
    free(predictions);
    free(confusionMatrix);
}

/*  // Example to print the test dataset
    float* test_matrix = test->get_dataset_matrix();
    for(int i = 0; i < test->num_instances(); i++) {
        for(int j = 0; j < test->num_attributes(); j++)
            printf("%.0f, ", test_matrix[i*test->num_attributes() + j]);
        printf("\n");
    }
*/