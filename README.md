# Test task for NTechLab

Write HTTP server that horizontally mirrors jpeg images. Input image is sent in POST request's body, output image must also be sent in response body

## Build

- Requirements: C++ compiler with C++20 support, Linux, CMake, make, bash
- Run ``prepare.sh`` script from repository root to download and build libraries. It'll take some time, around 5 minutes
- Enter ``server`` directory and run ``cmake . && make`` to build server

- If your C++ compiler does not support C++20 or something else bad happened you can build and run it in docker: 
  - Repo must be clean, so remove ``third_party`` directory and all artifacts in ``server`` directory or simply clone repo again
  - Build docker image: ``docker build . --tag ${YOUR_IMAGE_TAG}``
  - Run container: `` docker run -p8080:8080 ${YOUR_IMAGE_TAG}``

## Usage
    
Run ``ntechlab_testtask`` to start mirroring server. Image mirror endpoint is available at ``/mirror``. To stop server press ``Ctrl+C`` 

Command line options: 
  
  - pass ``-p`` option to set port number (8080 by default)
  - pass ``-b`` option to set ipv4 address (0.0.0.0 by default)
  - pass ``-j`` option to set number of threads (1 by default)
  - pass ``-i`` option to use interactive mode. (Off by default) Visit ``0.0.0.0:8080/interactive`` in your browser when turned on. Its important to run the server from ``server`` directory when setting this option because of relative paths. Port number and ip address must have their default values.

