import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import stream_pb2
import adapter_pb2


def convertMonitorType(type):
    if type == "DataBlocks":
        return stream_pb2.DataBlocks
    elif type == "StateChanges":
        return stream_pb2.StateChanges
    elif type == "MagnitudeChanges":
        return stream_pb2.MagnitudeChanges
    else:
        return stream_pb2.Unknown



def test_monitor(stub, component_name, magnitude, type, max_events=10):
    print("Starting Monitor subscription test...")

    request = stream_pb2.MonitorReq(component_name=component_name,
                                    magnitude=magnitude,
                                    type=convertMonitorType(type)
    )

    event_count = 0
    last_ts     = None
    ts_errors   = 0

    response_stream = stub.SubscribeMonitor(request)

    print("Subscribed. Waiting for events...\n")

    try:
        for data in response_stream:
            data_type = data.WhichOneof("event")
            event_count += 1
            ts_ok = True
 
            if data_type == "data_block":
                block = data.data_block

                if last_ts is not None and block.time_stamp < last_ts:
                    ts_errors += 1
                    ts_ok = False
                    print(f"  [!] Timestamp desynchronized: {block.time_stamp} < {last_ts}")
                last_ts = block.time_stamp

                print(f"DataBlock received ({event_count:2d}/{max_events})")
                print(f"ID: {block.id}")
                print(f"Component name: {block.component_name}")
                print(f"Magnitude: {block.magnitude}")
                print(f"Timestamp: {block.time_stamp}")
                print(f"Samples: {block.samples}")
                print("-----------------------------------")
            
            if data_type == "state_change":
                state = data.state_change

                if last_ts is not None and state.time_stamp < last_ts:
                    ts_errors += 1
                    ts_ok = False
                    print(f"  [!] Timestamp desincronizado: {state.time_stamp} < {last_ts}")
                last_ts = state.time_stamp

                print(f"StateChange received ({event_count:2d}/{max_events})")
                print(f"Component name: {state.component_name}")
                print(f"Timestamp: {state.time_stamp}")
                print(f"New value: {state.new_state}")
                print("-----------------------------------")

            if data_type == "magnitude_change":
                mag_change = data.magnitude_change

                if last_ts is not None and mag_change.time_stamp < last_ts:
                    ts_errors += 1
                    ts_ok = False
                    print(f"  [!] Timestamp desincronizado: {mag_change.time_stamp} < {last_ts}")
                last_ts = mag_change.time_stamp

                print(f"MagnitudeChange received ({event_count:2d}/{max_events})")
                print(f"Component name: {mag_change.component_name}")
                print(f"Magnitude: {mag_change.magnitude}")
                print(f"Timestamp: {mag_change.time_stamp}")
                print(f"New value: {mag_change.new_value}")
                print("-----------------------------------")
            
            if event_count >= max_events:
                print(f"\nMax Monitor events reached ({max_events} events). Closing stream...")
                response_stream.cancel()
                break
    except grpc.RpcError as e:
        if e.code() == grpc.StatusCode.CANCELLED:
            print("Monitor stream closed by client.")
        else:
            print(f"Monitor stream error: ({e.code().name}) {e.details()}")
    
    stream_stopped = False
    try:
        next(iter(response_stream))
        print("Closing failed: new data received.")
    except (grpc.RpcError, StopIteration):
        stream_stopped = True
        print("Stream stopped correctly: no more data received.")
    
    passed = event_count >= max_events and ts_errors == 0 and stream_stopped
    print(f"\nTEST RESULTS")
    print(f"  Events received: {event_count}/{max_events} {'PASS' if event_count >= max_events else 'FAIL'}")
    print(f"  Timestamps corrects: {'PASS' if ts_errors == 0 else f'FAIL ({ts_errors} fails)'}")
    print(f"  Stream closing: {'PASS' if stream_stopped else 'FAIL'}")
    print(f"  GLOBAL: {'PASS' if passed else 'FAIL'}")

    return

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

    print("------------START MONITOR TEST----------------")

    test_monitor(stub, component_name, "floatMonitor1", "DataBlocks")

    channel.close()
    return


if __name__ == "__main__":
    main()