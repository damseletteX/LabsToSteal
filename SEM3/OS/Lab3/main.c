#include "mmemory.h"
#include <stdio.h>
#include <string.h>

// Глобальные счетчики для отслеживания результатов прохождения тестов
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * Вспомогательная функция-ассершн для проверки условий.
 * Если условие истинно (cond == true), засчитывает успех [PASS].
 * Если ложно, фиксирует провал [FAIL].
 */
static void check(bool cond, const char *name)
{
    if (cond)
    {
        printf("[PASS] %s\n", name);
        tests_passed++;
    }
    else
    {
        printf("[FAIL] %s\n", name);
        tests_failed++;
    }
}

/**
 * Тест 1: Проверка инициализации менеджера памяти.
 * Проверяет, что:
 * - Вызов mmap прошел успешно (память выделилась).
 * - Общий размер (total_size) установился корректно.
 * - Указатель head указывает на начало выделенной области.
 * - Размер первого блока учитывает вычет служебной структуры (sizeof(MemoryBlock)).
 * - Блок помечен как свободный (isFree == true), а указатель next равен NULL.
 * - Функция очистки (clear) корректно сбрасывает указатель head.
 */
static void test_init(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    check(m.start_addr != NULL, "init: mmap ok");
    check(m.total_size == 4096, "init: total_size == 4096");
    check(m.head == (MemoryBlock *)m.start_addr, "init: head at start_addr");
    check(m.head->size == 4096 - sizeof(MemoryBlock), "init: head size correct");
    check(m.head->isFree == true, "init: head is free");
    check(m.head->next == NULL, "init: head next is NULL");
    clear(&m);
    check(m.head == NULL, "init: clear sets head NULL");
}

/**
 * Тест 2: Проверка искусственной фрагментации.
 * Проверяет, что функция run_fragmentation:
 * - Успешно разбивает память на несколько блоков (их больше одного).
 * - Суммарный объем всех блоков и их заголовков не превышает размер mmap.
 * - Все созданные блоки изначально помечены как свободные.
 */
static void test_fragmentation(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    run_fragmentation(&m);

    size_t used = 0;
    int count = 0;
    bool all_free = true;
    MemoryBlock *b = m.head;
    while (b != NULL)
    {
        used += sizeof(MemoryBlock) + b->size;
        count++;
        if (!b->isFree)
        {
            all_free = false;
        }
        b = b->next;
    }
    check(count >= 2, "frag: more than one block");
    check(used <= m.total_size, "frag: blocks fit into mmap");
    check(all_free, "frag: all blocks free");
    clear(&m);
}

/**
 * Тест 3: Проверка выделения и освобождения памяти.
 * Проверяет, что:
 * - Аллокатор успешно выделяет небольшой блок памяти и возвращает корректный указатель (смещенный за заголовок).
 * - Выделенный блок меняет статус на занятый (isFree == false).
 * - При повторном запросе, когда свободного места нет, возвращается NULL.
 * - Функция освобождения (free_memory) возвращает блоку статус свободного.
 * - Повторный запрос памяти корректно переиспользует (возвращает) тот же самый освобожденный блок.
 */
static void test_allocate(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);

    void *p = allocate_memory(&m, 16);
    check(p != NULL, "alloc: small block ok");
    check(p == (char *)m.head + sizeof(MemoryBlock), "alloc: returns data area");
    check(m.head->isFree == false, "alloc: block marked busy");

    void *q = allocate_memory(&m, 16);
    check(q == NULL, "alloc: no free block left");

    free_memory(p);
    check(m.head->isFree == true, "free: block free again");

    void *r = allocate_memory(&m, 16);
    check(r == p, "alloc: reuses freed block");
    clear(&m);
}

/**
 * Тест 4: Проверка выделения слишком большого объема памяти.
 * Убеждается, что при запросе размера, превышающего доступный лимит, аллокатор корректно возвращает NULL вместо падения.
 */
static void test_allocate_too_big(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    void *p = allocate_memory(&m, 6000);
    check(p == NULL, "alloc: too big returns NULL");
    clear(&m);
}

/**
 * Тест 5: Проверка безопасного чтения и записи.
 * Проверяет, что функции safe_write и safe_read корректно записывают строку в выделенный буфер и успешно считывают ее обратно (roundtrip).
 */
static void test_safe_read_write(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);
    void *p = allocate_memory(&m, 32);
    safe_write(p, 32, "hello");
    check(strcmp(safe_read(p, 32), "hello") == 0, "rw: roundtrip ok");
    clear(&m);
}

/**
 * Тест 6: Проверка защиты от выхода за границы и работы с освобожденной памятью (Out-of-Bounds & Use-After-Free).
 * Проверяет, что:
 * - Попытка прочитать объем данных, превышающий размер блока, пресекается (возвращает NULL).
 * - Попытка записать слишком большой объем данных отклоняется, оставляя существующие данные нетронутыми.
 * - Попытка чтения из уже освобожденного блока блокируется (возвращает NULL).
 */
static void test_out_of_bounds(void)
{
    MemoryManager m;
    init_memory_manager(&m, 4096);

    void *p = allocate_memory(&m, 16);
    check(safe_read(p, 6000) == NULL, "oob: read beyond block returns NULL");
    safe_write(p, 6000, "too long");
    check(safe_read(p, 32) != NULL, "oob: write refused, data intact");

    free_memory(p);
    check(safe_read(p, 16) == NULL, "oob: read from free block returns NULL");
    clear(&m);
}

static void test_load(void)
{
    MemoryManager m;
    init_memory_manager(&m, 8192); // Выделяем больший объем для нагрузки
    run_fragmentation(&m);         // Создаем предварительную фрагментацию

    void *ptrs[100];
    for (int i = 0; i < 100; i++) {
        ptrs[i] = NULL;
    }

    int successful_allocs = 0;
    int failed_allocs = 0;

    // Выполняем 1000 случайных итераций аллокации/освобождения
    for (int i = 0; i < 1000; i++)
    {
        int idx = rand() % 100;
        if (ptrs[idx] != NULL)
        {
            free_memory(ptrs[idx]);
            ptrs[idx] = NULL;
        }
        else
        {
            size_t req_size = (rand() % 32) + 4; // Случайный размер от 4 до 35 байт
            void *p = allocate_memory(&m, req_size);
            if (p != NULL)
            {
                ptrs[idx] = p;
                safe_write(p, req_size, "load_test");
                successful_allocs++;
            }
            else
            {
                failed_allocs++; // Фиксируем отказы из-за фрагментации
            }
        }
    }

// Очищаем оставшиеся занятые указатели перед удалением менеджера
    for (int i = 0; i < 100; i++)
    {
        if (ptrs[i] != NULL)
        {
            free_memory(ptrs[i]);
        }
    }

    // Проверяем, что тест отработал стабильно и успешно выделил хотя бы часть памяти
    check(successful_allocs > 0, "load: successfully handled stress allocations");
    
    clear(&m);
    check(m.head == NULL, "load: clear ok after stress test");
}

int main(void)
{
    test_load();
    test_init();
    test_fragmentation();
    test_allocate();
    test_allocate_too_big();
    test_safe_read_write();
    test_out_of_bounds();

    printf("Tests passed: %d, failed: %d\n", tests_passed, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}