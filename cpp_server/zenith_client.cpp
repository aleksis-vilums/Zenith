#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "zenith.grpc.pb.h"

ABSL_FLAG(std::string, target, "zenith-server:50051", "Server address");

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using zenith::Zenith;
using zenith::HelloReply;
using zenith::HelloRequest;
using zenith::TickRequest;
using zenith::TickResponse;

class ZenithClient {
 public:
  ZenithClient(std::shared_ptr<Channel> channel)
      : stub_(Zenith::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  void GenerateTicks(const std::string& tick){
    TickRequest request;
    request.set_tick(tick);

    ClientContext context;

    auto reader = stub_->GenerateTicks(&context, request);
    TickResponse reply;

    while (reader->Read(&reply)){
      std::cout << "Symbol: " << reply.tick() << " | Price: " << reply.price() << " | Date (YYYYMMDD) " << reply.date() << std::endl;
    }

    Status status = reader->Finish();
    if (!status.ok()){
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }

 private:
  std::unique_ptr<Zenith::Stub> stub_;
};

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  std::string target_str = absl::GetFlag(FLAGS_target);
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  ZenithClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = greeter.SayHello(user);
  std::cout << "Greeter received: " << reply << std::endl;

  std::string tick("NFLX");
  greeter.GenerateTicks(tick);
  return 0;
}