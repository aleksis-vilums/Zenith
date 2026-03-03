#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <random>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include "zenith.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using zenith::Zenith;
using zenith::HelloReply;
using zenith::HelloRequest;
using zenith::TickRequest;
using zenith::TickResponse;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

// Logic and data behind the server's behavior.
class ZenithServiceImpl final : public Zenith::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                    HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
  Status GenerateTicks(ServerContext* context, const TickRequest* request,
                        ServerWriter<TickResponse>* writer) {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> dist(-0.5, 0.5);

    double price = 100.0;

    for (int i = 0; i < 1000; ++i){
      TickResponse response;
      if (i > 740 && i < 800){
        price = 125.0 + dist(generator);
      } else {
        price += dist(generator);
      }
      response.set_tick(request->tick());
      response.set_price(price);
      response.set_date(std::time(nullptr));

      writer->Write(response);

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return Status::OK;
  }
};

void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("[::]:%d", port);
  ZenithServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
