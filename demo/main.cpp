#include <header.hpp>

int main(int argc, char *argv[]) { //аргс кол-во аргументов, аргв сами аргументы
  signal(SIGINT, Hasher::signal_catch); //обработчик сигнала,ф-ия ловит сигнал Кнтрл+С ЛОВИТ и запускает сигналкэч
  Hasher hasher(argc, argv); //в конструктор хэшер передаем аргументы
  hasher.start(true);
}