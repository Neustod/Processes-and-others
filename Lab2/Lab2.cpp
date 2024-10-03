#include <iostream>
#include <windows.h>
#include <process.h>
#include <cmath>

#define THREADS_AMOUNT 4
#define INTEGRAL_DELTA_DIVIDER 1000


typedef struct _trap_args {
    long double* pairOfRange;
    long double sum;
} TRAPARGS;


double fn(double x) 
{
    return (x - 1) * cos(x);
}


DWORD WINAPI trapezium(TRAPARGS* args) 
{
    long double a{ args->pairOfRange[0] }, b{ args->pairOfRange[1] };
    long double& sum = args->sum;

    int n = INTEGRAL_DELTA_DIVIDER;
    double h = (b - a) / n;
    long double integral = 0.5 * (fn(a) + fn(b));

    for (int i = 1; i < n; i++) {
        integral += fn(a + i * h);
    }
    
    sum = integral * h;
    
    return 0;
}


int main() {
    long double a = -1, b = 1;                          // Задание окрестности вычисления определенного интеграла.
    long double step{ (b - a) / (THREADS_AMOUNT) };     // Вычисление шага для разделение исходного интервала на подинтервалы.

    long double subIntervalPoints[THREADS_AMOUNT + 1];  // Инициализация массива разделительных точек для подинтервалов.
    long double result{ 0 };                            // Переменная для подсчета результата.

    DWORD dwExitCode{ 0 };

    HANDLE thrs[THREADS_AMOUNT];    // Массив дескрипторов нитей.
    TRAPARGS args[THREADS_AMOUNT];  // Аргументы для функций нитей.

    subIntervalPoints[0] = a;               // Задание предельных значений окрестностей интервала на подинтервальной линейке.
    subIntervalPoints[THREADS_AMOUNT] = b;

    for (int i = 1; i < THREADS_AMOUNT; i++) subIntervalPoints[i] = subIntervalPoints[i - 1] + step; // Вычисление значений подинтегральной линейки.

    for (int i = 0; i < THREADS_AMOUNT; i++) 
    {
        args[i] = { &subIntervalPoints[i], 0 };

        /* 
            Функция _beginthreadex объявлена в заголовочном файле process.h.
            Аналогична функции CreateThread за исключением возможности правильного освобождения контекcта нити функцией _endthreadex.
            Впрочем, функция _endthreadex вызывается автоматически при возврате из подпрограммы.

            Функция создает нить в текущем процессе, принимая в аргументы функцию, которую мы вынесем в одельную нить, а также указатель на ряд аргументов.

            ВАЖНО: закрывать дескрипторы нитей после окончания их работы через CloseHandle.

            Принимаемые аргументы:
            1.  lpSecurityAttributtes. Определяет может ли нить быть унаследована дочерними процессами.
            2.  StackSize. Указание начального размера нитевого стека. 0 - значение по умолчанию.
            3.  Функция, исполнение которой мы выносим в отдельную нить.
            4.  Указатель на аргументы функции.
            5.  Флаги создания. Пример: флаг CREATE_SUSPENDED создаст нить в приостановленном состоянии.
            6.  Указатель на переменную, куда сохранится id нити. Если NULL, то никуда.

            Возвращаемые значения:
                В случае успеха возвращает дескриптор новосозданной нити. Иначе возвращает 0
        */
        thrs[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)trapezium, &args[i], 0, NULL);
        if (thrs[i] == 0)
        {
            dwExitCode = GetLastError(); // Получение кода последней ошибки.
            printf_s("Error: unable to create a thread. Code 0x%x.\n", dwExitCode);

            system("pause");
            return 0;
        }
    }

    /*
        Функция WaitForMultipleObjects является очень многофункциональной. Например, мы можем ждать перехода в сигнальное состояние как всех нитей, так и хотя бы одной.
        Во втором случае, функция возвратит самый маленький индекс одного из дескрипторов нитей + WAIT_OBJECT_0 (Это нужно вычитать при вычислении индекса).
        Также функция может захватывать некоторые элементы по типу семафоров для организации критичесих секций.

        Функция приостанавливает выполнение вызывающей ее нити, пока не выполнятся установленные условия.

        Принимаемые аргументы:
        1.  Размер массива с дескрипторами нитей.
        2.  Указатель на массив дескрипторов нитей.
        3.  Булево значение. В режиме true функция ждет пока не перейдут в сигнальное состояние ВСЕ указанные нити. 
            Иначе функция ждет пока не перейдет в сигнальное состояние хотя бы одна нить.
        4.  Время ожидания в мс. Макрос INFINITE значит то, что значит, собственно, бесконечное ожидание.

        Возвращаемые значения:
            Возвращает одно из ряда значений: WAIT_OBJECT_0, WAIT_ABANDONED, WAIT_TIMEOUT.
            В режиме ожидания хотя бы одной нити возвращает самый маленький индекс одного из дескрипторов нитей + WAIT_OBJECT_0 (Это нужно вычитать при вычислении индекса).
    */
    WaitForMultipleObjects(THREADS_AMOUNT, thrs, true, INFINITE);

    for (int i = 0; i < THREADS_AMOUNT; i++) 
    {
        CloseHandle(thrs[i]);

        result += args[i].sum;
    }

    std::cout << "Function: (x - 1) * cos(x)\nRange: " << a << " to " << b << "\nIntegral value : " << result << std::endl;

    system("pause");
    return 0;
}
