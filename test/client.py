import sys
import argparse
import grpc
import unary_pb2
import unary_pb2_grpc
import adapter_pb2


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
            print(f"    - {unary_pb2.Mode.Name(param.mode)} {param.name}: 
                    {adapter_pb2.TypeKind.Name(param.typeCode)}")
        
        print("")
    except grpc.RpcError as error:
        print(f"Resolve test failed: ({error.code().name}) {error.details()}")

    return

def test_invoke(stub, component_name, method_name, args=None):
    print("Starting Invoke test...")

    try:
        request = unary_pb2.InvokeRequest(component_name=component_name,
                                            method_name=method_name, args=args or [])
        response = stub.Invoke(request)
        
        print(f"Invoke success ({method_name})")
        print(f"Return value: {response.return_value}")
        if response.out_args:
            print(f"Out args: {list(response.out_args)}")
    except grpc.RpcError as error:
        print(f"Invoke test failed: ({error.code().name}) {error.details()}")
    
    return


def main():
    address = "localhost:50051"
    component_name = "test/InspectorDevice_1"
    
    print("Connecting to " + address)

    channel = grpc.insecure_channel(address)
    stub = unary_pb2_grpc.GatewayStub(channel)

    try:
        grpc.channel_ready_future(channel).result(timeout=5)
        print("Connected to server")
    except grpc.FutureTimeoutError:
        print("Error: could not connect to server: " + address)
        channel.close()
        return
    
    test_listnaming(stub)
    
    print("----------------------------------")
    
    test_resolve(stub, component_name)
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "voidcommand")
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "proccessFloat", [30.5])
    
    print("----------------------------------")
    
    test_invoke(stub, component_name, "proccessString", ["Hello World"])

    channel.close()
    return


if __name__ == "__main__":
    main()