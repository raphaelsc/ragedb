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

#ifndef RAGEDB_NODEPROPERTIES_H
#define RAGEDB_NODEPROPERTIES_H

#include <Graph.h>
#include <seastar/http/function_handlers.hh>
#include <seastar/http/httpd.hh>
#include <seastar/http/json_path.hh>

using namespace seastar;
using namespace httpd;
using namespace ragedb;

class NodeProperties {
    class GetNodePropertyHandler : public httpd::handler_base {
    public:
        explicit GetNodePropertyHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class GetNodePropertyByIdHandler : public httpd::handler_base {
    public:
        explicit GetNodePropertyByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PutNodePropertyHandler : public httpd::handler_base {
    public:
        explicit PutNodePropertyHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PutNodePropertyByIdHandler : public httpd::handler_base {
    public:
        explicit PutNodePropertyByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class DeleteNodePropertyHandler : public httpd::handler_base {
    public:
        explicit DeleteNodePropertyHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class DeleteNodePropertyByIdHandler : public httpd::handler_base {
    public:
        explicit DeleteNodePropertyByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class GetNodePropertiesHandler : public httpd::handler_base {
    public:
        explicit GetNodePropertiesHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class GetNodePropertiesByIdHandler : public httpd::handler_base {
    public:
        explicit GetNodePropertiesByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PostNodePropertiesHandler : public httpd::handler_base {
    public:
        explicit PostNodePropertiesHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PostNodePropertiesByIdHandler : public httpd::handler_base {
    public:
        explicit PostNodePropertiesByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PutNodePropertiesHandler : public httpd::handler_base {
    public:
        explicit PutNodePropertiesHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class PutNodePropertiesByIdHandler : public httpd::handler_base {
    public:
        explicit PutNodePropertiesByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class DeleteNodePropertiesHandler : public httpd::handler_base {
    public:
        explicit DeleteNodePropertiesHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

    class DeleteNodePropertiesByIdHandler : public httpd::handler_base {
    public:
        explicit DeleteNodePropertiesByIdHandler(NodeProperties& nodes) : parent(nodes) {};
    private:
        NodeProperties& parent;
        future<std::unique_ptr<reply>> handle(const sstring& path, std::unique_ptr<request> req, std::unique_ptr<reply> rep) override;
    };

private:
    Graph& graph;
    GetNodePropertyHandler getNodePropertyHandler;
    GetNodePropertyByIdHandler getNodePropertyByIdHandler;
    PutNodePropertyHandler putNodePropertyHandler;
    PutNodePropertyByIdHandler putNodePropertyByIdHandler;
    DeleteNodePropertyHandler deleteNodePropertyHandler;
    DeleteNodePropertyByIdHandler deleteNodePropertyByIdHandler;

    GetNodePropertiesHandler getNodePropertiesHandler;
    GetNodePropertiesByIdHandler getNodePropertiesByIdHandler;
    PostNodePropertiesHandler putNodePropertiesHandler;
    PutNodePropertiesByIdHandler putNodePropertiesByIdHandler;
    PutNodePropertiesHandler postNodePropertiesHandler;
    PostNodePropertiesByIdHandler postNodePropertiesByIdHandler;
    DeleteNodePropertiesHandler deleteNodePropertiesHandler;
    DeleteNodePropertiesByIdHandler deleteNodePropertiesByIdHandler;

public:
    explicit NodeProperties(Graph &_graph) : graph(_graph), getNodePropertyHandler(*this), getNodePropertyByIdHandler(*this),
                                            putNodePropertyHandler(*this), putNodePropertyByIdHandler(*this),
                                            deleteNodePropertyHandler(*this), deleteNodePropertyByIdHandler(*this),
                                            getNodePropertiesHandler(*this), getNodePropertiesByIdHandler(*this),
                                            putNodePropertiesHandler(*this), putNodePropertiesByIdHandler(*this),
                                            postNodePropertiesHandler(*this), postNodePropertiesByIdHandler(*this),
                                            deleteNodePropertiesHandler(*this), deleteNodePropertiesByIdHandler(*this) {}
    void set_routes(routes& routes);
};


#endif //RAGEDB_NODEPROPERTIES_H
