@page cace_ari ARI Transcoder Tool
<!--
Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

This tool converts ARIs between different encoding forms.
It uses command-line options to control the ARI conversion.

The options related to input or output forms allow the following values:
 * The form "uri" meaning a newline-separated, URI-encoded form of ARIs consistent with the "application/uri-list" media type and Section 9.2 of ARI @cite draft-ietf-dtn-ari. For backward compatibility, there is a "text" form which is an alias for the "uri" form.
 * The form "cbor" meaning a sequence of CBOR-encoded form of ARIs consistent with the "application/cbor-seq" media type and Section 9.2 of ARI @cite draft-ietf-dtn-ari.
 * The form "cborhex" meaning a newline-separated, base16-encoded, CBOR-encoded form of ARIs consistent with the "text/plain" media type and Section 9.2 of ARI @cite draft-ietf-dtn-ari.

For "uri" or "cborhex" forms of input, each line is handled as a separate ARI and converted independently until the input stream is ended.
For "cbor" form of input, the stream is treated as a CBOR sequence @cite rfc8742 and each CBOR data item is converted independently until the input stream is ended.

```
usage: cace_ari [--help,-h]
                [--log-level,-l {debug,info,warning,error,crit}]
                [--inform,-i {auto,uri,cbor,cborhex}] [--source,-s {filename or -}]
                [--outform,-o {uri,cbor,cborhex}] [--dest,-d {filename or -}]
```

# Named Options

For all options of this command, if any option is supplied multiple times the last use supersedes all others.

- `--log-level`

  Possible choices: debug, info, warning, error, crit

  The minimum log severity.

  Default: `info`

- `--inform`

  Possible choices: auto, uri, cbor, cborhex

  The input encoding.

  The choice of 'auto' uses the initial bytes of the first input to detect the scheme "ari:" to indicate if the input is 'uri'.
  Otherwise, it is assumed to be 'cborhex' and processed accordingly.

  Default: `auto`

- `--source`

  The input file name to read or "-" for stdin stream.

  Default: `-`

- `--outform`

  Possible choices: auto, uri, cbor, cborhex

  The output encoding.

  The choice of 'auto' assumes a 'uri' output if the input is 'cbor' or 'cborhex' and assumes a 'cborhex' output if the input is 'uri'.

- `--dest`

  The output file name to write or "-" for stdout stream.

  Default: `-`
