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

#include <iostream>
#include "Shard.h"

namespace ragedb {

    static const unsigned int SHARD_BITS = 10U;
    static const unsigned int SHARD_MASK = 0x00000000000003FFU;
    static const unsigned int TYPE_BITS = 16U;
    static const unsigned int TYPE_MASK = 0x0000000003FFFFFFU;
    static const unsigned int SIXTY_FOUR = 64U;

    /**
     * Stop the Shard - Required method by Seastar for peering_sharded_service
     *
     * @return future
     */
    seastar::future<> Shard::stop() {
        std::stringstream ss;
        ss << "Stopping Shard " << seastar::this_shard_id() << '\n';
        std::cout << ss.str();
        return seastar::make_ready_future<>();
    }

    /**
     * Empty the Shard of all data
     */
    void Shard::Clear() {

    }

    seastar::future<std::string> Shard::HealthCheck() {
        std::stringstream message;
        message << "Shard " << seastar::this_shard_id() << " is OK";
        return seastar::make_ready_future<std::string>(message.str());
    }

    seastar::future<std::vector<std::string>> Shard::HealthCheckPeered() {
        return container().map([](Shard &local_shard) {
            return local_shard.HealthCheck();
        });
    }

    // Ids =================================================================================================================================

    // 64 bits:  10 bits for core id (1024) 16 bits for the type (65536) 38 bits for the id (274877906944)
    uint64_t Shard::internalToExternal(uint16_t type_id, uint64_t internal_id) const {
        return (((internal_id << TYPE_BITS) + type_id) << SHARD_BITS) + shard_id;
    }

    uint64_t Shard::externalToInternal(uint64_t id) {
        return (id >> (TYPE_BITS + SHARD_BITS));
    }

    uint16_t Shard::externalToTypeId(uint64_t id) {
        return (id >> SHARD_BITS) & TYPE_MASK;
    }

    uint16_t Shard::CalculateShardId(uint64_t id) {
        if(id < SHARD_MASK) {
            return 0;
        }
        return id & SHARD_MASK;
    }

    uint16_t Shard::CalculateShardId(const std::string &type, const std::string &key) const {
        // We need to find where the node goes, so we use the type and key to create a 64 bit number
        uint64_t x64 = std::hash<std::string>()((type + '-' + key));

        // Then we bucket it into a shard depending on the number of cpus we have
        return (uint16_t)(((__uint128_t)x64 * (__uint128_t)cpus) >> SIXTY_FOUR);
    }

    // *****************************************************************************************************************************
    //                                               Single Shard
    // *****************************************************************************************************************************

    // Node Type ============================================================================================================================
    std::string Shard::NodeTypeGetType(uint16_t type_id) {
        return node_types.getType(type_id);
    }

    uint16_t Shard::NodeTypeGetTypeId(const std::string &type) {
        return node_types.getTypeId(type);
    }

    bool Shard::NodeTypeInsert(const std::string& type, uint16_t type_id) {
        tsl::sparse_map<std::string, uint64_t> empty;
        node_keys.emplace(type, empty);
        return node_types.addTypeId(type, type_id);
    }

    // Nodes ================================================================================================================================
    uint64_t Shard::NodeAddEmpty(const std::string& type, uint16_t node_type, const std::string &key) {
        uint64_t internal_id = node_types.getCount(node_type);
        uint64_t external_id = 0;

        auto type_search = node_keys.find(type);
        // The Label will always exist
        if (type_search != std::end(node_keys)) {
            // Check if the key exists
            auto key_search = type_search->second.find(key);
            if (key_search == std::end(type_search->second)) {
                // If we have deleted nodes, fill in the space by adding the new node here
                if (node_types.getDeletedIds(node_type).isEmpty()) {
                    external_id = internalToExternal(node_type, internal_id);
                    // Set Metadata properties
                    // Add the node to the end and prepare a place for its relationships
                    node_types.getNodes(node_type).emplace_back(external_id, key);
                    node_types.getOutgoingRelationships(node_type).emplace_back();
                    node_types.getIncomingRelationships(node_type).emplace_back();
                    node_types.addId(node_type, internal_id);
                    //TODO: Making node_type ids all internal ids
                } else {
                    internal_id = node_types.getDeletedIds(node_type).minimum();
                    external_id = internalToExternal(node_type, internal_id);
                    // Set Metadata properties
                    Node node(external_id, key);
                    // Replace the deleted node and remove it from the list
                    node_types.getNodes(node_type).at(internal_id) = node;
                    node_types.addId(node_type, internal_id);
                }
                type_search->second.insert({ key, external_id });
            }
        }

        return external_id;
    }

    // *****************************************************************************************************************************
    //                                               Peered
    // *****************************************************************************************************************************

    // Node Type ===========================================================================================================================
    std::string Shard::NodeTypeGetTypePeered(uint16_t type_id) {
        return node_types.getType(type_id);
    }

    uint16_t Shard::NodeTypeGetTypeIdPeered(const std::string &type) {
        return node_types.getTypeId(type);
    }

    seastar::future<uint16_t> Shard::NodeTypeInsertPeered(const std::string &type) {
        uint16_t node_type_id = node_types.getTypeId(type);
        if (node_type_id == 0) {
            // node_type_id is global so unfortunately we need to lock here
            this->node_type_lock.for_write().lock().get();
            // The node type was not found and must therefore be new, add it to all shards.
            node_type_id = node_types.insertOrGetTypeId(type);
            return container().invoke_on_all([type, node_type_id](Shard &local_shard) {
                        local_shard.NodeTypeInsert(type, node_type_id);
                    })
                    .then([node_type_id, this] {
                        this->node_type_lock.for_write().unlock();
                        return seastar::make_ready_future<uint16_t>(node_type_id);
                    });
        }

        return seastar::make_ready_future<uint16_t>(node_type_id);
    }

    // Nodes ===============================================================================================================================

    seastar::future<uint64_t> Shard::NodeAddEmptyPeered(const std::string &type, const std::string &key) {
        uint16_t node_shard_id = CalculateShardId(type, key);
        uint16_t node_type_id = node_types.getTypeId(type);

        // The node type exists, so continue on
        if (node_type_id > 0) {
            return container().invoke_on(node_shard_id, [type, node_type_id, key](Shard &local_shard) {
                return local_shard.NodeAddEmpty(type, node_type_id, key);
            });
        }

        // The node type needs to be set by Shard 0 and propagated
        return container().invoke_on(0, [node_shard_id, type, key, this] (Shard &local_shard) {
            return local_shard.NodeTypeInsertPeered(type).then([node_shard_id, type, key, this] (uint16_t node_type_id) {
                return container().invoke_on(node_shard_id, [type, node_type_id, key](Shard &local_shard) {
                    return local_shard.NodeAddEmpty(type, node_type_id, key);
                });
            });
        });

    }

}