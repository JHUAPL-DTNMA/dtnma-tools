from typing import Any

import schemathesis
from cbor_diag import cbor2diag, diag2cbor


@schemathesis.serializer("text/uri-list")
def urilist_ser(ctx: schemathesis.SerializationContext, value: Any) -> bytes:
    if isinstance(value, bytes):
        return value

    return str(value).encode()


@schemathesis.deserializer("text/uri-list")
def urilist_des(
    ctx: schemathesis.DeserializationContext, resp: schemathesis.Response
) -> list[str]:
    text = resp.content.decode()
    return text.split("\r\n")


@schemathesis.serializer("application/cbor")
def cbor_ser(ctx: schemathesis.SerializationContext, value: Any) -> bytes:
    if isinstance(value, bytes):
        return value

    try:
        return diag2cbor(str(value))
    except ValueError:
        return b''


@schemathesis.deserializer("application/cbor")
def cbor_des(
    ctx: schemathesis.DeserializationContext, resp: schemathesis.Response
) -> list[str]:
    text = cbor2diag(resp.content)
    return text
