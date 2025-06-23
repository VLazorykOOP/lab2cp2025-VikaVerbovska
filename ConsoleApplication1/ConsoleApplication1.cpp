#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>     // Для потоків
#include <mutex>      // Для м'ютексів (блокування)
#include <chrono>     // Для затримок

using namespace std;

struct Point {
    double x, y;
};

// Клас робочої бджоли
class WorkerBee {
public:
    Point home;          // Точка початку (додому)
    Point pos;           // Поточна позиція
    Point target;        // Ціль руху (0,0 або додому)
    double speed;        // Швидкість руху
    bool goingToCorner;  // Чи летить до кута (0,0) чи назад додому
    mutex mtx;           // М'ютекс для захисту pos від одночасного доступу

    WorkerBee(Point start, double V) {
        home = start;
        pos = start;
        target = { 0, 0 };
        speed = V;
        goingToCorner = true;
    }

    // Метод руху: змінює pos на крок в напрямку target
    void move() {
        lock_guard<mutex> lock(mtx);  // Захищаємо pos від одночасного доступу

        double dx = target.x - pos.x;
        double dy = target.y - pos.y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist <= speed) {
            // Якщо досягли цілі, змінюємо напрямок руху
            pos = target;
            if (goingToCorner) {
                target = home;          // Летимо додому
                goingToCorner = false;
            }
            else {
                target = { 0, 0 };       // Летимо в кут (0,0)
                goingToCorner = true;
            }
        }
        else {
            // Рухаємось кроком зі швидкістю speed в напрямку target
            pos.x += speed * dx / dist;
            pos.y += speed * dy / dist;
        }
    }

    // Метод для безпечного отримання позиції
    Point getPosition() {
        lock_guard<mutex> lock(mtx);
        return pos;
    }
};

// Клас трутня
class DroneBee {
public:
    Point pos;             // Поточна позиція
    double speed;          // Швидкість
    double direction;      // Кут руху в радіанах
    int stepsBeforeChange; // Секунд до зміни напряму руху
    int N;                 // Як часто міняти напрямок (у секундах)
    mutex mtx;             // М'ютекс для захисту pos і direction

    DroneBee(Point start, double V, int changeEvery) {
        pos = start;
        speed = V;
        N = changeEvery;
        stepsBeforeChange = 0;
        changeDirection();
    }

    // Метод вибору нового випадкового напряму руху
    void changeDirection() {
        direction = (rand() % 360) * 3.14159 / 180.0;  // Кут у радіанах
        stepsBeforeChange = N;
    }

    // Метод руху: рухаємось в напрямку direction
    void move() {
        lock_guard<mutex> lock(mtx);

        if (stepsBeforeChange == 0) {
            changeDirection();
        }

        pos.x += speed * cos(direction);
        pos.y += speed * sin(direction);

        // Обмежуємо рух у межах 0..100 по x і y
        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x > 100) pos.x = 100;
        if (pos.y > 100) pos.y = 100;

        stepsBeforeChange--;
    }

    // Метод для безпечного отримання позиції
    Point getPosition() {
        lock_guard<mutex> lock(mtx);
        return pos;
    }
};

// Функція для роботи в окремому потоці робочої бджоли
void workerThreadFunc(WorkerBee& worker, int steps) {
    for (int i = 0; i < steps; i++) {
        worker.move();  // Зробити крок руху
        this_thread::sleep_for(chrono::seconds(1));  // Затримка 1 секунда
    }
}

// Функція для роботи в окремому потоці трутня
void droneThreadFunc(DroneBee& drone, int steps) {
    for (int i = 0; i < steps; i++) {
        drone.move();  // Зробити крок руху
        this_thread::sleep_for(chrono::seconds(1));  // Затримка 1 секунда
    }
}

int main() {
    srand(time(0));  // Ініціалізація генератора випадкових чисел

    WorkerBee worker({ 50, 50 }, 5);    // Робоча бджола стартує з (50,50), швидкість 5
    DroneBee drone({ 20, 20 }, 3, 4);   // Трутень стартує з (20,20), швидкість 3, змінює напрямок кожні 4 сек

    int simulationTime = 30;  // Час симуляції в секундах

    // Запускаємо потоки для руху кожної бджоли
    thread workerThread(workerThreadFunc, ref(worker), simulationTime);
    thread droneThread(droneThreadFunc, ref(drone), simulationTime);

    // У головному потоці виводимо позиції кожної секунди
    for (int t = 0; t < simulationTime; t++) {
        Point wPos = worker.getPosition();  // Безпечне читання позиції робочої бджоли
        Point dPos = drone.getPosition();   // Безпечне читання позиції трутня

        cout << "Time " << t << " sec:" << endl;
        cout << "  WorkerBee: (" << wPos.x << ", " << wPos.y << ")" << endl;
        cout << "  DroneBee: (" << dPos.x << ", " << dPos.y << ")" << endl;
        cout << "---------------------" << endl;

        this_thread::sleep_for(chrono::seconds(1));  // Затримка 1 секунда перед наступним виводом
    }

    // Очікуємо завершення потоків
    workerThread.join();
    droneThread.join();

    return 0;
}
