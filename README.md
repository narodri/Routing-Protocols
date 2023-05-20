# Routing-Protocols

(0) How to compile

g++ -o distvec distvec.cc 

g++ -o linkstate linkstate.cc 



(1) Link State Rounting

./linkstate topologyfile messagesfile changesfile 

ex)./linkstate topology.txt messages.txt changes.txt 



(2) Distance Vector Routing

./distvec topologyfile messagesfile changesfile 

ex)./distvec topology.txt messages.txt changes.txt 



(3) Topology File

The first line shows the total number of nodes on the network.

From the next line, there is information about the link on the network.

ex)

5 

0 1 8 

1 2 3 

1 4 4 

3 0 1 

3 4 1 

- Here, '0 1 8' means there is a link between node 0 and node 1, and the cost of that link is 8.



(4) Message File

Each line means one message, and the format of each message is as follows.

source destination message

ex)

1 0 here is a message from 1 to 0 

- This means that the message was transmitted from node 1 to node 0 and the content of the message was "Here is a message from 1 to 0."



(5) Change File

Changes file shows the state of the network.

ex)

1 3 1 

1 3 -999

- If there are two lines as above, it means that there have been two changes in this network.

- The "1 3 1" in the first line means that the cost of the link connecting node 1 and node 3 has been changed to 1. 

- If the cost of the link is -999, it is considered that there is no link between the nodes. In the second line of the above changes file, "1 3 -999" means that the link between numbers 1 and 3 is broken.



(6) Program

Step 1.

- Read the topology file to configure the network.

- Configure routing tables for each node using the link state or distance vector algorithm.

- Simulates the process of delivering each line of messages in the messages file from sender to receiver through the routing table.

Step 2.

- Apply the state change in the first line of the changes file.

- After applying, the cost of a specific link changes. (There may be a link that didn't exist or a link that was there may be lost.)

- Again, use the link state and distance vector algorithm to update the routing table of each node.

- Simulates the process of delivering each line of messages in the messages file from sender to receiver through the routing table.

Step 3.

- Repeat until the last line of the changes file is reflected in the network.
