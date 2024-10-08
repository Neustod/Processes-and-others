﻿#include <iostream>
#include <windows.h>


// Основная нить родительского процесса, запускающая дочерний процесс. После успешного создания ждет завершения дочернего процесса.
int main()
{
    STARTUPINFO sti; // Сильно много объяснять не буду по структурам. Эта структура содержит данные о стартовых параметрах процесса.
    PROCESS_INFORMATION pi; // Эта структура содержит дескрипторы процесса и основной нити дочернего процесса.

    DWORD dwExitCode{ 0 };

    TCHAR cmd[] = __TEXT("101001"); // Аргумент, который мы передадим в дочерний процесс.
    TCHAR path[] = __TEXT("../Debug/Lab1Child.exe"); // Путь до исполнительного файла дочернего процесса.

    // В обязательном порядке обнуляем структуры, чтобы там случайно не оказалось мусора, который мог бы потенциально вызвать нежелательный результат.
    ZeroMemory(&sti, sizeof STARTUPINFO);
    ZeroMemory(&pi, sizeof PROCESS_INFORMATION);

    // Это просто принять как данность. Нужно сохранять размер структуры STARTUPINFO в ней же.
    sti.cb = sizeof(STARTUPINFO);

    /*CreateProcess краткая справка
        Объявление CreateProcess прежде всего является макросом, который раскроется в CreateProcessW в UNICODE системах, а в противном случае в CreateProcessA (ANSI).
        От этого будут так же зависеть и типы принимаемых аргументов. (см. макрос TCHAR)

        Функция создает новый процесс с указанными параметрами.
        
        ВАЖНО:
            если был открыт дескриптор процесса, процесс отработал, и больше не нужен, то мы должны в обязательном порядке закрыть этот дескриптор через CloseHandle.
            Это возволит системе своевременно освободить ресурсы, и уберечь от утечек памяти.

    Аргументы к функции CreateProcess:
        1. Путь до исполнительного файла. Для CreateProcessW это строка WCHAR, а для CreateProcessA это строка CHAR
        2. Аргументы к исполняемому файлу.
        3. Указатель на структуру Process security attributes. Определяет возможность наследования дескриптора создаваемого процесса дочерними процессами. При NULL не может быть наследован.
        4. См. пункт 3, только речь про аттрибуты безопасности для нити процесса, а не для процесса.
        5. Булево значение InheritHandles. Значение TRUE позволит дочернему процессу получить доступ к дескриптору родительского.
        6. CreationFlags. Очевидно из названия, мы можем задать стартовые параметры для дочернего процесса. 
           Например, флаг CREATE_SUSPENDED позволит создать приостановленный процесс, а NORMAL_PRIORITY_CLASS позволит установить приоритет процесса в системе.
        7. Указатель на среду выполнения. При NULL выбирается среда родительского класса.
        8. Указание каталога с исполняемым файлом. Если NULL, то та же, что и у родительского.
        9. Указатель на структуру STATRUPINFO
        10. Указатель на структуру PROCESS_INFORMATION

    Возвращаемое значение:
        Если успешно, то ненулевое значение. Иначе 0.
    */
    if (!CreateProcess(path, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &sti, &pi)) 
    {
        dwExitCode = GetLastError(); // Возвращает код последней ошибки
        printf_s("Error: unable to create child process. Code 0x%x.\n", dwExitCode);

        system("pause");
        return dwExitCode;
    }
    else 
    {
        printf_s("Child process started successfully.\n");

        // Заранее закрываем дескриптор нити дочернего процесса, потому что мы взаимодействуем в основном с дескриптором процесса
        CloseHandle(pi.hThread);

        WaitForSingleObject(pi.hProcess, INFINITE); // Функция приостанавливает выполнение текущей нити до перехода дескриптора процесса в сигнальное состояние.
        
        printf_s("Child process is closed.\n");

        // Закрываем дескриптор процесса для освобождения ресурсов.
        CloseHandle(pi.hProcess);
    }

    system("pause");
    return dwExitCode;
}
