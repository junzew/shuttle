#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <sofa/pbrpc/pbrpc.h>
#include <gflags/gflags.h>
#include "logging.h"
#include "minion_impl.h"

using baidu::common::Log;
using baidu::common::FATAL;
using baidu::common::INFO;
using baidu::common::WARNING;

DECLARE_string(minion_port);

static volatile bool s_quit = false;
static void SignalIntHandler(int /*sig*/){
    s_quit = true;
}

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    baidu::shuttle::MinionImpl * minion = new baidu::shuttle::MinionImpl();
    sofa::pbrpc::RpcServerOptions options;
    sofa::pbrpc::RpcServer rpc_server(options);
    if (!rpc_server.RegisterService(static_cast<baidu::shuttle::Minion*>(minion))) {
        LOG(FATAL, "failed to register minion service");
        exit(-1);
    }

    std::string endpoint = "0.0.0.0:" + FLAGS_minion_port;
    if (!rpc_server.Start(endpoint)) {
        LOG(FATAL, "failed to start server on %s", endpoint.c_str());
        exit(-2);
    }
    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);
    LOG(INFO, "minion started.");
    while (!s_quit) {
        sleep(1);
    }
    return 0;
}