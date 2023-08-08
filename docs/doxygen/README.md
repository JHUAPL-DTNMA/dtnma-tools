<!--
Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
# Doxygen Documentation for Developers

To generate the full documentation simply run "doxygen" from this directory.


You will need the following to generate the complete documentation.  Only doxygen is required for text-only output:
- doxygen - Available from most package managers.  See http://doxygen.nl for more information
- graphviz - Available from most package managers.  Used to generate caller graphs.
- plantuml.jar - Used to generate additional UML diagrams.
  - Download from http://sourceforge.net/projects/plantuml/files/plantuml.jar/download into this directory.
  - Requires java.


## Files

- src - Documentation sources and resources included in Doxygen output
- output - After running doxygen, this directory will contain the output.
- Doxyfile - The Doxygen configuration file
