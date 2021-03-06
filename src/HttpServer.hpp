/*
 * Copyright 2015, akashche at redhat.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   HttpServer.hpp
 * Author: akashche
 *
 * Created on July 10, 2015, 7:23 PM
 */

#ifndef JVMTI_HTTP_HTTPSERVER_HPP
#define	JVMTI_HTTP_HTTPSERVER_HPP

#include <string>
#include <atomic>
#include <memory>
#include <cstdlib>

#include <jvmti.h>

#include "staticlib/containers/blocking_queue.hpp"
#include "staticlib/httpserver/http_server.hpp"
#include "staticlib/httpserver/http_response_writer.hpp"

#include "ZipResource.hpp"
#include "JvmtiAccessor.hpp"

namespace jvmti_http {

namespace detail {

class Query {
    staticlib::httpserver::http_response_writer_ptr writer;
    std::string property;

public:
    Query(const Query&) = delete;
    Query& operator=(const Query&) = delete;

    Query(Query&& other) :
    writer(std::move(other.writer)),
    property(std::move(other.property)) { }

    Query& operator=(Query&& other) {
        this->writer = std::move(other.writer);
        this->property = std::move(other.property);
        return *this;
    }

    Query() { }

    Query(staticlib::httpserver::http_response_writer_ptr writer, std::string property) :
    writer(std::move(writer)),
    property(std::move(property)) { }

    staticlib::httpserver::http_response_writer_ptr& get_writer() {
        return writer;
    }

    const std::string& get_property() {
        return property;
    }
};

}

class HttpServer {
    std::unique_ptr<JvmtiAccessor> ja;
    staticlib::containers::blocking_queue<detail::Query> queue;    
    staticlib::httpserver::http_server server;
    ZipResource webapp_resource;
#ifdef STATICLIB_WINDOWS
    std::atomic_flag running;
#else
    std::atomic_flag running = ATOMIC_FLAG_INIT;
#endif
    
public:
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    
    HttpServer(uint16_t port, JvmtiAccessor* ja, const std::string& webapp_zip_path,
            const std::string& cert_path);
    
    static void JNICALL jvmti_callback(jvmtiEnv* jvmti, JNIEnv* jni, void* user_data);
    
    void shutdown();
    
private:
    
    void read_from_queue(jvmtiEnv* jvmti, JNIEnv* jni);
};

}

#endif	/* JVMTI_HTTP_HTTPSERVER_HPP */

