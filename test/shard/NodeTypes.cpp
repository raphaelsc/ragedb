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

#include <catch2/catch.hpp>
#include "../../src/graph/Shard.h"

SCENARIO( "Shard can handle Node Types", "[node_types]" ) {
    GIVEN("An empty shard") {
        ragedb::Shard shard(4);

        WHEN("we get the node types") {
            THEN("it should be empty") {
                std::set<std::string> types = shard.NodeTypesGet();
                REQUIRE(types.empty());
            }
        }

        WHEN("we get the count of node types") {
            THEN("it should be zero") {
                uint64_t count = shard.NodeTypesGetCount();
                REQUIRE(count == 0);
            }
        }

        WHEN("we get the type id and type") {
            THEN("it should have the empty id and type") {
                uint16_t type_id = shard.NodeTypeGetTypeId("NOT_THERE");
                std::string type = shard.NodeTypeGetType(99);
                REQUIRE(type_id == 0);
                REQUIRE(type.empty());
            }
        }

        WHEN("we add a node type") {
            shard.NodeTypeInsert("Person", 1);

            THEN("it should be one") {
                uint64_t count = shard.NodeTypesGetCount();
                REQUIRE(count == 1);
            }

            THEN("it should be in the set") {
                std::set<std::string> types = shard.NodeTypesGet();
                REQUIRE(types.size() == 1);
                REQUIRE(types == std::set<std::string>({"Person"}));
            }

            THEN("it should have the correct id and type") {
                uint16_t type_id = shard.NodeTypeGetTypeId("Person");
                std::string type = shard.NodeTypeGetType(1);
                REQUIRE(type_id == 1);
                REQUIRE(type == "Person");
            }

            THEN("it should be unique and retain original id") {
                shard.NodeTypeInsert("Person", 2);
                uint16_t type_id = shard.NodeTypeGetTypeId("Person");
                std::string type = shard.NodeTypeGetType(1);
                std::string invalid = shard.NodeTypeGetType(2);
                REQUIRE(type_id == 1);
                REQUIRE(type == "Person");
                REQUIRE(invalid.empty());
            }
        }

        WHEN("we add two node types") {
            shard.NodeTypeInsert("Person", 1);
            shard.NodeTypeInsert("User", 2);

            THEN("it should be two") {
                uint64_t count = shard.NodeTypesGetCount();
                REQUIRE(count == 2);
            }
        }
    }

    GIVEN("A shard with three nodes") {
        ragedb::Shard shard(4);
        shard.NodeTypeInsert("Node", 1);
        uint64_t node1id = shard.NodeAddEmpty(1, "one");
        uint64_t node2id = shard.NodeAddEmpty(1, "two");
        uint64_t node3id = shard.NodeAddEmpty(1, "three");

        WHEN("we get the count of an invalid node type") {
            THEN("it should be zero") {
                uint64_t count = shard.NodeTypesGetCount("DOES_NOT_EXIST");
                REQUIRE(count == 0);
            }
        }

        WHEN("we get the count of an invalid node type id") {
            THEN("it should be zero") {
                uint64_t count = shard.NodeTypesGetCount(99);
                REQUIRE(count == 0);
            }
        }

        WHEN("add a node") {
            shard.NodeTypeInsert("Person", 2);
            shard.NodeAddEmpty( 2, "four");

            THEN("request the count of the node type") {
                uint64_t count_by_id = shard.NodeTypesGetCount(2);
                uint64_t count_by_type = shard.NodeTypesGetCount("Person");
                REQUIRE(count_by_id == 1);
                REQUIRE(count_by_type == 1);
            }
        }

        WHEN("add new nodes") {
            shard.NodeTypeInsert("User", 2);
            shard.NodeTypeInsert("Thing", 3);
            shard.NodeAddEmpty(2, "some user");
            shard.NodeAddEmpty(3, "some thing");
            shard.NodeAddEmpty(3, "some thing else");

            THEN("request the count of the node types") {
                uint64_t user_count = shard.NodeTypesGetCount("User");
                uint64_t user_count_by_id = shard.NodeTypesGetCount(2);
                uint64_t thing_count = shard.NodeTypesGetCount("Thing");
                uint64_t thing_count_by_id = shard.NodeTypesGetCount(3);
                REQUIRE(user_count == 1);
                REQUIRE(user_count_by_id == 1);
                REQUIRE(thing_count == 2);
                REQUIRE(thing_count_by_id == 2);
            }
        }

        WHEN("we try to insert a type that already exists ") {
            shard.NodeTypeInsert("Node", 1);

            THEN("it should ignore it") {
                uint64_t node_count = shard.NodeTypesGetCount("Node");
                uint64_t node_count_by_id = shard.NodeTypesGetCount(1);
                REQUIRE(node_count == 3);
                REQUIRE(node_count_by_id == 3);

                REQUIRE(node1id == shard.NodeGetID("Node", "one"));
                REQUIRE(node2id == shard.NodeGetID("Node", "two"));
                REQUIRE(node3id == shard.NodeGetID("Node", "three"));
            }
        }
    }

}