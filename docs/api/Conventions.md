<!--
Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
Laboratory LLC.

This file is part of the Delay-Tolerant Networking Management
Architecture (DTNMA) Tools package.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

This page contains conventions necessary for DTNMA developers and helpful for library users to understand.

# Documentation

All top-level public API must have inline Doxygen comment blocks (_e.g._, `/** docs */`) preferably within the same header in which the API is declared.
For reference, Doxygen comment blocks can contain complex markup based on a large set of available [commands](https://www.doxygen.nl/manual/commands.html).

## Macro-Expanded Container Declarations

When M*LIB macros are used to declare type-safe containers, the Doxygen inspection of the macro-expanded code should be inhibited but there should also be a explicit Doxygen block to provide explanation of the purpose of the struct and a reference to the type of its contents.

An example of this is below (corresponding to @ref ari_list_t).

@verbatim
/** @struct ari_list_t
 * An [M-I-LIST](https://github.com/P-p-H-d/mlib/blob/master/README.md#m-i-list)
 * of ::ari_t items.
 */
/// @cond Doxygen_Suppress
M_DEQUE_DEF(ari_list, ari_t)
/// @endcond
@endverbatim

## Citations and References

For definitions from rfc's and other sources, `@cite [source]` in the Doxygen header.
If possible (e.g. for RFCs) include the document number in the text for convenience, as in ```RFC XXXX @cite rfcXXXX```.

# File Naming

Within the source tree, all library contents are under the `src` directory and all unit tests are under the `test` directory.
Within the `src` directory the tree structure of all header files is the same as when the library is installed to the host filesystem.

File names follow CFE-style conventions.

The top of each file should contain a Doxygen block for the file itself, including an association with a specific module. An example of this is below.

@verbatim
/** @file
 * @ingroup frontend
 * Brief summary.
 * Detailed description follows.
 */
@endverbatim

# Symbol Naming

Symbol names follow the C99 convention of lowercase names with underscore separators.

All public API prefixed with the library name (_e.g._ `cace_`) to provide a virtual 'namespace' to the API.

All public API functions should follow the general `[noun]_[verb]` convention for naming.
More generally, public API functions should follow the pattern `BLS_[StructureContext]_[VerbPhraseFunctionName]`

When a set of functions are related to a struct, they should have the same noun prefix as the struct name (_e.g._, the `struct example_s` with typedef `example_t` should have corresponding functions named like `example_XYZ()`).
Beyond the common naming, functions related to a struct should be indicated using the `@memberof` Doxygen command.
An example of this is below.

@verbatim
/** Brief summary.
 * Detailed description follows.
 */
typedef struct cace_example_s{
  ...
} cace_example_t;

/** Brief summary.
 * Detailed description follows.
 * @memberof cace_example_t
 *
 * ... @param and @return ...
 */
int cace_example_dothing(cace_example_t *obj, ...);
@endverbatim

# Error Reporting and Handling

Functions that cannot fail should have `void` return type.
Functions that can fail should have `int` return type and use the following common values:

0 - Success   
1 - Invalid parameter (unless otherwise noted)
NOTE!! This pattern is being adapted. A negative value indicates error, zero indicates succes.
There may be times when there may be meaningful context associated with a positive value (e.g., number of bytes written).

# Structs and Functions

Much of the public API involves state kept in a struct with associated functions to inspect and manipulate the state of that struct.

Generally, users of the API should not access struct members directly.
But specific documentation on each struct will define its specific public API.

## Initialization <name>\_init() and <name>\_init\_<form>()

All structs must have an associated initialization function.
Members of the struct cannot be accessed before its initialization and functions called on the struct will have undefined behavior.

## De-initialization <name>\_deinit()

All structs must have an associated de-initialization function.
After its de-initialization the members of the struct will no longer have well defined state.

To help with troubleshooting, de-initialization should set pointers set to NULL and other values to a well-defined state. One option is to use `memset()` to zeroize the entire struct.

# Macros

This section contains references to commonly used macros defined for the libraries

## Memory Management Macros

When heap memory is needed at runtime, the following macros are used and have the same signature and semantics as the corresponding C99 functions indicated below.

- [ARI_MALLOC](@ref ARI_MALLOC) as `malloc()`
- [ARI_REALLOC](@ref ARI_REALLOC) as `realloc()`
- [ARI_FREE](@ref ARI_FREE) as `free()`

## Error Checking Handler Macros

To help with the error reporting conventions above, the following macros can be used to simplify function precondition checking.
The precondition checks (on function parameters or on any other state generally) should be the first thing inside the function definition.

- [CHKRET(cond, val)](@ref CHKRET) for general error values
- [CHKNULL(cond)](@ref CHKNULL) when the function has a pointer return type
- [CHKERR1(cond)](@ref CHKERR1) when the function has an `int` return type
- [CHKVOID(cond)](@ref CHKVOID) when the function has an `void` return type
- [CHKFALSE(cond)](@ref CHKFALSE) when the function has an `bool` return type

# Enums

Enums with explicit values must be justified with citations, for example the declarations of @ref ari_type_t.
Otherwise, they should not be given values.

Enums should be `typedef`-ed with a `_e` suffix.
Enum values should be full `SCREAMING_CASE` matching the name of the struct.
@verbatim
typedef enum {
  CACE_MYENUM_OPTION1 = 1,
  CACE_MYENUM_OPTION2,
  ...
} cace_myenum_e;
@endverbatim

# Unit Testing

Conventions for unit testing using the [Unity](https://github.com/ThrowTheSwitch/Unity) testing library are:

- The name of the test source file should be the same name as the unit-being-tested prefixed by `test_`.
- Where possible, name of the test functions should be the name of the function prefixed by `test_` and suffixed by the test condition, either `_valid` or `_invalid` or similar.

