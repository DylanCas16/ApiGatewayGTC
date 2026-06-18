import sys
sys.path.insert(0, "generated")

import argparse
import grpc
import server_pb2_grpc
import unary_pb2
import adapter_pb2
import time


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

    anyvalue_args = [convert_to_adapter(arg) for arg in (args or [])]
    request = unary_pb2.InvokeRequest(component_name=component_name,
                                    method_name=method_name, 
                                    args=anyvalue_args
    )

    t0 = time.perf_counter()
    try:
        response = stub.Invoke(request)
        elapsed  = time.perf_counter() - t0
        
        print(f"Invoke success ({method_name})")
        print(f"Return value: {response.return_value}")
        if response.out_args:
            print(f"Out args: {list(response.out_args)}")
        print(f"Latency: {elapsed}")
        
    except grpc.RpcError as error:
        print(f"Invoke test failed: ({error.code().name}) {error.details()}")
    
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

    print("-------START UNARY TESTS----------")
    
    test_listnaming(stub)
    
    print("----------------------------------")
    
    test_resolve(stub, component_name)

    print("----------------------------------")
    
    test_invoke(stub, component_name, "start")

    print("----------------------------------")
    
    test_invoke(stub, component_name, "init")
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "ping")
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "processFloat", [30.5])

    channel.close()
    return


if __name__ == "__main__":
    main()