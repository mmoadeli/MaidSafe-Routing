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

#include <set>
#include <vector>

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/global_fun.hpp"
#include "boost/multi_index/mem_fun.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/identity.hpp"

#include "maidsafe/common/test.h"
#include "maidsafe/common/node_id.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace routing {

namespace test {

uint32_t kLifeInSeconds = 2; // 5 min
uint32_t kHistoryCleanupFactor = 1000;

struct ProcessedEntry {
  ProcessedEntry(const NodeId& source_in, int32_t messsage_id_in)
      : source(source_in), message_id(messsage_id_in), expiry_time(std::time(NULL)) {}
  ProcessedEntry Key() const { return *this; }
  std::time_t BirthTime() const { return expiry_time; }
  NodeId source;
  int32_t message_id;
  std::time_t expiry_time;
};

bool operator > (const ProcessedEntry& lhs, const ProcessedEntry& rhs);
 
typedef boost::multi_index_container<
  ProcessedEntry,
  boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<boost::multi_index::identity<ProcessedEntry>
  >,
  boost::multi_index::ordered_non_unique<
      BOOST_MULTI_INDEX_CONST_MEM_FUN(ProcessedEntry, std::time_t, BirthTime)>
  >
> ProcessedEntrySet;
 

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
bool Container<std::set<ProcessedEntry>>::Add(const NodeId& source, int32_t message_id);
 
template <>
void Container<std::set<ProcessedEntry>>::Remove(std::unique_lock<std::mutex>& lock);

template <>
bool Container<ProcessedEntrySet>::Add(const NodeId& source, int32_t message_id);
 
template <>
void Container<ProcessedEntrySet>::Remove(std::unique_lock<std::mutex>& lock);


}  // namespace test

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_TESTS_CONTAINERS_COMPARISON_H_

