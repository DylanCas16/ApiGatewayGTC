# Arquitectura de la Api Gateway

## Diagramas finales

La arquitectura de la Api Gateway se puede visualizar mediante dos diagramas principales, los cuales muestra cómo se comunicarán los distintos componentes que conforman esta.

### Diagrama de componentes internos

A continuación, se mostrará el diagrama que muestra la estructura interna de la Api Gateway, incluyendo sus comunicaciones y propósitos de estas.

<img width="3871" height="8192" alt="InternalDiagram" src="https://github.com/user-attachments/assets/0dbf2eb5-a9b9-45ad-85b9-dc2ee7c2abd8" />

### Diagrama de hilos de las conexiones

Aquí se mostrará cómo se comunican los componentes a nivel de hilos, donde además se muestran los momentos donde se comunica con `gcs-env`.

<img width="3821" height="8192" alt="ThreadsDiagram" src="https://github.com/user-attachments/assets/9ba45ce4-e7d5-4588-8e57-a43c2dcaf3b8" />


## Mapeo de tipos IDL <-> C++/Protobuf

### Tipos escalares

La siguiente tabla empareja los tipos escalares que se encuentran en los `.idl` con su conversión en C++:

| IDL Type | Módulo | type_id char | C++ Type (cstdint) | Proto type | Notas |
|---|---|---|---|---|---|
| `double` | DGT | `'d'` | `double` | `double` | 64 bit |
| `float` | DGT | `'f'` | `float` | `float` | 32 bit |
| `short` | DGT | `'s'` | `int16_t` | `int32` | `CORBA::Short` (proto no tiene int16) |
| `long` | DGT | `'l'` | `int32_t` | `int32` | `CORBA::Long` |
| `unsigned short` | DGT | `'u'` | `uint16_t` | `uint32` | `CORBA::UShort` |
| `unsigned long` | DGT | `'w'` | `uint32_t` | `uint32` | `CORBA::ULong` |
| `octet` | DGT | `'o'` | `uint8_t` | `uint32` | `CORBA::Octet` ( o `bytes` en proto) |
| `boolean` | DGT | `'b'` | `bool` | `bool` | `CORBA::Boolean` |
| `string` | DGT | `'n'` | `std::string` | `string` | Desde `const char*` con operador `>>=` |
| `enum` | DGT | `'e'` | `enum class` | `enum` | El valor 0 suele ser `DOESNT_CARE` o `UNSPECIFIED` |
| `struct` | DGT | `'r'` | `struct` | `message` | Cada campo mapeado individualmente |
| `DGT::TimeValue` | DGT | `'t'` | `int64_t` | `int64` | Microsegundos (`usec`) |
| `DGT::State_t` | DGT | — | `std::string` | `string` | Campo `scoped_name`, usar `':'` como separador de subestados |
| `CORBA::Any` | CONFIG | — | `struct AnyValue` | `AnyValue` | Requiere introspección de TypeCode |


### Arrays con Dimension_t

Estos arrays están formados por unas dimensiones (height, width) y una lista de datos que cumplan con el tamaño definido en las dimensiones.
La siguiente tabla muestra los tipos que puede haber, donde hay que tener en cuenta que "dimensiones" se refiere a `uint32_t height` + `uint32_t width` (`uint32 height` + `uint32 width` en proto):

| IDL Type | type_id 1D | type_id 2D | C++/Proto Struct | Campos C++ |
|---|---|---|---|---|
| `DGT::DoubleArray` | `'D'` | `'9'` | `DoubleArray` | `std::vector<double> data` + dimensiones |
| `DGT::FloatArray` | `'F'` | `'8'` | `FloatArray` | `std::vector<float> data` + dimensiones |
| `DGT::LongArray` | `'L'` | `'7'` | `LongArray` | `std::vector<int32_t> data` + dimensiones |
| `DGT::ShortArray` | `'S'` | `'6'` | `ShortArray` | `std::vector<int16_t> data` + dimensiones |
| `DGT::ULongArray` | `'W'` | `'3'` | `ULongArray` | `std::vector<uint32_t> data` + dimensiones |
| `DGT::UShortArray` | `'U'` | `'4'` | `UShortArray` | `std::vector<uint16_t> data` + dimensiones |
| `DGT::OctetArray` | `'O'` | `'5'` | `OctetArray` / `bytes` | Para 1D usar `std::vector<uint8_t>` directamente, para 2D añadir dimensiones |


### MM::MultiTypeList

Se trata de una `union` de distintos "Sample", estos se pueden distinguir entre listas normales, arrays 1D y arrays 2D.
Un `SampleXxxx` está formado por dos campos: `DGT::TimeValue time_stamp` (`int64 time_stamp` en proto) y un valor según el tipo que sea (`double`, `float`, `long`, `short`, `octet`), siendo un `oneof` en proto.
En la siguiente tabla se puede apreciar todas las posibilidades:

#### Tabla simple

| Discriminador IDL | char | IDL branch | C++ branch | Proto oneof branch |
|---|---|---|---|---|
| `TYPE_SAMPLE_DOUBLE_LIST` | `'d'` | `SampleDoubleList d` | `MM::SampleDoubleList` (`sequence<SampleDouble>`) | `SampleDoubleList double_samples` |
| `TYPE_SAMPLE_FLOAT_LIST` | `'f'` | `SampleFloatList f` | `MM::SampleFloatList` (`sequence<SampleFloat>`) | `SampleFloatList float_samples` |
| `TYPE_SAMPLE_LONG_LIST` | `'l'` | `SampleLongList l` | `MM::SampleLongList` (`sequence<SampleLong>`) | `SampleLongList long_samples` |
| `TYPE_SAMPLE_SHORT_LIST` | `'s'` | `SampleShortList s` | `MM::SampleShortList` (`sequence<SampleShort>`) | `SampleShortList short_samples` |
| `TYPE_SAMPLE_OCTET_LIST` | `'o'` | `SampleOctetList o` | `MM::SampleOctetList` (`sequence<SampleOctet>`) | `SampleOctetList octet_samples` |

#### Tabla ARRAY1D

| Discriminador IDL | char | IDL branch | C++ branch | Proto oneof branch |
|---|---|---|---|---|
| `TYPE_SAMPLE_DOUBLE_ARRAY1D_LIST` | `'D'` | `SampleDoubleArrayList array_d` | `MM::SampleDoubleArrayList` (`sequence<SampleDoubleArray>`) | `SampleDoubleArrayList double_array_samples` |
| `TYPE_SAMPLE_FLOAT_ARRAY1D_LIST` | `'F'` | `SampleFloatArrayList array_f` | `MM::SampleFloatArrayList` (`sequence<SampleFloatArray>`) | `SampleFloatArrayList float_array_samples` |
| `TYPE_SAMPLE_LONG_ARRAY1D_LIST` | `'L'` | `SampleLongArrayList array_l` | `MM::SampleLongArrayList` (`sequence<SampleLongArray>`) | `SampleLongArrayList long_array_samples` |
| `TYPE_SAMPLE_SHORT_ARRAY1D_LIST` | `'S'` | `SampleShortArrayList array_s` | `MM::SampleShortArrayList` (`sequence<SampleShortArray>`) | `SampleShortArrayList short_array_samples` |
| `TYPE_SAMPLE_OCTET_ARRAY1D_LIST` | `'O'` | `SampleOctetArrayList array_o` | `MM::SampleOctetArrayList` (`sequence<SampleOctetArray>`) | `SampleOctetArrayList octet_array_samples` |

#### Tabla ARRAY2D

| Discriminador IDL | char | IDL branch | C++ branch | Proto oneof branch |
|---|---|---|---|---|
| `TYPE_SAMPLE_DOUBLE_ARRAY2D_LIST` | `'1'` | `SampleDoubleArrayList array_d` | (misma rama que 1D) | (misma rama que 1D) |
| `TYPE_SAMPLE_FLOAT_ARRAY2D_LIST` | `'2'` | `SampleFloatArrayList array_f` | (misma rama que 1D) | (misma rama que 1D) |
| `TYPE_SAMPLE_LONG_ARRAY2D_LIST` | `'3'` | `SampleLongArrayList array_l` | (misma rama que 1D) | (misma rama que 1D) |
| `TYPE_SAMPLE_SHORT_ARRAY2D_LIST` | `'4'` | `SampleShortArrayList array_s` | (misma rama que 1D) | (misma rama que 1D) |
| `TYPE_SAMPLE_OCTET_ARRAY2D_LIST` | `'5'` | `SampleOctetArrayList array_o` | (misma rama que 1D) | (misma rama que 1D) |


### CORBA::Any

Se trata de un tipo de variable que puede representar cualquier tipo, para solucionar este problema se hará uso de un switch que recoja los posibles casos.
La siguiente tabla muestra los posibles tipos, así como sus conversiones:

| `tc->kind()` | Tipo IDL | Extracción `CORBA::Any` | Inserción en `CORBA::Any` | C++ Type resultado |
|---|---|---|---|---|
| `CORBA::tk_double` | `double` | `any >>= v` | `any <<= v` | `CORBA::Double` |
| `CORBA::tk_float` | `float` | `any >>= v` | `any <<= v` | `CORBA::Float` |
| `CORBA::tk_long` | `long` | `any >>= v` | `any <<= v` | `CORBA::Long` |
| `CORBA::tk_short` | `short` | `any >>= v` | `any <<= v` | `CORBA::Short` |
| `CORBA::tk_boolean` | `boolean` | `any >>= CORBA::Any::to_boolean(v)` | `any <<= CORBA::Any::from_boolean(v)` | `CORBA::Boolean` |
| `CORBA::tk_string` | `string` | `any >>= v` | `any <<= v` | `const char*` |
| `CORBA::tk_octet` | `octet` | `any >>= CORBA::Any::to_octet(v)` | `any <<= CORBA::Any::from_octet(v)` | `CORBA::Octet` |
| `CORBA::tk_struct` | `struct` | `any >>= v` (operador del tipo concreto) | `any <<= v` (operador del tipo concreto) | Tipo del stub, ej: `DGT::TimeValue` |
| `default` | desconocido | — | — | Sin extracción, ignorar o pasar como `bytes` |

Dentro de los ficheros proto, su implementación pued ser como un `message` que incluya dos campos: el tipo de valor y un `oneof` para todas sus posibilidades.


### Reglas de serialización DII

Para la implementación de DII, es necesario seguir el siguiente flujo:

_request() -> add_in_arg() (nº argumentos) -> set_return_type() -> invoke() -> return_value()

#### Tabla de tipos de parámetros

| Dirección IDL | Método DII | Ejemplo C++ |
|---|---|---|
| `in` escalar | `add_in_arg()` | `req->add_in_arg() <<= (CORBA::Long)42;` |
| `in` string | `add_in_arg()` | `req->add_in_arg() <<= "texto";` |
| `in` struct | `add_in_arg()` | `req->add_in_arg() <<= my_struct;` |
| `out` | `add_out_arg()` | Se lee tras `invoke()` con `>>=` |
| `inout` | `add_inout_arg()` | Se inicializa antes, se lee después |
| valor de retorno | `set_return_type()` | **Obligatorio** aunque sea `void` |

#### Tabla de los tipos para `set_return_type()`

| Tipo de retorno IDL | TypeCode a pasar |
|---|---|
| `void` | `CORBA::_tc_void` |
| `long` | `CORBA::_tc_long` |
| `double` | `CORBA::_tc_double` |
| `float` | `CORBA::_tc_float` |
| `short` | `CORBA::_tc_short` |
| `boolean` | `CORBA::_tc_boolean` |
| `string` | `CORBA::_tc_string` |
| `octet` | `CORBA::_tc_octet` |
| struct/sequence | `TypeCode del IFR` | Obtener con `InterfaceDef::describe()` |


### Tipos especiales de gcs

Hay varios tipos de estructuras claves que no han sido mencionadas ya que no son tan atómicas, pero que son imprescindibles para la implementación de la API.
La siguiente tabla muestra estas estructuras:

| Tipo IDL | Módulo | Observación | Proto equivalente |
|---|---|---|---|
| `DGT::StringList` | DGT | `sequence<string>` | `repeated string` |
| `DGT::TimeValue` | DGT | `long long usec` | `int64` |
| `DGT::TimeInterval` | DGT | `{begin, end}` en usec | `message TimeInterval { int64 begin=1; int64 end=2; }` |
| `DGT::State_t` | DGT | `string scoped_name` con `':'` | `string` |
| `DGT::GCSException` | DGT | `{name, text, stackTrace}` | `google.rpc.Status` o `message GCSError` |
| `DGT::Array` | DGT | tipo genérico con `type_id` + `OctetList` | `AnyValue` con `bytes raw_val` |
| `CONFIG::Property` | CONFIG | `{name, any value}` | `message Property { string name=1; AnyValue value=2; }` |
| `MM::DataBlock` | MM | contiene `MultiTypeList` | `message DataBlockEvent` con `MultiTypeList` |
| `ALARM::Event` | ALARM | snapshot de cambio de estado | `message AlarmEvent` |
| `LOG::Record` | LOG | entrada de log con tipo y source | `message LogEvent` |
