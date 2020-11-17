/*Определить множество индексов i, для которых (A[i] - B[i]) или
(A[i] + B[i]) являются простыми числами. Входные данные: массивы целых
положительных чисел А и B, произвольной длины ≥ 1000. Количество
потоков является входным параметром.
 Динеев Владислав Денисович
 Бпи191
 17.11.2020
*/

#include <iostream>
#include <thread>
#include "ctime"
#include <random>

// определяем структуру для передачи данных потоку
typedef struct {
    int start_index;
    int thread_number;
    int array_size;
    int* array_a;
    int* array_b;
    int* res_array;
} threadData;

// функция проверки числа на простоту
bool isSimple(int number){
    int n = abs(number);
    for(long long i=2;i<=sqrt(n);i++)
        if(n%i==0)
            return false;
    return true;
}

// функция для каждого из потоков
void* threadFunc(void* thread_data){
    threadData* data = (threadData*) thread_data;

    for (int i = data->start_index; i < data->array_size; i+=data->thread_number) {
        if(isSimple(data->array_a[i] + data->array_b[i]) || isSimple(data->array_a[i] - data->array_b[i])){
            data->res_array[i] = 1;
        }
    }
    return NULL;
}

int main() {
    // пользователь вводит данные
    int array_size;
    std::cout << "Input arrays size:";
    std::cin >> array_size;

    if(array_size < 1000){
        std::cout << "Incorrect array size! Has to be greater than 1000.";
        return -1;
    }

    int thread_number;
    std::cout << "Input number of threads:";
    std::cin >> thread_number;

    if(thread_number < 1){
        std::cout << "Incorrect number of threads! Has to be no less than 1.";
        return -1;
    }

    // выделяем память под массивы A, B и массив результатов
    int* a_array = (int*) malloc(array_size * (sizeof(int)));
    int* b_array = (int*) malloc(array_size * (sizeof(int)));
    int* res_array = (int*) malloc(array_size * sizeof(int));

    // генератор случайных чисел
    std::default_random_engine generator(time(0));
    // ограничил 10000
    std::uniform_int_distribution<int> distribution(1,10000);

    for (int i = 0; i < array_size; ++i) {
        // заполняем массив случайными числами для удобства
        a_array[i] = distribution(generator);
        b_array[i] = distribution(generator);
        res_array[i] = 0;
    }

    // выделяем память под масссив ссылок на потоки и данные, которые в них будем передавать
    std::thread* threads = (std::thread*) malloc(thread_number * sizeof(std::thread));
    threadData* thread_data_array = (threadData*) malloc(thread_number * sizeof(threadData));


    // заполняем массив потоков и запускаем выполнение
    for (int i = 0; i < thread_number; ++i) {
        thread_data_array[i].start_index = i;
        thread_data_array[i].thread_number = thread_number;
        thread_data_array[i].array_size = array_size;
        thread_data_array[i].array_a = a_array;
        thread_data_array[i].array_b = b_array;
        thread_data_array[i].res_array = res_array;

        new (&threads[i]) std::thread(threadFunc, &(thread_data_array[i]));
    }

    // ждем завершения работы потоков
    for (int i = 0; i < thread_number; ++i) {
        threads[i].join();
    }

    // выводим результат
    std::cout << "Indices of elements, which sum or difference is prime:" << std::endl;
    for (int i = 0; i < array_size; ++i) {
        if(res_array[i] == 1){
            std::cout << i << std::endl;
        }
    }

    // освобождаем память
    free(threads);
    free(thread_data_array);
    free(a_array);
    free(b_array);
    free(res_array);
}
