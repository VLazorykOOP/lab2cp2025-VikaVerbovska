#include <iostream>
#include <cmath>
#include <cstdlib>  // Для rand()
#include <ctime>    // Для time()

using namespace std;

struct Point {
    double x, y;
};

// Функція для обчислення відстані між двома точками
double distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Клас бджоли-робочої
class WorkerBee {
public:
    Point home;    // Точка народження
    Point pos;     // Поточна позиція
    Point target;  // Ціль (0,0)
    double speed;  // Швидкість
    bool goingToCorner; // Чи летить в кут (0,0) чи повертається додому

    WorkerBee(Point start, double V) {
        home = start;
        pos = start;
        target = { 0, 0 };
        speed = V;
        goingToCorner = true;
    }

    void move() {
        // Вектор руху від pos до target
        double dx = target.x - pos.x;
        double dy = target.y - pos.y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist <= speed) {
            // Досягли цілі - міняємо напрямок
            pos = target;
            if (goingToCorner) {
                target = home;  // Повертаємось додому
                goingToCorner = false;
            }
            else {
                target = { 0, 0 };  // Летимо назад в кут
                goingToCorner = true;
            }
        }
        else {
            // Рухаємось у напрямку target зі швидкістю speed
            pos.x += speed * dx / dist;
            pos.y += speed * dy / dist;
        }
    }
};

// Клас трутня
class DroneBee {
public:
    Point pos;
    double speed;
    double direction; // Кут руху в радіанах
    int stepsBeforeChange; // Кількість секунд до зміни напряму
    int N; // Як часто міняти напрямок

    DroneBee(Point start, double V, int changeEvery) {
        pos = start;
        speed = V;
        N = changeEvery;
        stepsBeforeChange = 0;
        changeDirection();
    }

    void changeDirection() {
        // Випадковий кут від 0 до 2*PI
        direction = (rand() % 360) * 3.14159 / 180.0;
        stepsBeforeChange = N;
    }

    void move() {
        if (stepsBeforeChange == 0) {
            changeDirection();
        }

        // Рух в напрямку direction
        pos.x += speed * cos(direction);
        pos.y += speed * sin(direction);

        // Обмеження руху (щоб не виходити за межі [0; Xmax], [0; Ymax])
        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x > 100) pos.x = 100;  
        if (pos.y > 100) pos.y = 100;  

        stepsBeforeChange--;
    }
};

int main() {
    srand(time(0));  // Ініціалізація генератора випадкових чисел

    WorkerBee worker({ 50, 50 }, 5);  // Починаємо з (50,50), швидкість 5 одиниць/сек
    DroneBee drone({ 20, 20 }, 3, 4); // Починаємо з (20,20), швидкість 3, змінюємо напрямок кожні 4 сек

    for (int t = 0; t < 30; t++) {  // Симуляція 30 секунд
        worker.move();
        drone.move();

        cout << "Time " << t << " sec:" << endl;
        cout << "  WorkerBee: (" << worker.pos.x << ", " << worker.pos.y << ")" << endl;
        cout << "  DroneBee: (" << drone.pos.x << ", " << drone.pos.y << ")" << endl;
        cout << "---------------------" << endl;
    }

    return 0;
}
