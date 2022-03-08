#include <iostream>
#include "caf/all.hpp"
#include "supervisor.h"
#include "ping.h"
#include "pong.h"

using namespace caf;

void caf_main(actor_system& system) {
  //std::cout << "hello world" << std::endl;
  auto po = system.spawn(pong);
  auto pi = system.spawn(ping, po);
  auto sv = system.spawn(supervisor, pi);
}

CAF_MAIN()