import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import stream_pb2
import adapter_pb2
import time


def test_config(stub, property_name, max_events=10):
    print("Starting Config subscription test...")

    request = stream_pb2.ConfigReq(property_name=property_name)

    event_count = 0
    response_stream = stub.SubscribeConfig(request)

    print("Subscribed. Waiting for events...\n")

    try:
        for data in response_stream:
            ts_received_ms = float(time.time() * 1000)
            latency_ms = ts_received_ms - data.gateway_ts

            event_count += 1

            print(f"Property change received ({event_count:2d}/{max_events})")
            print(f"CORBA callback to client latency: {latency_ms} ms")
            print("-----------------------------------")
            print(f"Config name: {data.config_name}")
            print(f"Class name: {data.class_name}")
            print(f"Component name: {data.component_name}")
            print(f"Timestamp: {data.time_stamp}")
            print("-----------------------------------")
            print()

            if event_count >= max_events:
                print(f"\nMax Config events reached ({max_events}). Closing stream...")
                response_stream.cancel()
                break

    except grpc.RpcError as e:
        if e.code() == grpc.StatusCode.CANCELLED:
            print("Config stream closed by client.")
        else:
            print(f"Config stream error: ({e.code().name}) {e.details()}")
    
    stream_stopped = False
    try:
        next(iter(response_stream))
        print("Closing failed: new data received.")
    except (grpc.RpcError, StopIteration):
        stream_stopped = True
        print("Stream stopped correctly.")

    passed = event_count >= max_events and stream_stopped
    print(f"\nTEST RESULTS")
    print(f"  Events received: {event_count}/{max_events} {'PASS' if event_count >= max_events else 'FAIL'}")
    print(f"  Stream closing: {'PASS' if stream_stopped else 'FAIL'}")
    print(f"  GLOBAL: {'PASS' if passed else 'FAIL'}")


def main():
    address = "localhost:50051"
    property_name = "Test/InspectorDevice_1.floatProperty"
    
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
    
    print("------------START CONFIG TEST----------------")

    test_config(stub, property_name)

    channel.close()
    return

if __name__ == "__main__":
    main()
