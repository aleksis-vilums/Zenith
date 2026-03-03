import grpc
import zenith_pb2
import zenith_pb2_grpc

from river import drift

def run():
    channel = grpc.insecure_channel("zenith-server:50051")
    stub = zenith_pb2_grpc.ZenithStub(channel)

    request = zenith_pb2.TickRequest(tick="AAPL")
    helloRequest = zenith_pb2.HelloRequest(name="Aleks")

    response = stub.SayHello(helloRequest)

    adwin = drift.ADWIN()
    for tick in stub.GenerateTicks(request):
        print(
            f"Symbol: {tick.tick} | "
            f"Price: {tick.price} | "
            f"Timestamp: {tick.date}"
        )
        adwin.update(tick.price)

        if adwin.drift_detected:
            print(f"Change detected at time {tick.date}, Price: {tick.price}")

if __name__ == "__main__":
    run()