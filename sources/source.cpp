// Copyright 2021 Your Name <your_email>

#include <header.hpp>

Hasher::Hasher(int argc, char **argv) {
  if (argc == 1) { //если 1 то мы просто запустили прогу, НО НИЧЕГО НЕ ВВЕЛИ
    number_threads = std::thread::hardware_concurrency(); //в этом случае кол-во поток = максимальному кол-ву потоков (кол-ву ядер)
  } else {
    number_threads = static_cast<unsigned int>(std::stoi(argv[1])); //кол-во потоков устанавливается через цифру которую мы ввели
  }
  duration = 0; //длительность
}

void Hasher::start(const bool& key) {
  logging::add_common_attributes(); //подключаем стандартные атрибуты
  logging::add_console_log(std::clog, keywords::format = "[%Severity%] %TimeStamp%: %Message%");
  //эддсонсольлог - ф-ия вывода на консоль по следующему формату
  logging::add_file_log //ф-ия добавления в файл
      (
          keywords::file_name = "sample_%N.log", //Сэмпл1.лог после переполнения Сэмпл2.лог
          keywords::rotation_size = 10 * 1024 * 1024, //когда файл достигает размера 10мб создается новый файл
          keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
          keywords::format = "[%Severity%][%TimeStamp%]: %Message%"
      );
  for(unsigned int i = 0; i < number_threads; ++i){
    threads.emplace_back(std::thread([&](){ //эмплэйсбэк конструирует объект (выполняет конструктор) и записывает поток в вектор потоков, сам поток при создании выполняет лямбда-ф-ию(безымянную фу-ия)
      do_hashing(key);  //Амперсант в кв скобках показывает что мы захватываем все что происходило раньше и далее каждый поток выполняет тело функции
    }));
  }
}

void Hasher::do_hashing(const bool& key) {
  time_point start = high_clock::now();
  while(key){
    mutex.lock();
    src_str = std::to_string(std::rand()); //переводим рандом число в строку
    hash_hex_str = picosha2::hash256_hex_string(src_str); //делаем хэш строку из стринга
    time_point end = high_clock::now(); //замеряем снова время
    duration += static_cast<int>(std::chrono::nanoseconds(end - start).count());
    start = end;
    sort_hash(hash_hex_str);
    mutex.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(2));
  }
}

void Hasher::sort_hash(string &hash) { //ф-ия сортировки хэшей, отлавливаем такие у которых в конце 0000
  if(hash_hex_str.substr(60, 4) == Hex_End) { //сабстр с 60 позиции вырезает 4 символа
    BOOST_LOG_SEV(slg, info) << endl //слово ИНФО записывает в начало сообщения, бустлогсерв - логер, выводит сообщения в консоль и файл
                             << "sourse: " << std::hex << std::stol(src_str) << std::dec
                             << " hash: " << hash
                             << " duration: " << duration
                             << " thread: " << std::this_thread::get_id() << endl;
    json j = {
        {"sourse", src_str}, //то что зашло(рандом число в строке)
        {"hash_hex", hash_hex_str},//хеш суума полученная
        {"duration", duration}
    };
    right_hashs.push_back(j); //правильные хэши записываем в вектор джисонов
  } else {
    BOOST_LOG_SEV(slg, trace) << endl //то же самое что и выше только другой уровень серьезности
                              << "sourse: " << std::hex << std::stol(src_str) << std::dec
                              << " hash: " << hash_hex_str
                              << " thread: " << std::this_thread::get_id() << endl;
  }
}

Hasher::~Hasher() {
  for(unsigned int i = 0; i < number_threads; ++i) {
    threads[i].join();
  }
}

std::vector<json> Hasher::right_hashs;