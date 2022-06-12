# Supervisor for C++ Actor Framework (CAF)

<div align="center">
<img src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/crash-me-if-you-can.png" alt><br>
<em>License: CC-BY-NC-ND by Fred Hebert (www.learnyousomeerlang.com)</em>
</div>

<br>

This project shall demonstrate the capabilities of the C++ Actor Framework (CAF)
on the example of a lightweight C++ supervisor process that monitors another lightweight 
C++ process and restarts it in case of an error. This shall resemble the capabilities
of an Erlang Supervisor - which has different supervising strategies (e.g. one_for_one) 
and can be used to span complex and distributed supervision hierarchies (called applications).

## Build & Run

### Step A: Build and Download external dependencies:

$ git clone https://github.com/weberdaniel/supervisor-demo <br>
$ cd supervisor-demo <br>
$ mkdir build <br>
$ cd build <br>
$ cmake .. <br> 
$ make -j 24 # in case of 24 cpu cores <br>

### Step B: Repeat to create executables:

$ cmake .. <br> 
$ make -j 24 # in case of 24 cpu cores <br>

### Step C: Run:

$ cd .. <br>
$ ./build/app/svdemo/svdemo --config-file=./app/svdemo/caf-application.conf

### Build & Run in Docker Container

T.B.D.

## Actor Frameworks and Reactive Design

Actor Frameworks are a crucial building block for Artificial Intelligence. Actor Frameworks have been first introduced by 
Carl Hewitt, Peter Bishop und Richard Steiger in 1973.

### The C++ Actor Framework

<table>
<tbody>
<tr>
<td width="33%"> <div align="center"><img src="https://www.actor-framework.org/static/img/caf-logo.png" width="200"> </div> </td>
<td> A notable modern implementation of an Actor Framework in C++ is CAF, the C++ Actor Framework (https://www.actor-framework.org). In contrary to programming middleware like CORBA or ACE, it uses modern C++ to improve memory management, performance and type safety. </td>
</tr>
</tbody>
</table>

Some highlights:

- leverage on C++ high performance
- low memory footprint
- native bindings to Prometheus DB for high-performance data science in real-time
- Resilience due to Reactive Design
- Scalability
- Tight memory control
- modern c++
- used for high-performance network analysis (Zeek)
- Actors blend into Qt Widgets seamlessly by deriving from caf::mixin::actor_widget<QWdiget> 

### Reactive Software Design: Make your software fault tolerant!
  
<table>
<tbody>
<tr>
<td width="33%"> <img align='left' src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/reactive_design.jpg" width="100%">  </td>
<td> The principle of reactive software design patterns reaches back to the 1970s. A famous example of a reactive software design is the AXD301 Ericsson switch, which provides a nine-nines reliability (99.9999999%), which is basically a millisecond of downtime per year. Since highly distributed software architectures are common - reactive design patterns have been rediscovered and become relevant on hardware related software domains as well. Examples for this are the IoT. But also tech companies aim for a decentralized fault-tolerant data center technology. For example Microsoft is testing out underwater distributed data centers. Multi-core CPUs can be regarded as distributed systems as well. Therefore the design principles of devices like the Ericsson switch AXD301 are getting rediscovered and become implemented on other programming domains as well. Since today any kind of software can be regarded as a part of a distributed architecture, reactive design patterns can be applied to almost any software that should be fault-tolerant and highly reliable. This is reflected in the creation of the Reactive Manifesto in order to spread word about the design concepts: https://www.reactivemanifesto.org
(Book Cover: Reactive Design Patterns, Roland Kuhn)  </td>
</tr>
</tbody>
</table>

### An example design pattern: "Crash me if you can"
  
The 'Let it crash'/'Crash me if you can' pattern is one of the elementary design patterns of the domain. In order to build fault-tolerant software, a common design pattern is to  only code for the applcation usecase - in any other usecase - the process will just crash and be restarted. The result looks like the following:

<br><br>
<div align="center">
<img src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/restart.png" width="50%" alt><br>
<em>Child process is being restarted by the supervisor</em>
</div>
<br>
  
The Erlang specification of a supervisor can be found here:
https://www.erlang.org/doc/man/supervisor.html
  
#### One for One Strategy
  
<br><br>
<div align="center">
<img src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/sup-one_for_one.png" width="50%" alt style="fill: blue"><br>
</div>
  
#### One for All Strategy
<br><br>
<div align="center">
<img src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/sup-one_for_all.png" width="50%" alt><br>
</div>
  
#### Rest for One Strategy
<br><br>
<div align="center">
<img src="https://github.com/weberdaniel/supervisor-demo/raw/main/doc/images/sup-rest_for_one.png" width="50%" alt><br>
</div>


## Aims and Goals

The goal of this project is to implement an Erlang supervision hierarchy inside a C++ high performance application, as closly as possible resembling the principles of Erlang, but in the domain of high performance computing. 
  
The ultimate Goal actually would be to create something like the Erlang/OTP for CAF, like the <b> CAF-Open-Telecom-Platform CAF/OTP </b>.
  
## Contribution  

In SCRUM you usually find a DEFINITION OF DONE, telling what is expected of the code quality. In this case there is no scrum team, but still a Definition of Done is used as far as possible (Reviews are not always possible)

### The Definition of Done:
  
- Fullfillment of Google Coding Style (cpplint must give 0 errors)
- C++ Core Guidelines must be kept : https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
- Another Person must review the code
- Unit Test must be > 80%
- Memory Check with Valgrind
- Code Coverage > 80%

