/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/


#include <utility>
#include "boost/signals2.hpp"
#include "boost/asio/deadline_timer.hpp"
#include "boost/date_time.hpp"
#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/routing/timer.h"
#include "maidsafe/routing/routing.pb.h"
#include "maidsafe/routing/log.h"

namespace maidsafe {

namespace routing {

maidsafe::routing::Timer::Timer(AsioService &io_service) :
   io_service_(io_service),
  task_id_(RandomUint32()),
  queue_() {}
// below comment would require an overload or default here to
// put in another task with the same task_id
uint32_t Timer::AddTask(uint32_t timeout,
                        const TaskResponseFunctor &response_functor) {
  ++task_id_;
  TimerPointer timer(new asio::deadline_timer(io_service_.service(),
                                      boost::posix_time::seconds(timeout)));
  timer->async_wait(std::bind(&Timer::KillTask, this, task_id_));
  queue_.insert(std::make_pair(task_id_, std::make_pair(timer,
                                                        response_functor)));
  return task_id_;
}
// TODO(dirvine)
// we could change the find to iterate entire map if we want to be able to send
// multiple requests and accept the first one back, dropping the rest.
void Timer::KillTask(uint32_t task_id) {
  auto it = queue_.find(task_id);
  if (it != queue_.end()) {
    // message timed out or task killed
     (*it).second.second(1, "");
    queue_.erase(it);
  }else {
    DLOG(ERROR) << "Attempt to kill an expired or non existent task";
  }
}

void Timer::ExecuteTaskNow(protobuf::Message &message) {
    auto it = queue_.find(message.id());
  if (it != queue_.end()) {
    // message all OK in routing
    (*it).second.second(0, message.data());
    queue_.erase(it);
  } else {
    DLOG(ERROR) << "Attempt to run an expired or non existent task";
  }
}


}  // namespace maidsafe

}  // namespace routing