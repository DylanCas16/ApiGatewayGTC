import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import stream_pb2
import adapter_pb2
import time


def test_alarm(stub, component, alarm_filter=None, state_filter=None, severity_filter=None, max_events=10):
    print("Starting Alarm subscription test...")

    request = stream_pb2.AlarmReq(component_name=component)

    event_count = 0
    skipped = 0
    response_stream = stub.SubscribeAlarms(request)

    print("Subscribed. Waiting for events...\n")

    try:
        for data in response_stream:
            ts_received_ms = float(time.time() * 1000)
            latency_ms = ts_received_ms - data.gateway_ts

            if alarm_filter is not None and data.alarm_name != alarm_filter:
                skipped +=1
                continue
            if state_filter is not None and data.state != state_filter:
                skipped += 1
                continue
            if severity_filter is not None and data.severity != severity_filter:
                skipped += 1
                continue

            event_count += 1

            print(f"Alarm received ({event_count:2d}/{max_events})")
            print(f"CORBA callback to client latency: {latency_ms} ms")
            print("-----------------------------------")
            print(f"Alarm name: {data.alarm_name}")
            print(f"Component name: {data.component_name}")
            print(f"State: {data.state}")
            print(f"Severity: {data.severity}")
            print(f"Description: {data.description}")
            print(f"Timestamp: {data.time_stamp}")
            print(f"ID: {data.id}")
            print("-----------------------------------")
            print()

            if event_count >= max_events:
                print(f"\nMax Alarm events reached ({max_events}). Closing stream...")
                response_stream.cancel()
                break

    except grpc.RpcError as e:
        if e.code() == grpc.StatusCode.CANCELLED:
            print("Alarm stream closed by client.")
        else:
            print(f"Alarm stream error: ({e.code().name}) {e.details()}")
    
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
    
    print("------------START ALARM TEST----------------")

    test_alarm(stub, component_name)

    channel.close()
    return


if __name__ == "__main__":
    main()