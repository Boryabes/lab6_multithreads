// Copyright 2021 Your Name <your_email>

#include <header.hpp>

Hasher::Hasher(int argc, char **argv) {
  if (argc == 1) {
    number_threads = std::thread::hardware_concurrency();
  } else {
    number_threads = static_cast<unsigned int>(std::stoi(argv[1]));
  }
  duration = 0; //длительность
}

void Hasher::start(const bool& key) {
  logging::add_common_attributes();
  logging::add_console_log(std::clog, keywords::format = "[%Severity%] %TimeStamp%: %Message%");
  //эддсонсольлог - ф-ия вывода на консоль по следующему формату
  logging::add_file_log //ф-ия добавления в файл
      (
          keywords::file_name = "sample_%N.log",
          keywords::rotation_size = 10 * 1024 * 1024,
          keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
          keywords::format = "[%Severity%][%TimeStamp%]: %Message%"
      );
  for(unsigned int i = 0; i < number_threads; ++i){
    threads.emplace_back(std::thread([&](){
      do_hashing(key);
    }));
  }
}

void Hasher::do_hashing(const bool& key) {
  time_point start = high_clock::now();
  while(key){
    mutex.lock();
    src_str = std::to_string(std::rand());
    hash_hex_str = picosha2::hash256_hex_string(src_str);
    time_point end = high_clock::now();
    duration += static_cast<int>(std::chrono::nanoseconds(end - start).count());
    start = end;
    sort_hash(hash_hex_str);
    mutex.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(2));
  }
}

void Hasher::sort_hash(string &hash) {
  if(hash_hex_str.substr(60, 4) == Hex_End) {
    BOOST_LOG_SEV(slg, info) << endl
                             << "sourse: " << std::hex << std::stol(src_str) << std::dec
                             << " hash: " << hash
                             << " duration: " << duration
                             << " thread: " << std::this_thread::get_id() << endl;
    json j = {
        {"sourse", src_str},
        {"hash_hex", hash_hex_str},
        {"duration", duration}
    };
    right_hashs.push_back(j);
  } else {
    BOOST_LOG_SEV(slg, trace) << endl
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