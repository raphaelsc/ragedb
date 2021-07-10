/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Relationship.h"
#include "Shard.h"

namespace ragedb {

    Relationship::Relationship(uint64_t id,
                               uint64_t startingNodeId,
                               uint64_t endingNodeId) : id(id),
                               starting_node_id(startingNodeId),
                               ending_node_id(endingNodeId) {}

    uint64_t Relationship::getId() const {
        return id;
    }

    uint16_t Relationship::getTypeId() const {
        return Shard::externalToTypeId(id);
    }

    uint64_t Relationship::getStartingNodeId() const {
        return starting_node_id;
    }

    uint64_t Relationship::getEndingNodeId() const {
        return ending_node_id;
    }

}