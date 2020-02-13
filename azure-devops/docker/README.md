# Use Docker images to bring your own C++ toolset

The Dockerfile in this directory creates an image containing all the tools to build the MSVC's STL library. It is inspired by the [Dockerfile](https://docs.microsoft.com/en-us/visualstudio/install/build-tools-container?view=vs-2019) provided an example by the official Microsoft documentation.

The image is based on Windows kernel version 1803, which as of October 2019 is the same version of the Microsoft hosted agents, so the same image can be used in both Microsoft hosted and self hosted (a.k.a. private) build agents.

The Docker file creates an image that contains:

 - most recent preview of MSVC toolset for x86, x64, ARM, ARM64, installed from the official Microsoft servers;
 - tools such as Git and CMake installed with Choco package manager;

## How to run the image in a command prompt

Get the id of the image out of the container on the agent:

 >docker images

Get a command prompt running in the image with:

 >docker run &lt;id&gt; -i cmd

## How to attach an additional data disk on an Azure VM running on Windows Core

The standard disk size of an Azure VM running Windows Core is 30Gb only.
When used as a build agent, it is needed to add a bigger (e.g. 1TB) data disk to store and cache docker images. The following Powershell command helps to attach a new uninitialized disk,:

```powershell
   PS>Get-Disk | Where partitionstyle -eq 'raw' |
      Initialize-Disk -PartitionStyle MBR -PassThru |
     New-Partition -AssignDriveLetter -UseMaximumSize |
     Format-Volume -FileSystem NTFS -NewFileSystemLabel "myDataDisk" -Confirm:$false
```
