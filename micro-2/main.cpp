/*13. Задача о гостинице. В гостинице 30 номеров, клиенты гостиницы снимают номер на одну ночь,
* если в гостинице нет свободных номеров, клиенты устраиваются на ночлег рядом с гостиницей и ждут,
* пока любой номер не освободится. Создать многопоточное приложение, моделирующее работу гостиницы.
 * Выполнил: Динеев Владислав Денисович
 * Группа: БПИ191
 * Вариант: 13
 */

#include <iostream>
#include "thread"
#include "mutex"
#include "queue"
#include "chrono"

using namespace std;

const int MEAN_WAIT_TIME = 40;
const int GUEST_ARRIVAL_TIME = 1;
const int ROOM_COUNT = 30;

queue<int> freeRooms;
mutex selectLock;
mutex freeMutex;
mutex outMutex;
condition_variable freeRoom;

void stay(int guestNumber) {
    selectLock.lock();
    if (freeRooms.empty()) {
        unique_lock<mutex> lock(freeMutex);
        selectLock.unlock();
        outMutex.lock();
        cout << "Guest " << guestNumber << " is waiting for room to free..." << endl;
        outMutex.unlock();
        freeRoom.wait(lock);
        lock.unlock();
        selectLock.lock();
    }

    int roomNumber = freeRooms.front();
    freeRooms.pop();
    selectLock.unlock();

    outMutex.lock();
    cout << "Guest " << guestNumber << " is now staying in the room " << roomNumber << endl;
    outMutex.unlock();
    this_thread::sleep_for(chrono::seconds(MEAN_WAIT_TIME));

    selectLock.lock();
    freeRooms.push(roomNumber);
    selectLock.unlock();
    outMutex.lock();
    cout << "Guest " << guestNumber << " freed the room " << roomNumber << endl;
    outMutex.unlock();
    freeRoom.notify_one();
}

int main() {
    int guestCount;
    cout << "Input guest count: ";
    cin >> guestCount;
    cout << endl;

    for (int i = 0; i < ROOM_COUNT; ++i) {
        freeRooms.push(i);
    }

    thread *guests[guestCount];
    for (int i = 0; i < guestCount; ++i) {
        guests[i] = new thread(stay, i);
        this_thread::sleep_for(chrono::seconds(GUEST_ARRIVAL_TIME));
    }

    for (int i = 0; i < guestCount; ++i) {
        guests[i]->join();
        delete guests[i];
    }
}
