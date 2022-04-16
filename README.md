# GiJo-chat-server
A concurrent chat server program that implements a custom application layer protocol.

### Details:
- Implements an original application layer chat protocol called **CPT**
- CPT protocol documentation can be found in the **documentation/** folder at the project root
- CPT protocol comes with an API, found in the **cpt_api/** folder at the project root
- Full API documentation can be found in the **documentation/** folder at the project room


Application must be run on POSIX compliant machines.
To run, build to folder called "cmake-build-debug", then use (example):
```bash

# enable executable file permissions for shell script
sudo chmod +x ./run

# -- input params --
# -p -> port
# -h -> host ip
# -l -> login name
./run -l jordan -h 127.0.0.1 -p 8080
```