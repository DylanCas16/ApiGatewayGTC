import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import time
import server_pb2_grpc
import config_pb2
import adapter_pb2


def config_invoke(stub, times=5, delay=2.0):
    property_name = "Test/InspectorDevice_1.floatProperty"

    for i in range(times):
        try:
            current_property = stub.GetProperty(config_pb2.GetPropRequest(
                 property_name=property_name
            ))

            current_value = current_property.property.value.float_val
            print(f"Property 'floatProperty' = {current_value} ({i+1}/{times})")

            new_property = config_pb2.Property(
                name=property_name,
                value=adapter_pb2.AnyValue(
                    type_kind=adapter_pb2.TYPE_FLOAT,
                    float_val=current_value + i
                )
            )

            response = stub.SetProperty(config_pb2.SetPropRequest(
                property=new_property
            ))
            if response.done:
                print(f"Property 'floatProperty' changed = {new_property.value.float_val} ({i+1}/{times})")
            else:
                print(f"Property 'floatProperty' not changed ({i+1}/{times})")

            if i < times - 1:
                print(f"  Waiting {delay}s...")
                time.sleep(delay)

        except grpc.RpcError as error:
            print(f"Config method failed: ({error.code().name}) {error.details()}")
    
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
    
    config_invoke(stub)

    channel.close()
    return

if __name__ == "__main__":
    main()