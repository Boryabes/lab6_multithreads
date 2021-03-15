#include <header.hpp>

int main(int argc, char *argv[]) {
  signal(SIGINT, Hasher::signal_catch);
  Hasher hasher(argc, argv);
  hasher.start(true);
}
