/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_ROUTING_TESTS_CONTAINERS_COMPARISON_H_
#define MAIDSAFE_ROUTING_TESTS_CONTAINERS_COMPARISON_H_

#include <cstdint>
#include <vector>
#include <string>

#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/filesystem/path.hpp"

#include "maidsafe/common/rsa.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/bootstrap_file_operations.h"
#include "maidsafe/routing/node_info.h"
#include "maidsafe/routing/routing_table.h"
#include "maidsafe/routing/utils.h"


namespace maidsafe {

namespace routing {

namespace test {

const uint32_t kLifeInSeconds = 1; // 5 min
const uint32_t kHistoryCleanupFactor = 100;

struct ProcessedEntry {
  ProcessedEntry(const NodeId& source_in, int32_t messsage_id_in)
      : source(source_in), message_id(messsage_id_in), expiry_time(std::time(NULL)) {}
  NodeId source;
  int32_t message_id;
  std::time_t expiry_time;
};

bool operator > (const ProcessedEntry& lhs, const ProcessedEntry& rhs);

template <typename ContainerType>
class Container {
 public:
  Container() : container_() {}
  bool Add(const NodeId& source, int32_t message_id);

 private:
  void Remove(std::unique_lock<std::mutex>& lock);
  std::mutex mutex_;
  ContainerType container_;
};

template <typename ContainerType>
bool Container<ContainerType>::Add(const NodeId& /*source*/, int32_t /*message_id*/) {
  return ContainerType::Initialise_please;
}

template <typename ContainerType>
void Container<ContainerType>::Remove(std::unique_lock<std::mutex>& /*lock*/) {
  ContainerType::Initialise_please;
}

template <>
void Container<std::vector<ProcessedEntry>>::Remove(std::unique_lock<std::mutex>& lock);

template <>
bool Container<std::vector<ProcessedEntry>>::Add(const NodeId& source, int32_t message_id);

template <>
void Container<std::set<ProcessedEntry>>::Remove(std::unique_lock<std::mutex>& lock);

template <>
bool Container<std::set<ProcessedEntry>>::Add(const NodeId& source, int32_t message_id);

}  // namespace test

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_TESTS_CONTAINERS_COMPARISON_H_

