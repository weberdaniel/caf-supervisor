#include <iostream>
#include "caf/all.hpp"

using namespace caf;

CAF_BEGIN_TYPE_ID_BLOCK(supervisor, caf::first_custom_type_id)
// process_function
CAF_ADD_ATOM(supervisor, work);
CAF_ADD_ATOM(supervisor, supervise);
// restart_type
CAF_ADD_ATOM(supervisor, permanent);
CAF_ADD_ATOM(supervisor, transient);
CAF_ADD_ATOM(supervisor, temporary);
// restart_strategy
CAF_ADD_ATOM(supervisor, one_for_one);
CAF_ADD_ATOM(supervisor, one_for_all);
CAF_ADD_ATOM(supervisor, rest_for_one);
CAF_ADD_ATOM(supervisor, simple_one_for_one);
// auto-shutdown
CAF_ADD_ATOM(supervisor, never);
CAF_ADD_ATOM(supervisor, any_significant);
CAF_ADD_ATOM(supervisor, all_significant);
// shutdown
CAF_ADD_ATOM(supervisor, brutal_kill);
CAF_ADD_ATOM(supervisor, infinity);
CAF_ADD_ATOM(supervisor, wait);
// other
CAF_ADD_ATOM(supervisor, pinging_atom);
CAF_ADD_ATOM(supervisor, get_child);
// other
CAF_ADD_ATOM(supervisor, keep_alive);
CAF_END_TYPE_ID_BLOCK(supervisor)

template<class TStaticStartArgs>
struct childspec {
    // this is not a PID or Actor Id, it is only a name for the spec
    std::string child_id;
    // start arguments
    TStaticStartArgs start;
    // permanent | transient | temporary
    std::string_view restart;
    // a supervisor can perform an auto-shutdown in case
    // significant children are terminated.
    bool significant;
    // if brtual_kill is set, the child process will be unconditionally killed,
    // if infinity is set, the child is a supervisor. If wait is set, a time will
    // be waited, then the actor will be killed unconditionally
    std::string_view shutdown;
    // how long to wait in case of "wait" shutdown parameter
    std::chrono::milliseconds wait_time;
    // type: supervise | work
    std::string_view type;
};

template<class TStaticStartArgs>
class child {
    childspec<TStaticStartArgs> spec;
    actor child;
    int restart_count;
    std::chrono::time_point<std::chrono::system_clock> restart_period_start;
};

struct sup_flags {
  // one_for_one | one_for_all | rest_for_one | simple_one_for_one
  std::string_view restart_strategy;
  // number of max. restarts during one restart period.
  uint32_t restart_intensity;
  // duration of the restart period
  std::chrono::seconds restart_period;
  // never | any_significant | all_significant
  std::string_view auto_shutdown;
};


template<class TClassActor, class TStaticStartArgs>
class supervisor: public event_based_actor {
 public:

  supervisor(actor_config& cfg,
             caf::string_view restart_strategy,
             int32_t intensity,
             std::chrono::seconds period,
             TStaticStartArgs class_actor_static_args)
    : event_based_actor(cfg),
      restart_strategy(restart_strategy),
      intensity(intensity),
      period(period),
      class_actor_static_args_(class_actor_static_args)
  {
    supervising_.assign(
      [=](keep_alive) {
        delayed_send(this, std::chrono::seconds(1), keep_alive_v);
      });
  }

 protected:
  behavior make_behavior() override {
    delayed_send(this, std::chrono::seconds(1), keep_alive_v);
    child = spawn<TClassActor>(class_actor_static_args_);
    this->monitor(child);
    this->set_down_handler([&](down_msg& msg) {
      child = spawn<TClassActor>(class_actor_static_args_);
      this->monitor(child);
      become(supervising_);
    } );
    return supervising_;
  }

 private:
  actor child;
  caf::string_view restart_strategy;
  int32_t intensity;
  std::chrono::microseconds period;
  TStaticStartArgs class_actor_static_args_;
  behavior supervising_;
};

template<class TClassActor>
class supervisor<TClassActor, void> : public event_based_actor {
 public:
  supervisor(actor_config& cfg,
             caf::string_view restart_strategy,
             int32_t intensity,
             std::chrono::microseconds period )
    : event_based_actor(cfg),
      restart_strategy(restart_strategy),
      intensity(intensity),
      period(period)
  {
    supervising_.assign(
      [=](keep_alive) {
        delayed_send(this, std::chrono::seconds(1), keep_alive_v);
      });
  }

  behavior make_behavior() override {
      delayed_send(this, std::chrono::seconds(1), keep_alive_v);
      child = spawn<TClassActor>();
      this->monitor(child);
      this->set_down_handler([&](down_msg& msg) {
          child = spawn<TClassActor>();
          this->monitor(child);
          become(supervising_);
      });
      return supervising_;
  }

private:
    actor child;
    caf::string_view restart_strategy;
    int32_t intensity;
    std::chrono::microseconds period;
    behavior supervising_;
};