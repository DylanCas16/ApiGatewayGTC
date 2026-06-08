import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import unary_pb2
import stream_pb2
import adapter_pb2


def convert_to_adapter(value):
    if isinstance(value, float):
        return adapter_pb2.AnyValue(type_kind=adapter_pb2.TYPE_FLOAT, float_val=value)
    elif isinstance(value, int):
        return adapter_pb2.AnyValue(type_kind=adapter_pb2.TYPE_LONG, long_val=value)
    elif isinstance(value, str):
        return adapter_pb2.AnyValue(type_kind=adapter_pb2.TYPE_STRING, string_val=value)
    elif isinstance(value, bool):
        return adapter_pb2.AnyValue(type_kind=adapter_pb2.TYPE_BOOLEAN, bool_val=value)
    else:
        raise TypeError(f"Tipo no soportado: {type(value)}")

def convertMonitorType(type):
    if type == "DataBlocks":
        return stream_pb2.DataBlocks
    elif type == "StateChanges":
        return stream_pb2.StateChanges
    elif type == "MagnitudeChanges":
        return stream_pb2.MagnitudeChanges
    else:
        return stream_pb2.Unknown

def test_listnaming(stub):
    print("Starting ListNaming test...")

    try:
        response = stub.ListNaming(unary_pb2.NamingRequest())
        devices = [(entry.name, entry.ior) for entry in response.entries]

        if devices:
            print("ListNaming success (devices found)")
            
            for name, ior in devices:
                print("Name: " + name)
                print("IOR: " + ior)
        else:
            print("ListNaming success (devices not found)")
    except grpc.RpcError as error:
        print(f"ListNaming test failed: ({error.code().name}) {error.details()}")
    
    return

def test_resolve(stub, component_name):
    print("Starting Resolve test...")

    try:
        request = unary_pb2.ResolveRequest(component_name=component_name)
        response = stub.Resolve(request)
        print(f"Resolve success ({len(response.methods)} methods found)")

        for method in response.methods:
            print(f"Name: {method.name} ")
            print(f"Return TypeCode: {adapter_pb2.TypeKind.Name(method.resultTypeCode)}")
            
            print(f"Params:")
            for param in method.params:
                print(f"    - {unary_pb2.Mode.Name(param.mode)} {param.name}: {adapter_pb2.TypeKind.Name(param.typeCode)}")
            
            print("")
    except grpc.RpcError as error:
        print(f"Resolve test failed: ({error.code().name}) {error.details()}")

    return

def test_invoke(stub, component_name, method_name, args=None):
    print("Starting Invoke test...")

    try:
        anyvalue_args = [convert_to_adapter(arg) for arg in (args or [])]
        request = unary_pb2.InvokeRequest(component_name=component_name,
                                            method_name=method_name, 
                                            args=anyvalue_args
        )
        response = stub.Invoke(request)
        
        print(f"Invoke success ({method_name})")
        print(f"Return value: {response.return_value}")
        if response.out_args:
            print(f"Out args: {list(response.out_args)}")
    except grpc.RpcError as error:
        print(f"Invoke test failed: ({error.code().name}) {error.details()}")
    
    return

def test_monitor(stub, component_name, magnitude, type, max_events=10):
    print("Starting Monitor subscription method...")

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
                print("ID: " + block.id)
                print("Component name: " + block.component_name)
                print("Magnitude: " + block.magnitude)
                print("Timestamp: " + block.time_stamp)
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
                print("Component name: " + state.component_name)
                print("Timestamp: " + state.time_stamp)
                print("New value: " + state.new_state)
                print("-----------------------------------")

            if data_type == "magnitude_change":
                mag_change = data.magnitude_change

                if last_ts is not None and mag_change.time_stamp < last_ts:
                    ts_errors += 1
                    ts_ok = False
                    print(f"  [!] Timestamp desincronizado: {mag_change.time_stamp} < {last_ts}")
                last_ts = mag_change.time_stamp

                print(f"MagnitudeChange received ({event_count:2d}/{max_events})")
                print("Component name: " + mag_change.component_name)
                print("Magnitude: " + mag_change.magnitude)
                print("Timestamp: " + mag_change.time_stamp)
                print("New value: " + mag_change.new_value)
                print("-----------------------------------")
            
            if event_count >= max_events:
                print(f"\nMax event number reached ({max_events} events). Closing stream...")
                response_stream.cancel()
                break
    except grpc.RpcError as e:
        if e.code() == grpc.StatusCode.CANCELLED:
            print("Stream closed by client.")
        else:
            print(f"Stream error: ({e.code().name}) {e.details()}")
    
    stream_stopped = False
    try:
        next(iter(response_stream))
        print("Closing failed: new data received.")
    except (grpc.RpcError, StopIteration):
        stream_stopped = True
        print("Stream stopped correctly: no more data received.")
    
    passed = event_count >= max_events and ts_errors == 0 and stream_stopped
    print(f"\nTEST RESULTS")
    print(f"  Events received: {event_count}/{max_events}  "
          f"{'PASS' if event_count >= max_events else 'FAIL'}")
    print(f"  Timestamps corrects: "
          f"{'PASS' if ts_errors == 0 else f'FAIL ({ts_errors} fails)'}")
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
    
    print("-------------Unary----------------")
    
    test_listnaming(stub)
    
    print("----------------------------------")
    
    test_resolve(stub, component_name)

    print("----------------------------------")
    
    test_invoke(stub, component_name, "start")

    print("----------------------------------")
    
    test_invoke(stub, component_name, "init")
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "voidcommand")
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "processFloat", [30.5])
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "processDouble", [293])

    print("----------------------------------")
    
    test_invoke(stub, component_name, "processShort", [5])

    print("----------------------------------")
    
    test_invoke(stub, component_name, "processLong", [70])

    print("----------------------------------")
    
    test_invoke(stub, "MonitorManagers/MM1", "getMagnitudes", component_name)

    print("------------Stream----------------")

    # test_monitor(stub, component_name, "doubleMagnitude", "DataBlocks")

    print("----------------------------------")

    channel.close()
    return


if __name__ == "__main__":
    main()