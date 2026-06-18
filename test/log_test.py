import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import stream_pb2
import adapter_pb2


def test_log(stub, component, host_ip_filter=None, type_filter=None, timestamp_range=None, max_events=10):
    print("Starting log stream test...")

    request = stream_pb2.LogReq(component_name=component)

    event_count = 0
    skipped = 0
    response_stream = stub.SubscribeLogs(request)

    print("Subscribed. Waiting for events...\n")

    try:
        for data in response_stream:
            if host_ip_filter is not None and data.host_ip_address != host_ip_filter:
                skipped +=1
                continue
            if type_filter is not None and data.type != type_filter:
                skipped += 1
                continue
            if timestamp_range is not None and not (timestamp_range[0] <= data.time_stamp <= timestamp_range[1]):
                skipped += 1
                continue

            event_count += 1

            print(f"Log Message received ({event_count:2d}/{max_events})")
            print(f"Component name: {data.component_name}")
            print(f"Host IP: {data.host_ip_address}")
            print(f"Source: {data.source_code}")
            print(f"Type: {data.type}")
            print(f"Timestamp: {data.time_stamp}")
            print(f"Message data: {data.msg_data}")
            print("-----------------------------------")

            if event_count >= max_events:
                print(f"\nMax Log events reached ({max_events}). Closing stream...")
                response_stream.cancel()
                break
    
    except grpc.RpcError as e:
        if e.code() == grpc.StatusCode.CANCELLED:
            print("Log stream closed by client.")
        else:
            print(f"Log stream error: ({e.code().name}) {e.details()}")
    
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
    print(f"  Events skipped: {skipped}")
    print(f"  Stream closing: {'PASS' if stream_stopped else 'FAIL'}")
    print(f"  GLOBAL: {'PASS' if passed else 'FAIL'}")


def main():
    address = "localhost:50051"
    component_name = "Test/InspectorDevice_1"
    
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

    print("------------START LOG TEST----------------")

    test_log(stub, component_name)

    channel.close()
    return


if __name__ == "__main__":
    main()