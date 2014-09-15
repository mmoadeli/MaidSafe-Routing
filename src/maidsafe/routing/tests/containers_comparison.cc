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

#include "maidsafe/routing/tests/containers_comparison.h"

#include "boost/progress.hpp"

namespace maidsafe {

namespace routing {

namespace test {

bool operator < (const ProcessedEntry& lhs, const ProcessedEntry& rhs) {
  return (lhs.message_id == rhs.message_id) ? lhs.source < rhs.source
                                            : lhs.message_id < rhs.message_id;
}

template <>
bool Container<std::vector<ProcessedEntry>>::Add(const NodeId& source, int32_t message_id) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (std::any_of(std::begin(container_), std::end(container_),
                  [&](const ProcessedEntry& entry) {
                    return message_id == entry.message_id &&
                           source == entry.source;
                  }))
    return false;

  container_.push_back(ProcessedEntry(source, message_id));
//  std::cout << container_.size() << std::endl;
  if (container_.size() % kHistoryCleanupFactor == 0)
    Remove(lock);

  return true;
}

template <>
void Container<std::vector<ProcessedEntry>>::Remove(std::unique_lock<std::mutex>& lock) {
  assert(lock.owns_lock());
  static_cast<void>(lock);
  ProcessedEntry dummy(NodeId(NodeId::IdType::kRandomId), RandomInt32());
  auto upper(std::upper_bound(std::begin(container_), std::end(container_), dummy,
                              [](const ProcessedEntry& lhs, const ProcessedEntry& rhs) {
                                return (lhs.expiry_time - rhs.expiry_time < kLifeInSeconds);
                              }));
  if (upper == std::end(container_)) {
    std::cout << "Nothing to remove \n";
  } else {
//    std::cout << "distance " << std::distance(std::begin(container_), upper) << std::endl;
    container_.erase(std::begin(container_), upper);
//    std::cout << "vector size after delete " << container_.size() << std::endl;
  }
}

template <>
bool Container<std::set<ProcessedEntry>>::Add(const NodeId& source, int32_t message_id) {
  std::unique_lock<std::mutex> lock(mutex_);

  auto entry(ProcessedEntry(source, message_id));
  auto found(container_.find(entry));
  if (found != std::end(container_))
    return false;

  container_.insert(entry);
//  std::cout << container_.size() << std::endl;
  if (container_.size() % kHistoryCleanupFactor == 0)
    Remove(lock);

  return true;
}

template <>
void Container<std::set<ProcessedEntry>>::Remove(std::unique_lock<std::mutex>& lock) {
  assert(lock.owns_lock());
  static_cast<void>(lock);
  std::time_t now(std::time(NULL));
  for (auto iter(std::begin(container_)); iter != std::end(container_);) {
    if (now - iter->expiry_time > kLifeInSeconds)
      iter = container_.erase(iter);
    else
      iter++;
  }
//  std::cout << "set size after delete " << container_.size() << std::endl;
}
 
template <>
bool Container<ProcessedEntrySet>::Add(const NodeId& source, int32_t message_id) {
  std::unique_lock<std::mutex> lock(mutex_);
 
  auto entry(ProcessedEntry(source, message_id));
  auto found(container_.find(entry));
  if (found != std::end(container_))
    return false;
 
  container_.insert(entry);
  //  std::cout << container_.size() << std::endl;
  if (container_.size() % kHistoryCleanupFactor == 0)
    Remove(lock);
 
  return true;
}
 
template <>
void Container<ProcessedEntrySet>::Remove(std::unique_lock<std::mutex>& lock) {
  assert(lock.owns_lock());
  static_cast<void>(lock);
  ProcessedEntrySet::nth_index<1>::type& sorted_birth(container_.get<1>());
  ProcessedEntry dummy(NodeId(NodeId::IdType::kRandomId), RandomInt32());
  auto upper(std::upper_bound(std::begin(sorted_birth), std::end(sorted_birth), dummy,
                              [](const ProcessedEntry& lhs, const ProcessedEntry& rhs) {
                                return (lhs.expiry_time - rhs.expiry_time < kLifeInSeconds);
                              }));
  if (upper == std::end(sorted_birth)) {
    std::cout << "Nothing to remove \n";
  } else {
   //    std::cout << "distance " << std::distance(std::begin(container_), upper) << std::endl;
   sorted_birth.erase(std::begin(sorted_birth), upper);
  //    std::cout << "vector size after delete " << container_.size() << std::endl;
 }
}

TEST(ContainerComparison, BEH_Vector) {
  Container<std::vector<ProcessedEntry>> container;
  std::vector<int> message_sizes {100000, 500000, 1000000, 2000000, 4000000, 8000000};
  std::vector<int> life_spans {1, 2, 4, 8};
  std::vector<int> clean_up_factors {100, 500, 1000, 2000, 50000};
  for (size_t life_span_index(0); life_span_index < life_spans.size(); ++life_span_index) {
    kLifeInSeconds = life_spans.at(life_span_index);
    for (size_t clean_up_index(0); clean_up_index < clean_up_factors.size(); ++clean_up_index) {
      kHistoryCleanupFactor = clean_up_factors.at(clean_up_index);
      for (size_t message_index(0); message_index < message_sizes.size(); ++message_index) {
        boost::progress_timer t;
        for (int message_size(0); message_size < message_sizes.at(message_index); ++message_size)
          container.Add(NodeId(NodeId::IdType::kRandomId), RandomInt32());
        std::cout << kLifeInSeconds <<  ", " << kHistoryCleanupFactor << ", "
                  << message_sizes.at(message_index) << ", elapsed time: " << t.elapsed();
      }
    }
  }
}
 
TEST(ContainerComparison, BEH_Set) {
  Container<std::set<ProcessedEntry>> container;
  std::vector<int> message_sizes {100000, 500000, 1000000, 2000000, 4000000, 8000000};
  std::vector<int> life_spans {1, 2, 4, 8};
  std::vector<int> clean_up_factors {100, 500, 1000, 2000, 50000};
  for (size_t life_span_index(0); life_span_index < life_spans.size(); ++life_span_index) {
    kLifeInSeconds = life_spans.at(life_span_index);
    for (size_t clean_up_index(0); clean_up_index < clean_up_factors.size(); ++clean_up_index) {
      kHistoryCleanupFactor = clean_up_factors.at(clean_up_index);
      for (size_t message_index(0); message_index < message_sizes.size(); ++message_index) {
        boost::progress_timer t;
        for (int message_size(0); message_size < message_sizes.at(message_index); ++message_size)
          container.Add(NodeId(NodeId::IdType::kRandomId), RandomInt32());
        std::cout << kLifeInSeconds <<  ", " << kHistoryCleanupFactor << ", "
                  << message_sizes.at(message_index) << ", elapsed time: " << t.elapsed();
      }
    }
  }
}

TEST(ContainerComparison, BEH_MultiIndex) {
  Container<ProcessedEntrySet> container;
  std::vector<int> message_sizes {100000, 500000, 1000000, 2000000, 4000000, 8000000};
  std::vector<int> life_spans {1, 2, 4, 8};
  std::vector<int> clean_up_factors {100, 500, 1000, 2000, 50000};
  for (size_t life_span_index(0); life_span_index < life_spans.size(); ++life_span_index) {
    kLifeInSeconds = life_spans.at(life_span_index);
    for (size_t clean_up_index(0); clean_up_index < clean_up_factors.size(); ++clean_up_index) {
      kHistoryCleanupFactor = clean_up_factors.at(clean_up_index);
      for (size_t message_index(0); message_index < message_sizes.size(); ++message_index) {
        boost::progress_timer t;
        for (int message_size(0); message_size < message_sizes.at(message_index); ++message_size)
          container.Add(NodeId(NodeId::IdType::kRandomId), RandomInt32());
        std::cout << kLifeInSeconds <<  ", " << kHistoryCleanupFactor << ", "
                  << message_sizes.at(message_index) << ", elapsed time: " << t.elapsed();
      }
    }
  }
}

}  // namespace test

}  // namespace routing

}  // namespace maidsafe


