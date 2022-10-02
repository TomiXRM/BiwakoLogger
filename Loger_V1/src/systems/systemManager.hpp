#ifndef SYSTEM_HPP
#define SYSTEM_HPP

// #include "../setup/setup.hpp"

class system {
  public:
    system(char *name, int id);
    void init();
    void loop();

  private:
    char *name;
    int id;
    char mode;
};
#endif