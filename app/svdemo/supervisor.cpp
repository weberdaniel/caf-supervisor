#include "supervisor.h"

void one_for_one_strategy(
const std::shared_ptr<supervisor_dynamic_state>& ptr,
const down_msg& msg,
event_based_actor* self) {
  auto it = ptr->children_.begin();
  while (it != ptr->children_.end()) {
    if (msg.source == (*it).address) {
      CAF_LOG_INFO("Received Down Message from " + (*it).child_id);
      std::string id = (*it).child_id;
      for (auto &e: ptr->specs_) {
        if (e.child_id == id) {
          auto duration =
            std::chrono::system_clock::now().time_since_epoch();
          auto millis =
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
            .count();
          auto delta = millis -
            std::chrono::duration_cast<std::chrono::milliseconds>(
            (*it).restart_period_start.time_since_epoch()).count();

          CAF_LOG_INFO("Iterating Child " + e.child_id);
          CAF_LOG_INFO(" -- time passed (ms): " + std::to_string(delta));
          CAF_LOG_INFO(" -- restarts during time passed (#): "
            + std::to_string((*it).restart_count)
            + " (max. " + std::to_string(ptr->flags_.restart_intensity) + ")");

          // 1. reset the restart count if period expired
          if (delta > ptr->flags_.restart_period.count()) {
            CAF_LOG_INFO(" -- reset restarts during time passed " + e.child_id);
            (*it).restart_count = 0;
            (*it).restart_period_start = std::chrono::system_clock::now();
          }

          // 2. if we start a new meassurement/ the old meassurement expired,
          //    set the timer
          if ((*it).restart_count == 0) {
            CAF_LOG_INFO(" -- reset time passed " + e.child_id);
            (*it).restart_period_start = std::chrono::system_clock::now();
          }

          // 3. if we reached the maximum attempts give up
          if ((*it).restart_count == ptr->flags_.restart_intensity) {
            CAF_LOG_INFO(" -- maximum restarts reached for " + e.child_id);
            CAF_LOG_INFO(" -- shut down all children ");
            for (auto &a: ptr->specs_) {
              self->send_exit((*it).process.address(),
              exit_reason::unknown);
            }
            CAF_LOG_INFO(" -- shut down self:");
            self->quit();
            return;
          }

          // 4. if we don't give up, do a restart
          CAF_LOG_INFO(" -- respawn child " + e.child_id);
          actor process = self->home_system().spawn(e.start);
          self->monitor(process);
          (*it).address = process->address();
          (*it).process = std::move(process);
          (*it).restart_count++;
        }
      }
    }
    it++;
  }
}

void rest_for_one_strategy(
  const std::shared_ptr<supervisor_dynamic_state>& ptr,
  const down_msg& msg,
  event_based_actor* self) {

}

void one_for_all_strategy(
        const std::shared_ptr<supervisor_dynamic_state>& ptr,
        const down_msg& msg,
        event_based_actor* self) {

}

void supervisor::init(const std::vector<child_specification>& specs,
supervisor_flags flags) {
  ptr_->specs_ = specs;
  ptr_->flags_ = flags;
}

child::child( child&& copy) noexcept :
  // transfer copy content into this
  child_id(std::move(copy.child_id)),
  address(std::move(copy.address)),
  process(std::move(copy.process)),
  restart_count(std::move(copy.restart_count)),
  restart_period_start(std::move(copy.restart_period_start)) {
    // leave copy in valid but undefined state
    copy.child_id = "";
    copy.address = actor_addr();
    copy.process = nullptr;
    copy.restart_count = -1;
    copy.restart_period_start =
      std::chrono::time_point<std::chrono::system_clock>::min();
};

// note move to self leaves object in valid but undefined state,
// by definition of move operation. therefore move to self should
// never occur, and if it occurs it is defined to leave an undefined
// state, so it must not be handled. See Klaus Iglberger 2019, CppCon.

child& child::operator=(child&& copy) noexcept {
  destroy(process); // clean up all visible ressources
  child_id = std::move(copy.child_id);
  address = std::move(copy.address);
  process = std::move(copy.process);
  restart_count = std::move(copy.restart_count);
  restart_period_start = std::move(copy.restart_period_start);
  copy.child_id = "";
  copy.address = actor_addr();
  copy.process = nullptr;
  copy.restart_count = -1;
  copy.restart_period_start =
    std::chrono::time_point<std::chrono::system_clock>::min();
  return *this;
}

void supervisor::operator()(event_based_actor* self)  {
  CAF_LOG_INFO("Supervisor Start");
  CAF_LOG_INFO("Supervisor Configure Down Handler");

  std::shared_ptr<supervisor_dynamic_state> ptr = ptr_;
  self->set_down_handler([self, ptr](down_msg& msg) {
    if(ptr->flags_.restart_strategy == type_name<one_for_one>::value) {
      one_for_one_strategy(ptr, msg, self);
    } else if(ptr->flags_.restart_strategy == type_name<one_for_all>::value) {
      one_for_all_strategy(ptr, msg, self);
    } else if(ptr->flags_.restart_strategy == type_name<rest_for_one>::value) {
      rest_for_one_strategy(ptr, msg, self);
    }
  });

  CAF_LOG_INFO("Supervisor Start Children: ");
  for( auto& e : ptr_->specs_ ) {
    CAF_LOG_INFO("Supervisor Start Child: " + e.child_id);
    actor child_actor= self->home_system().spawn(e.start);
    self->monitor(child_actor);
    child child;
    child.child_id = e.child_id;
    child.address = child_actor.address();
    child.process= std::move(child_actor);
    child.restart_count = 0;
    child.restart_period_start = std::chrono::system_clock::now();
    ptr_->children_.push_back(std::move(child));
  };
  CAF_LOG_INFO("Supervisor enters Idle state");
  self->become([self,ptr](int msg){
    CAF_LOG_INFO("Supervisor receives keep_alive");
    self->delayed_send(self,std::chrono::seconds(3),3);
  });
  self->delayed_send(self,std::chrono::seconds(3),3);
}
