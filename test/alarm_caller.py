import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import time
import server_pb2_grpc
import unary_pb2
import adapter_pb2


def generate_alarm(stub, times=10, delay=2.0):
    component_name = "Test/InspectorDevice_1"

    for i in range(times):
        try:
            arg = [adapter_pb2.AnyValue(type_kind=adapter_pb2.TYPE_SHORT, short_val=1)]
            stub.Invoke(unary_pb2.InvokeRequest(
                component_name=component_name,
                method_name="sendAlarm", 
                args=arg
            ))
            print(f"sendAlarm call {i+1}/{times}")

            stub.Invoke(unary_pb2.InvokeRequest(
                component_name=component_name,
                method_name="cancelAlarm",
                args=arg
            ))
            print(f"cancelAlarm call {i+1}/{times}")

            if i < times - 1:
                print(f"  Waiting {delay}s...")
                time.sleep(delay)

        except grpc.RpcError as error:
            print(f"Invoke method failed: ({error.code().name}) {error.details()}")
    
    return

def main():
    address = "localhost:50051"
    
    print("Connecting to " + address)

    channel = grpc.insecure_channel(address)
    stub = server_pb2_grpc.GatewayServerStub(channel)

    try:
        grpc.channel_ready_future(channel).result(timeout=5)
        print("Connected to server")
    except grpc.FutureTimeoutError:
        print("Error: could not connect to server: " + address)
        channel.close()
        return
    
    generate_alarm(stub)

    channel.close()
    return

if __name__ == "__main__":
    main()
